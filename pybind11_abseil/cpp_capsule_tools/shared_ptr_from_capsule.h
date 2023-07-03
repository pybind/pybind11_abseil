#ifndef PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_SHARED_PTR_FROM_CAPSULE_H_
#define PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_SHARED_PTR_FROM_CAPSULE_H_

// Must be first include (https://docs.python.org/3/c-api/intro.html).
#include <Python.h>

#include <memory>
#include <utility>

#include "absl/status/statusor.h"
#include "pybind11_abseil/cpp_capsule_tools/void_ptr_from_capsule.h"

namespace pybind11_abseil {
namespace cpp_capsule_tools {

// Extract a shared_ptr from a PyCapsule or return absl::InvalidArgumentError,
// with a detailed message.
template <typename T>
absl::StatusOr<std::shared_ptr<T>> SharedPtrFromCapsule(
    PyObject* py_obj, const char* name, const char* as_capsule_method_name) {
  absl::StatusOr<std::pair<PyObject*, void*>> statusor_void_ptr =
      VoidPtrFromCapsule(py_obj, name, as_capsule_method_name);
  if (!statusor_void_ptr.ok()) {
    return statusor_void_ptr.status();
  }
  auto sp = *static_cast<std::shared_ptr<T>*>(statusor_void_ptr.value().second);
  Py_XDECREF(statusor_void_ptr.value().first);
  return sp;
}

}  // namespace cpp_capsule_tools
}  // namespace pybind11_abseil

#endif  // PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_SHARED_PTR_FROM_CAPSULE_H_
