#ifndef PYBIND11_ABSEIL_RAW_PTR_FROM_CAPSULE_H_
#define PYBIND11_ABSEIL_RAW_PTR_FROM_CAPSULE_H_

// Must be first include (https://docs.python.org/3/c-api/intro.html).
#include <Python.h>

#include "absl/status/statusor.h"

namespace pybind11_abseil {
namespace raw_ptr_from_capsule {

// Prefer RawPtrFromCapsule<T>(), to minimize the use of static_casts, and
// to keep them as close as possible to the context establishing type safety
// (via the capsule name).
absl::StatusOr<void*> VoidPtrFromCapsule(PyObject* py_obj, const char* name,
                                         const char* as_capsule_method_name);

// Extract a raw pointer from a PyCapsule or return absl::InvalidArgumentError,
// with a detailed message.
// CAUTION: The returned raw pointer does (of course) not manage the lifetime
//          of the pointee! It is best to use the raw pointer only for the
//          duration of a function call, similar to e.g. std::string::c_str(),
//          but not to store it in any way (e.g. as a data member of a
//          long-lived object).
// If py_obj is a capsule, the capsule name is inspected. If it matches the
// name argument, the raw pointer is returned. Otherwise
// absl::InvalidArgumentError is returned. - Note that name can be nullptr,
// and can match a nullptr capsule name.
// If py_obj is not a capsule, and as_capsule_method_name is given (i.e. it is
// not nullptr), the py_obj method with that name will be called without
// arguments, with the expectation to receive a capsule object in return, from
// which the raw pointer is then extracted exactly as described above.
// A specific error message is generated for every possible error condition
// (most of the code in this function is for error handling).
template <typename T>
absl::StatusOr<T*> RawPtrFromCapsule(PyObject* py_obj, const char* name,
                                     const char* as_capsule_method_name) {
  absl::StatusOr<void*> statusor_void_ptr =
      VoidPtrFromCapsule(py_obj, name, as_capsule_method_name);
  if (!statusor_void_ptr.ok()) {
    return statusor_void_ptr.status();
  }
  return static_cast<T*>(statusor_void_ptr.value());
}

}  // namespace raw_ptr_from_capsule
}  // namespace pybind11_abseil

#endif  // PYBIND11_ABSEIL_RAW_PTR_FROM_CAPSULE_H_
