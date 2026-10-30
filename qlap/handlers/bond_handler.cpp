// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn/Python

#include <handlers/bond_handler.hpp>

#include <handlers/cashflows.hpp>
#include <json_conversion_helpers.hpp>

#include <oh/repository.hpp>
#include <oh/enumerations/typefactory.hpp>

#include <qlo/bonds.hpp>
#include <qlo/conversions/all.hpp>

#include <ql/quantlib.hpp>

#include <stdexcept>

namespace QuantLibAddinPython
{

json::value handleBond( const InstrumentPtr& instrument, const json::value& input )
{
    boost::shared_ptr<QuantLib::Bond> bond;
    instrument->getLibraryObject(bond);

    if (!bond)
        throw std::runtime_error("Unable to retrieve Bond");

    const auto& obj = input.as_object();

    auto daycounter = get_or_default<std::string>(obj, "DayCounter", "Actual/365 (Fixed)");
    auto compounding = get_or_default<std::string>(obj, "Compounding", "Continuous");
    auto frequency = get_or_default<std::string>(obj, "Frequency", "Annual");

    QuantLib::DayCounter dc = ObjectHandler::Create<QuantLib::DayCounter>()(daycounter);
    QuantLib::Compounding comp = ObjectHandler::Create<QuantLib::Compounding>()(compounding);
    QuantLib::Frequency freq = ObjectHandler::Create<QuantLib::Frequency>()(frequency);

    json::object results;

    QuantLib::Bond::Price cleanPrice;
    QuantLib::Rate yield = 0.0;

    if (auto* price = obj.if_contains("price"); price && !price->is_null())
    {
        cleanPrice = QuantLib::Bond::Price( boost::json::value_to<double>(*price), QuantLib::Bond::Price::Clean );
        yield = bond->yield(cleanPrice, dc, comp, freq);
    }
    else if (auto* y = obj.if_contains("yield"); y && !y->is_null())
    {
        yield = boost::json::value_to<double>(*y);
        cleanPrice = QuantLib::Bond::Price( bond->cleanPrice(yield, dc, comp, freq), QuantLib::Bond::Price::Clean );
    }
    else
    {
        cleanPrice = QuantLib::Bond::Price( bond->cleanPrice(), QuantLib::Bond::Price::Clean );
        yield = bond->yield(dc, comp, freq);
    }

    results["cleanPrice"] = cleanPrice.amount();
    results["yield"] = yield;

    QuantLib::InterestRate yieldRate( yield, dc, comp, freq );

    results["basisPointValue"] = QuantLib::BondFunctions::basisPointValue( *bond, yieldRate );
    results["accruedDays"] = QuantLib::BondFunctions::accruedDays( *bond );
    results["duration"] = QuantLib::BondFunctions::duration( *bond, yieldRate );
    results["convexity"] = QuantLib::BondFunctions::convexity( *bond, yieldRate );

    results["cashflows"] = handle_leg(bond->cashflows());

    if (auto* zcurve = obj.if_contains("zcurve"); zcurve && !zcurve->is_null())
    {
        const auto curveId = boost::json::value_to<std::string>( *zcurve );
        boost::shared_ptr<QuantLibAddin::YieldTermStructure> curveObject;
        ObjectHandler::Repository::instance().retrieveObject( curveObject, curveId );

        if (!curveObject)
            throw std::runtime_error("Unable to retrieve yield curve: " + curveId);

        boost::shared_ptr<QuantLib::YieldTermStructure> curve;
        curveObject->getLibraryObject(curve);

        results["zspread"] = QuantLib::BondFunctions::zSpread( *bond, cleanPrice, curve, dc, comp, freq );
    }

    json::object output;

    output["results"] = results;
    output["status"] = "success";
    output["handler"] = "Bond";
    output["input"] = input;

    return output;
}

}
