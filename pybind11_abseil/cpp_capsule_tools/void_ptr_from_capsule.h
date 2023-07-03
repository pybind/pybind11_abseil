#ifndef PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_VOID_PTR_FROM_CAPSULE_H_
#define PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_VOID_PTR_FROM_CAPSULE_H_

// Must be first include (https://docs.python.org/3/c-api/intro.html).
#include <Python.h>

#include <utility>

#include "absl/status/statusor.h"

namespace pybind11_abseil {
namespace cpp_capsule_tools {

// Helper for higher-level functions (e.g. RawPtrFromCapsule(),
// SharedPtrFromCapsule()).
//
// Arguments:
//   If py_obj is a capsule, the capsule name is inspected. If it matches the
//   name argument, the raw pointer is returned. Otherwise
//   absl::InvalidArgumentError is returned. - Note that name can be nullptr,
//   and can match a nullptr capsule name.
//   If py_obj is not a capsule, and as_capsule_method_name is given (i.e. it
//   is not nullptr), the py_obj method with that name will be called without
//   arguments, with the expectation to receive a capsule object in return,
//   from which the raw pointer is then extracted exactly as described above.
//   A specific error message is generated for every possible error condition
//   (most of the code in this function is for error handling).
// Return value:
//   The PyObject* is nullptr if the input py_obj is a capsule.
//   Otherwise PyObject* is the capsule obtained in the function call.
//   IMPORTANT: It is the responsibility of the caller to call Py_XDECREF().
absl::StatusOr<std::pair<PyObject*, void*>> VoidPtrFromCapsule(
    PyObject* py_obj, const char* name, const char* as_capsule_method_name);

}  // namespace cpp_capsule_tools
}  // namespace pybind11_abseil

#endif  // PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_VOID_PTR_FROM_CAPSULE_H_
