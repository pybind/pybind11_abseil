#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "third_party/pybind11/include/pybind11/pybind11.h"
#include "third_party/pybind11/include/pybind11/stl.h"
#include "pybind11_abseil/compat/status_from_cpp_exc.h"

namespace pybind11_abseil::compat {

namespace {

absl::Status CatchesErrorAlreadySet() {
  pybind11::gil_scoped_acquire gil;
  return CallAndCatchPybind11Exceptions([&]() -> absl::Status {
    PyErr_SetString(PyExc_ValueError, "test error");
    throw pybind11::error_already_set();
  });
}

absl::Status CatchesBuiltinException() {
  pybind11::gil_scoped_acquire gil;
  return CallAndCatchPybind11Exceptions(
      [&]() -> absl::Status { throw pybind11::value_error("test error 2"); });
}

absl::Status CatchesCastError() {
  pybind11::gil_scoped_acquire gil;
  return CallAndCatchPybind11Exceptions([&]() -> absl::Status {
    pybind11::object o = pybind11::int_(1);
    o.cast<std::string>();
    return absl::OkStatus();
  });
}

absl::Status CatchesStatusNotOk() {
  pybind11::gil_scoped_acquire gil;
  return CallAndCatchPybind11Exceptions([&]() -> absl::Status {
    return absl::ResourceExhaustedError("test error 3");
  });
}

// This function shows that Python and pybind11 exceptions can be caught, and
// transferred to code that follows Google conventions (i.e. never use C++
// exceptions, but use absl::Status instead).
std::vector<std::string> CollectVariousStatuses() {
  return {
      absl::StrCat(CatchesErrorAlreadySet()),
      absl::StrCat(CatchesBuiltinException()),
      absl::StrCat(CatchesCastError()),
      absl::StrCat(CatchesStatusNotOk()),
  };
}

}  // namespace

PYBIND11_MODULE(status_from_cpp_exc_test_lib, m) {
  m.def("CollectVariousStatuses", &CollectVariousStatuses);
}

}  // namespace pybind11_abseil::compat
