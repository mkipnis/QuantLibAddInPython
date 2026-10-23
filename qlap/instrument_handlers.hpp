#pragma once

#include <boost/json.hpp>
#include <boost/shared_ptr.hpp>

#include <pybind11/pybind11.h>

#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include <oh/repository.hpp>
#include <qlo/qladdin.hpp>
#include <qlo/overnightindexedswap.hpp>
#include <qlo/bonds.hpp>

#include <ql/quantlib.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/thirty360.hpp>
#include <ql/compounding.hpp>
#include <ql/time/frequency.hpp>

#include <oh/property.hpp>
#include <oh/processor.hpp>
#include <oh/enumerations/typefactory.hpp>
#include <qlo/conversions/all.hpp>


#include "cashflows.hpp"

namespace QuantLibAddinPython
{


using InstrumentPtr =  boost::shared_ptr<QuantLibAddin::Instrument>;
using InstrumentPricingHandler = std::function<json::value( const InstrumentPtr&, const json::value&)>;
using InstrumentPricingMatcher = std::function<bool(const InstrumentPtr&)>;


struct RegisteredInstrumentHandler
{
    InstrumentPricingMatcher matches;
    InstrumentPricingHandler handler;
};

// Registry of handlers.
inline std::vector<RegisteredInstrumentHandler>& getInstrumentHandlers()
{
    static std::vector<RegisteredInstrumentHandler> handlers;

    return handlers;
}

// Register a handler for a type, including derived types.
template <typename T, typename F>
void registerInstrumentHandler(F&& handler)
{
    getInstrumentHandlers().push_back({
        [](const InstrumentPtr& instrument) {
            return static_cast<bool>(
                boost::dynamic_pointer_cast<T>(instrument)
            );
        },
        std::forward<F>(handler)
    });
}

// Dispatch to the most recently registered matching handler.
inline json::value dispatchInstrument(const InstrumentPtr& instrument, const json::value& input)
{
    if (!instrument)
        throw std::runtime_error("Instrument pointer is null");

    const auto& handlers = getInstrumentHandlers();

    // Reverse order lets specific handlers take precedence
    // over handlers registered earlier, such as Instrument.
    for (auto it = handlers.rbegin(); it != handlers.rend(); ++it)
    {
        if (it->matches(instrument))
            return it->handler(instrument, input);
    }

    throw std::runtime_error(
        std::string("No handler registered for runtime type: ") +
        typeid(*instrument).name()
    );
}


// Register handlers once, before dispatching.
inline void registerDefaultHandlers()
{
    static const bool registered = [] {

        // Generic handler. Keep this registered first.
        registerInstrumentHandler<QuantLibAddin::Instrument>
        (
            [](const InstrumentPtr&, const json::value& input)
            {
                json::object output;
                output["status"] = "success";
                output["handler"] = "Instrument";
                output["input"] = input;
                return json::value(output);
            }
        );

        // OvernightIndexedSwap handler.
        registerInstrumentHandler<QuantLibAddin::OvernightIndexedSwap>
        (
            [](const InstrumentPtr& instrument, const json::value& input)
            {

                boost::shared_ptr<QuantLib::OvernightIndexedSwap> ois_ptr;
                instrument->getLibraryObject(ois_ptr);
            
                json::object results;
                
                results["type"] = ois_ptr->type();
                results["nominal"] = ois_ptr->nominal();
                results["fixedRate"] = ois_ptr->fixedRate();
                results["spread"] = ois_ptr->spread();

                results["fairRate"] = ois_ptr->fairRate();
                results["fairSpread"] = ois_ptr->fairSpread();
                
                results["overnightLegNPV"] = ois_ptr->overnightLegNPV();
                results["fixedLegNPV"] = ois_ptr->fixedLegNPV();
                results["overnightLegBPS"] = ois_ptr->overnightLegBPS();
                results["fixedLegBPS"] = ois_ptr->fixedLegBPS();
                results["NPV"] = ois_ptr->NPV();
                
                results["fixedLeg"] = handle_leg(ois_ptr->fixedLeg());
                results["overnightLeg"] = handle_leg(ois_ptr->overnightLeg());
                
                json::object output;
                
                output["results"] = results;
                
                output["status"] = "success";
                output["handler"] = "OvernightIndexedSwap";
                output["input"] = input;

                // Add swap-specific calculations here.

                return json::value(output);
            }
        );

        // Bond handler.
        registerInstrumentHandler<QuantLibAddin::Bond>
        (
            [](const InstrumentPtr& instrument, const json::value& input)
            {

                boost::shared_ptr<QuantLib::Bond> bond_ptr;
                instrument->getLibraryObject(bond_ptr);
                
                auto daycounter = get_or_default<std::string>(input.as_object(), "DayCounter", "Actual/365 (Fixed)");
                auto compounding = get_or_default<std::string>(input.as_object(), "Compounding", "Continuous");
                auto frequency = get_or_default<std::string>(input.as_object(), "Frequency", "Annual");
                
                QuantLib::DayCounter DayCounterEnum = ObjectHandler::Create<QuantLib::DayCounter>()(daycounter);
                QuantLib::Compounding CompoundingEnum = ObjectHandler::Create<QuantLib::Compounding>()(compounding);
                QuantLib::Frequency FrequencyEnum =ObjectHandler::Create<QuantLib::Frequency>()(frequency);
                
                json::object results;
                
                QuantLib::Bond::Price clean_price;
                QuantLib::Rate yield = 0.0;

                if (auto* input_price = input.as_object().if_contains("price"); input_price && !input_price->is_null())
                {
                    // Price -> yield
                    const double price_value = boost::json::value_to<double>(*input_price);

                    clean_price = QuantLib::Bond::Price( price_value, QuantLib::Bond::Price::Clean );

                    yield = bond_ptr->yield( clean_price, DayCounterEnum, CompoundingEnum, FrequencyEnum );
                }
                else if (auto* input_yield = input.as_object().if_contains("yield"); input_yield && !input_yield->is_null())
                {
                    // Yield -> price
                    yield = boost::json::value_to<double>(*input_yield);

                    const double clean_price_val = bond_ptr->cleanPrice( yield, DayCounterEnum, CompoundingEnum, FrequencyEnum );

                    clean_price = QuantLib::Bond::Price( clean_price_val, QuantLib::Bond::Price::Clean );
                }
                else
                {
                    // Calculate price and yield using the bond's pricing setup
                    const double clean_price_val = bond_ptr->cleanPrice();

                    clean_price = QuantLib::Bond::Price( clean_price_val, QuantLib::Bond::Price::Clean);

                    yield = bond_ptr->yield( DayCounterEnum, CompoundingEnum, FrequencyEnum );
                }
                // Boost.JSON requires primitive values, not QuantLib::Bond::Price.
                results["cleanPrice"] = clean_price.amount();
                results["yield"] = yield;
                
                QuantLib::InterestRate yield_rate(yield, DayCounterEnum, CompoundingEnum, FrequencyEnum);
                
                results["basisPointValue"] = QuantLib::BondFunctions::basisPointValue(*bond_ptr, yield_rate );
                results["accruedDays"] = QuantLib::BondFunctions::accruedDays(*bond_ptr);
                results["duration"] = QuantLib::BondFunctions::duration(*bond_ptr, yield_rate);
                results["convexity"] =  QuantLib::BondFunctions::convexity(*bond_ptr, yield_rate);
                
                results["cashflows"] = handle_leg(bond_ptr->cashflows());

                
                if (auto* input_zcurve = input.as_object().if_contains("zcurve"); input_zcurve && !input_zcurve->is_null())
                {
                    // Z-Spread
                    auto z_curve_name = boost::json::value_to<std::string>(*input_zcurve);
                    boost::shared_ptr<QuantLibAddin::YieldTermStructure> z_curve;
                    ObjectHandler::Repository::instance().retrieveObject(z_curve, z_curve_name);
                    
                    boost::shared_ptr<QuantLib::YieldTermStructure> z_curve_lib_object;
                    z_curve->getLibraryObject(z_curve_lib_object);

                    results["zspread"]  = QuantLib::BondFunctions::zSpread(*bond_ptr, clean_price,
                                                                           z_curve_lib_object,
                                                                           DayCounterEnum, CompoundingEnum, FrequencyEnum);
                }
                
                json::object output;
                
                output["results"] = results;
                
                output["status"] = "success";
                output["handler"] = "Bond";
                output["input"] = input;

                return json::value(output);
            }
         );

        return true;
        }();

        (void)registered;
    }

}
