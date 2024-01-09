// This code is meant to be built with C++ exception handling disabled:
// the whole point of absl::Status, absl::StatusOr is to provide an alternative
// to C++ exception handling.

#ifndef PYBIND11_ABSEIL_TESTS_STATUS_TESTING_NO_CPP_EH_LIB_H_
#define PYBIND11_ABSEIL_TESTS_STATUS_TESTING_NO_CPP_EH_LIB_H_

#include <Python.h>

#include <functional>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace pybind11_abseil_tests {
namespace status_testing_no_cpp_eh {

inline std::string CallCallbackWithStatusReturn(
    const std::function<absl::Status()> &cb) {
  absl::Status cb_return_value = cb();
  return cb_return_value.ToString();
}

inline std::string CallCallbackWithStatusOrIntReturn(
    const std::function<absl::StatusOr<int>()> &cb) {
  absl::StatusOr<int> cb_return_value = cb();
  if (cb_return_value.ok()) {
    return std::to_string(cb_return_value.value());
  }
  return cb_return_value.status().ToString();
}

inline PyObject *CallCallbackWithStatusOrObjectReturn(
    const std::function<absl::StatusOr<PyObject *>()> &cb) {
  absl::StatusOr<PyObject*> cb_return_value = cb();
  if (cb_return_value.ok()) {
    return cb_return_value.value();
  }
  return PyUnicode_FromString(cb_return_value.status().ToString().c_str());
}

inline absl::Status GenerateErrorStatusNotOk() {
  return absl::AlreadyExistsError("Something went wrong, again.");
}

}  // namespace status_testing_no_cpp_eh
}  // namespace pybind11_abseil_tests

#endif  // PYBIND11_ABSEIL_TESTS_STATUS_TESTING_NO_CPP_EH_LIB_H_
