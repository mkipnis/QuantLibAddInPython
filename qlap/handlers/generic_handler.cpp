// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn [Python]

#include <handlers/generic_handler.hpp>

namespace QuantLibAddinPython
{

json::value handleGenericInstrument( const InstrumentPtr&, const json::value& input )
{
    json::object output;

    output["status"] = "success";
    output["handler"] = "Instrument";
    output["input"] = input;

    return output;
}

}
