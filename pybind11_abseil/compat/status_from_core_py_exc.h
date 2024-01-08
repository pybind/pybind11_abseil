#ifndef PYBIND11_ABSEIL_COMPAT_STATUS_FROM_CORE_PY_EXC_H_
#define PYBIND11_ABSEIL_COMPAT_STATUS_FROM_CORE_PY_EXC_H_

#include "absl/status/status.h"
#include "pybind11_abseil/compat/py_base_utilities.h"

namespace pybind11_abseil::compat {

absl::Status StatusFromFetchedExc(
    const py_base_utilities::PyExcFetchGivenErrOccurred& fetched);

}  // namespace pybind11_abseil::compat

#endif  // PYBIND11_ABSEIL_COMPAT_STATUS_FROM_CORE_PY_EXC_H_
