cmake_minimum_required(VERSION 3.20)

project(QuantLibAddinPython LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# -----------------------------------------------------------------------
# Dependency build options
# -----------------------------------------------------------------------

set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(QL_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(QL_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(QL_ENABLE_THREAD_SAFE_OBSERVER_PATTERN ON CACHE BOOL "" FORCE)
set(QL_ENABLE_SESSIONS ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL
    "Build dependencies as static libraries" FORCE)

# -----------------------------------------------------------------------
# Python
# -----------------------------------------------------------------------

find_package(
    Python REQUIRED
    COMPONENTS Interpreter Development.Module
)

# -----------------------------------------------------------------------
# pybind11
# -----------------------------------------------------------------------

execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE PYBIND11_CMAKEDIR_RESULT
)

if(NOT PYBIND11_CMAKEDIR_RESULT EQUAL 0)
    message(FATAL_ERROR
        "Could not locate pybind11. Install it with:\n"
        "  ${Python_EXECUTABLE} -m pip install pybind11"
    )
endif()

find_package(pybind11 CONFIG REQUIRED)

# -----------------------------------------------------------------------
# Fetch QuantLibAddIn and its dependencies
#
# The repository root builds:
#   - QuantLib
#   - ObjectHandler
#   - QuantLibObjects
#   - AddinCpp
#
# -----------------------------------------------------------------------

FetchContent_Declare(
    quantlibaddin_repo
    GIT_REPOSITORY https://github.com/mkipnis/QuantLibAddIn.git
    GIT_TAG excel
    GIT_SHALLOW TRUE
)

# Generate the missing QuantLibAddin and AddinCpp sources.
set(RUN_GENSRC ON CACHE BOOL
    "Generate QuantLibAddin source files" FORCE)

FetchContent_MakeAvailable(quantlibaddin_repo)

# -----------------------------------------------------------------------
# Validate dependency targets
# -----------------------------------------------------------------------

if(NOT TARGET QuantLib::QuantLib)
    message(FATAL_ERROR
        "QuantLib::QuantLib was not created by QuantLibAddIn."
    )
endif()

if(NOT TARGET QuantLibObjects)
    message(FATAL_ERROR
        "QuantLibObjects target is unavailable."
    )
endif()

if(NOT TARGET AddinCpp)
    message(FATAL_ERROR
        "AddinCpp target is unavailable."
    )
endif()

if(NOT TARGET ohlib)
    message(FATAL_ERROR
        "ohlib target is unavailable."
    )
endif()

# -----------------------------------------------------------------------
# Python extension module
# -----------------------------------------------------------------------

pybind11_add_module(
    quantlib_addin
    MODULE
    bindings/QuantLibAddinCpp.cpp
)

target_compile_features(
    quantlib_addin
    PRIVATE
    cxx_std_17
)

# -----------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------

target_include_directories(
    quantlib_addin
    PRIVATE
    "${quantlibaddin_repo_SOURCE_DIR}/QuantLibAddin/Addins/Cpp"
    "${quantlibaddin_repo_SOURCE_DIR}/QuantLibAddin"
    "${quantlibaddin_repo_SOURCE_DIR}/ObjectHandler"
)

# -----------------------------------------------------------------------
# Link dependencies
# -----------------------------------------------------------------------

target_link_libraries(
    quantlib_addin
    PRIVATE
    AddinCpp
    QuantLibObjects
    ohlib
    QuantLib::QuantLib
)

# -----------------------------------------------------------------------
# Extension output
# -----------------------------------------------------------------------

set_target_properties(
    quantlib_addin
    PROPERTIES
    OUTPUT_NAME "quantlib_addin"
)

# -----------------------------------------------------------------------
# Install into the Python package
# -----------------------------------------------------------------------

install(
    TARGETS quantlib_addin
    LIBRARY DESTINATION .
    RUNTIME DESTINATION .
)

