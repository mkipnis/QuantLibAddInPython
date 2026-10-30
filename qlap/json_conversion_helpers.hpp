// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn/Python

#pragma once

#include <boost/json.hpp>
#include <boost/shared_ptr.hpp>
#include <pybind11/pybind11.h>


namespace py = pybind11;
namespace json = boost::json;

namespace QuantLibAddinPython
{
// Convert Python dict -> Boost.JSON.
inline json::value to_boost_json(const py::dict& input)
{
    const std::string text = py::str(
        py::module_::import("json").attr("dumps")(input)
    );

    return json::parse(text);
}

// Convert Boost.JSON -> Python dict.
inline py::dict to_python_dict(const json::value& input)
{
    const std::string text = json::serialize(input);

    py::object result =
        py::module_::import("json").attr("loads")(text);

    if (!py::isinstance<py::dict>(result))
        throw py::type_error("JSON result must be an object");

    return result.cast<py::dict>();
}

template<typename T, typename D>
T get_or_default(
    const boost::json::object& obj,
    const std::string& key,
    const D& default_value)
{
    auto it = obj.find(key);

    if (it == obj.end() || it->value().is_null())
        return T(default_value);

    return boost::json::value_to<T>(it->value());
}

}
