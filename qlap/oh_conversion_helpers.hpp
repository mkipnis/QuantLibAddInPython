// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn [Python]

#pragma once

namespace py = pybind11;

namespace QuantLibAddinPython
{

template <typename T>
ObjectHandler::property_t property_from(const py::object& value)
{
    return ObjectHandler::property_t(value.cast<T>());
}

ObjectHandler::property_t property_from_date(const py::object& value)
{
    if (value.is_none())
        return {};

    const QuantLib::Date date = py::isinstance<py::int_>(value)
        ? QuantLib::Date(value.cast<QuantLib::Date::serial_type>())
        : value.cast<QuantLib::Date>();

    return ObjectHandler::property_t(
        static_cast<long>(date.serialNumber())
    );
}

inline ObjectHandler::property_t property_from_python(const py::object& value)
{
    if (value.is_none())
        return {};

    // Check bool before int because Python bool is a subclass of int.
    if (py::isinstance<py::bool_>(value))
        return property_from<bool>(value);

    if (py::isinstance<py::int_>(value))
        return property_from<long>(value);

    if (py::isinstance<py::float_>(value))
        return property_from<double>(value);

    if (py::isinstance<py::str>(value))
        return property_from<std::string>(value);

    if (py::isinstance<QuantLib::Date>(value))
        return property_from_date(value);

    throw py::type_error(
        "Unsupported Python type for ObjectHandler::property_t"
    );

}

}