// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn/Python

#include <handlers/bond_handler.hpp>

#include <handlers/cashflows.hpp>
#include <json_conversion_helpers.hpp>

#include <oh/repository.hpp>
#include <oh/enumerations/typefactory.hpp>

#include <qlo/vanillaoption.hpp>
#include <qlo/conversions/all.hpp>

#include <ql/quantlib.hpp>

#include <stdexcept>

namespace QuantLibAddinPython
{

template<typename F>
void addGreek(boost::json::object& results, const std::string& name, F&& fn)
{
    try {
        results[name] = fn();
    } catch( const std::exception&)
    {
        results[name] = nullptr;
    }
}

json::value handleVanillaOption( const InstrumentPtr& instrument, const json::value& input )
{
    boost::shared_ptr<QuantLib::VanillaOption> vanillaOption;
    instrument->getLibraryObject(vanillaOption);

    if (!vanillaOption)
       throw std::runtime_error("Unable to retrieve VanillaOption");

    json::object results;

    const auto& obj = input.as_object();

    const auto* price = obj.if_contains("price");

    if (price && !price->is_null() )
    {
       try {
           
            const auto* processId = obj.if_contains("process");
            
            if ( processId && !processId->is_null() )
            {
                double optionPrice = boost::json::value_to<double>(*price);
                
                std::string processName = boost::json::value_to<std::string>(*processId);
                
                boost::shared_ptr<QuantLibAddin::GeneralizedBlackScholesProcess> processObject;
                ObjectHandler::Repository::instance().retrieveObject(processObject, processName);
                
                if (!processObject)
                   throw std::runtime_error("Unable to retrieve GeneralizedBlackScholesProcess object: " + processName);
                
                boost::shared_ptr<QuantLib::GeneralizedBlackScholesProcess> genBsProcess;
                processObject->getLibraryObject(genBsProcess);
                
                const auto* dividendsValue = obj.if_contains("dividends");

                QuantLib::DividendSchedule dividends;
                
                if (dividendsValue && dividendsValue->is_object())
                {
                    const auto& dividendsIn = dividendsValue->as_object();

                    for (const auto& [date, amount] : dividendsIn) {
                        auto dateSerial = std::stoll(date);
                        double dividend = amount.as_double();
                        
                        QuantLib::Date dividendDate( static_cast<QuantLib::Date::serial_type>(dateSerial) );
                        
                        auto fixed_dividend = QuantLib::ext::make_shared<QuantLib::FixedDividend>( dividend, dividendDate );
                        
                        dividends.push_back(fixed_dividend);
                    }
                }
                
                results["impliedVolatility"] = vanillaOption->impliedVolatility( optionPrice, genBsProcess, dividends);
            } else {
                throw std::runtime_error("Process Id is not specified");
            }
        }
        catch ( const std::exception& exp )
        {
            results["impliedVolatility"] = nullptr;
            results["impliedVolatilityExpception"] = exp.what();
        }
    }

    addGreek(results, "delta", [&]() { return vanillaOption->delta(); });
    addGreek(results, "deltaForward", [&]() { return vanillaOption->deltaForward(); });
    addGreek(results, "elasticity", [&]() { return vanillaOption->elasticity(); });
    addGreek(results, "gamma", [&]() { return vanillaOption->gamma(); });
    addGreek(results, "theta", [&]() { return vanillaOption->theta(); });
    addGreek(results, "thetaPerDay", [&]() { return vanillaOption->thetaPerDay(); });
    addGreek(results, "vega", [&]() { return vanillaOption->vega(); });
    addGreek(results, "rho", [&]() { return vanillaOption->rho(); });
    addGreek(results, "dividentRho", [&]() { return vanillaOption->dividendRho(); });
    addGreek(results, "strikeSensitivity", [&]() { return vanillaOption->strikeSensitivity(); });
    addGreek(results, "itmCashProbability", [&]() { return vanillaOption->itmCashProbability(); });


    json::object output;

    output["results"] = results;
    output["status"] = "success";
    output["handler"] = "VanillaOption";
    output["input"] = input;

    return output;
}

}
