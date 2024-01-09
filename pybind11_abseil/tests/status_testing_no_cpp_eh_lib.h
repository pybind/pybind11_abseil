// This code is meant to be built with C++ exception handling disabled:
// the whole point of absl::Status, absl::StatusOr is to provide an alternative
// to C++ exception handling.

#ifndef PYBIND11_ABSEIL_TESTS_STATUS_TESTING_NO_CPP_EH_LIB_H_
#define PYBIND11_ABSEIL_TESTS_STATUS_TESTING_NO_CPP_EH_LIB_H_

#include <Python.h>

#include <functional>
#include <string>

#include "absl/log/absl_check.h"
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

inline absl::StatusOr<PyObject *> ReturnStatusOrPyObjectPtr(bool is_ok) {
  if (is_ok) {
    return PyLong_FromLongLong(2314L);
  }
  return absl::InvalidArgumentError("!is_ok");
}

inline std::string PassStatusOrPyObjectPtr(
    const absl::StatusOr<PyObject *> &obj) {
  if (!obj.ok()) {
    return "!obj.ok()@" + std::string(obj.status().message());
  }
  if (PyTuple_CheckExact(obj.value())) {
    return "is_tuple";
  }
  return "!is_tuple";
}

inline std::string CallCallbackWithStatusOrPyObjectPtrReturn(
    const std::function<absl::StatusOr<PyObject *>(std::string)> &cb,
    std::string cb_arg) {
  // Implicitly take ownership of Python reference:
  absl::StatusOr<PyObject *> cb_result = cb(cb_arg);
  std::string result = PassStatusOrPyObjectPtr(cb_result);
  if (cb_result.ok()) {
    ABSL_CHECK_NE(Py_REFCNT(cb_result.value()), 0);
    Py_DECREF(cb_result.value());  // Release owned reference.
  }
  return result;
}

}  // namespace status_testing_no_cpp_eh
}  // namespace pybind11_abseil_tests

#endif  // PYBIND11_ABSEIL_TESTS_STATUS_TESTING_NO_CPP_EH_LIB_H_
