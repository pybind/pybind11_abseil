#ifndef PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_RAW_PTR_FROM_CAPSULE_H_
#define PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_RAW_PTR_FROM_CAPSULE_H_

// Must be first include (https://docs.python.org/3/c-api/intro.html).
#include <Python.h>

#include "absl/status/statusor.h"
#include "pybind11_abseil/cpp_capsule_tools/void_ptr_from_capsule.h"

namespace pybind11_abseil {
namespace cpp_capsule_tools {

// Extract a raw pointer from a PyCapsule or return absl::InvalidArgumentError,
// with a detailed message.
// The function arguments are documented under VoidPtrFromCapsule().
// CAUTION: The returned raw pointer does (of course) not manage the lifetime
//          of the pointee! It is best to use the raw pointer only for the
//          duration of a function call, similar to e.g. std::string::c_str(),
//          but not to store it in any way (e.g. as a data member of a
//          long-lived object).
template <typename T>
absl::StatusOr<T*> RawPtrFromCapsule(PyObject* py_obj, const char* name,
                                     const char* as_capsule_method_name) {
  absl::StatusOr<std::pair<PyObject*, void*>> statusor_void_ptr =
      VoidPtrFromCapsule(py_obj, name, as_capsule_method_name);
  if (!statusor_void_ptr.ok()) {
    return statusor_void_ptr.status();
  }
  Py_XDECREF(statusor_void_ptr.value().first);
  return static_cast<T*>(statusor_void_ptr.value().second);
}

}  // namespace cpp_capsule_tools
}  // namespace pybind11_abseil

#endif  // PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_RAW_PTR_FROM_CAPSULE_H_
