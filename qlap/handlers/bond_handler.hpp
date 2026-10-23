
#pragma once

#include <instrument_handlers.hpp>

namespace QuantLibAddinPython
{

json::value handleBond(
    const InstrumentPtr& instrument,
    const json::value& input);

}
