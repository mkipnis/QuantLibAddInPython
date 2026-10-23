#pragma once

#include <boost/json.hpp>
#include <boost/shared_ptr.hpp>

#include <functional>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include <qlo/qladdin.hpp>

namespace QuantLibAddinPython
{

namespace json = boost::json;

using InstrumentPtr =
    boost::shared_ptr<QuantLibAddin::Instrument>;

using InstrumentPricingHandler =
    std::function<json::value(
        const InstrumentPtr&,
        const json::value&)>;

using InstrumentPricingMatcher =
    std::function<bool(const InstrumentPtr&)>;

struct RegisteredInstrumentHandler
{
    InstrumentPricingMatcher matches;
    InstrumentPricingHandler handler;
};

std::vector<RegisteredInstrumentHandler>&
getInstrumentHandlers();

template <typename T, typename F>
void registerInstrumentHandler(F&& handler)
{
    getInstrumentHandlers().push_back({
        [](const InstrumentPtr& instrument)
        {
            return static_cast<bool>(
                boost::dynamic_pointer_cast<T>(instrument)
            );
        },
        std::forward<F>(handler)
    });
}

json::value dispatchInstrument(
    const InstrumentPtr& instrument,
    const json::value& input);

}
