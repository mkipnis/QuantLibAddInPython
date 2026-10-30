// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn/Python

#include <instrument_handlers.hpp>

#include <stdexcept>

namespace QuantLibAddinPython
{

std::vector<RegisteredInstrumentHandler>&
getInstrumentHandlers()
{
    static std::vector<RegisteredInstrumentHandler> handlers;
    return handlers;
}

json::value dispatchInstrument(
    const InstrumentPtr& instrument,
    const json::value& input)
{
    if (!instrument)
        throw std::runtime_error("Instrument pointer is null");

    const auto& handlers = getInstrumentHandlers();

    // Most recently registered matching handler wins.
    for (auto it = handlers.rbegin();
         it != handlers.rend(); ++it)
    {
        if (it->matches(instrument))
            return it->handler(instrument, input);
    }

    throw std::runtime_error(
        std::string("No handler registered for runtime type: ") +
        typeid(*instrument).name()
    );
}

}
