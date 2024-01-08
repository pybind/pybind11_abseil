#ifndef PYBIND11_ABSEIL_COMPAT_STATUS_FROM_PY_EXC_H_
#define PYBIND11_ABSEIL_COMPAT_STATUS_FROM_PY_EXC_H_

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>

#include "absl/status/status.h"
#include "pybind11_abseil/compat/py_base_utilities.h"

namespace pybind11_abseil::compat {

// Caller must ensure that the fetched exc type is "error.StatusNotOk".
absl::Status StatusFromFetchedStatusNotOk(
    const py_base_utilities::PyExcFetchGivenErrOccurred& fetched);

// WARNING: `normalize_exception = true` has the potential to mask bugs.
//          This problem will go away with Python 3.12:
//          https://github.com/python/cpython/issues/102594
absl::Status StatusFromPyExcGivenErrOccurred(bool normalize_exception = false);
absl::Status StatusFromPyExcMaybeErrOccurred(bool normalize_exception = false);

}  // namespace pybind11_abseil::compat

#endif  // PYBIND11_ABSEIL_COMPAT_STATUS_FROM_PY_EXC_H_
