
#pragma once

#include <instrument_handlers.hpp>

namespace QuantLibAddinPython
{

json::value handleOIS(
    const InstrumentPtr& instrument,
    const json::value& input);

}
