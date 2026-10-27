// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn [Python]

#pragma once

#include <instrument_handlers.hpp>

namespace QuantLibAddinPython
{

json::value handleGenericInstrument( const InstrumentPtr& instrument,const json::value& input );

}
