#include "pybind11_abseil/compat/status_from_core_py_exc.h"

#include <Python.h>

#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "pybind11_abseil/compat/py_base_utilities.h"

namespace pybind11_abseil::compat {

namespace {

using PyExceptionStatusCodeMapType =
    absl::flat_hash_map<PyObject*, absl::StatusCode>;

const PyExceptionStatusCodeMapType& GetPyExceptionStatusCodeMap() {
  // When making changes here, please review
  // tests/status_from_py_exc_testing_test.py:TAB_StatusFromFetchedExc
  static const auto* kPyExcStatusCodeMap = new PyExceptionStatusCodeMapType(
      {{PyExc_MemoryError, absl::StatusCode::kResourceExhausted},
       {PyExc_NotImplementedError, absl::StatusCode::kUnimplemented},
       {PyExc_KeyboardInterrupt, absl::StatusCode::kAborted},
       {PyExc_SystemError, absl::StatusCode::kInternal},
       {PyExc_SyntaxError, absl::StatusCode::kInternal},
       {PyExc_TypeError, absl::StatusCode::kInvalidArgument},
       {PyExc_ValueError, absl::StatusCode::kOutOfRange},
       {PyExc_LookupError, absl::StatusCode::kNotFound}});
  return *kPyExcStatusCodeMap;
}

}  // namespace

absl::Status StatusFromFetchedExc(
    const py_base_utilities::PyExcFetchGivenErrOccurred& fetched) {
  std::string message = fetched.FlatMessage();
  const PyExceptionStatusCodeMapType& pyexc_status_code_map =
      GetPyExceptionStatusCodeMap();
  for (const auto& it : pyexc_status_code_map) {
    if (fetched.Matches(it.first)) {
      return absl::Status(it.second, message);
    }
  }
  return absl::UnknownError(message);
}

}  // namespace pybind11_abseil::compat
