#ifndef PYBIND11_ABSEIL_COMPAT_STATUS_FROM_CPP_EXC_H_
#define PYBIND11_ABSEIL_COMPAT_STATUS_FROM_CPP_EXC_H_

#include <cassert>

#include "third_party/pybind11/include/pybind11/pytypes.h"
#include "pybind11_abseil/compat/status_from_py_exc.h"

namespace pybind11_abseil::compat {

// This function is intended for C++ exception-aware code like the one using
// pybind11 API, which needs to be called from an exception-free library like
// the majority of Google's C++ codebase.
//
// It is not needed for pybind11 extension modules: After a call to
// `ImportStatusModule()`, code inside a `m.def()` call automatically
// installs exception handlers and performs the same conversion.
//
// This wrapper executes the given function, catches potential C++ exceptions
// and converts them to absl::Status.
//
// The status code depends on the exception type (see
//   GetPyExceptionStatusCodeMap).
// The status message always starts with the Python exception class name.
// The Python traceback is not preserved.
template <typename Func>
inline decltype(std::declval<Func>()()) CallAndCatchPybind11Exceptions(
    Func&& func) {
  assert(PyGILState_Check());
  try {
    return std::forward<Func>(func)();
  } catch (pybind11::error_already_set& e) {
    e.restore();
    return StatusFromPyExcGivenErrOccurred();
  } catch (pybind11::builtin_exception& e) {
    e.set_error();
    return StatusFromPyExcGivenErrOccurred();
  }
}

}  // namespace pybind11_abseil::compat

#endif  // PYBIND11_ABSEIL_COMPAT_STATUS_FROM_CPP_EXC_H_
