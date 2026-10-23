
#include <handlers/ois_handler.hpp>

#include <handlers/cashflows.hpp>

#include <qlo/overnightindexedswap.hpp>
#include <ql/instruments/overnightindexedswap.hpp>

namespace QuantLibAddinPython
{

json::value handleOIS(
    const InstrumentPtr& instrument,
    const json::value& input)
{
    boost::shared_ptr<QuantLib::OvernightIndexedSwap> ois;
    instrument->getLibraryObject(ois);

    if (!ois)
        throw std::runtime_error(
            "Unable to retrieve OvernightIndexedSwap"
        );

    json::object results;

    results["type"] = ois->type();
    results["nominal"] = ois->nominal();
    results["fixedRate"] = ois->fixedRate();
    results["spread"] = ois->spread();

    results["fairRate"] = ois->fairRate();
    results["fairSpread"] = ois->fairSpread();

    results["overnightLegNPV"] = ois->overnightLegNPV();
    results["fixedLegNPV"] = ois->fixedLegNPV();
    results["overnightLegBPS"] = ois->overnightLegBPS();
    results["fixedLegBPS"] = ois->fixedLegBPS();
    results["NPV"] = ois->NPV();

    results["fixedLeg"] = handle_leg(ois->fixedLeg());
    results["overnightLeg"] = handle_leg(ois->overnightLeg());

    json::object output;

    output["results"] = results;
    output["status"] = "success";
    output["handler"] = "OvernightIndexedSwap";
    output["input"] = input;

    return output;
}

}
