
#pragma once

#include <oh_conversion_helpers.hpp>
#include <json_conversion_helpers.hpp>

#include <instrument_handlers.hpp>
#include <register_handlers.hpp>

#include <oh/repository.hpp>

#include <stdexcept>
#include <string>

namespace QuantLibAddinPython
{

inline json::value qlAddinCalculate(
    const std::string& ObjectId,
    const json::value& input)
{
    if (!input.is_object())
        throw std::invalid_argument(
            "Expected a JSON object");

    registerDefaultHandlers();

    InstrumentPtr instrument;

    ObjectHandler::Repository::instance().retrieveObject(
        instrument, ObjectId);

    return dispatchInstrument(instrument, input);
}

}
