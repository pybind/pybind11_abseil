#include "pybind11_abseil/compat/status_from_py_exc.h"

#include <Python.h>

#include "absl/log/absl_check.h"
#include "absl/log/absl_log.h"
#include "absl/status/status.h"
#include "pybind11_abseil/compat/py_base_utilities.h"
#include "pybind11_abseil/compat/status_from_core_py_exc.h"
#include "pybind11_abseil/cpp_capsule_tools/raw_ptr_from_capsule.h"

namespace pybind11_abseil::compat {

absl::Status StatusFromFetchedStatusNotOk(
    const py_base_utilities::PyExcFetchGivenErrOccurred& fetched) {
  ABSL_CHECK(fetched.Value() != nullptr);
  PyObject* py_status = nullptr;
  bool py_status_owned = false;
  if (PyTuple_Check(fetched.Value())) {
    Py_ssize_t size = PyTuple_Size(fetched.Value());
    ABSL_CHECK(size == 1) << "Unexpected tuple size from PyErr_Fetch(): "
                          << size;
    py_status = PyTuple_GetItem(fetched.Value(), 0);
  } else {
    py_status = PyObject_GetAttrString(fetched.Value(), "status");
    py_status_owned = true;
  }
  if (py_status == nullptr) {
    ABSL_LOG(FATAL) << "FAILED: Retrieving `StatusNotOk` `status` attribute "
                       "from fetched Python exception ["
                    << fetched.FlatMessage() << "]";
  }
  if (py_status == Py_None) {
    ABSL_LOG(FATAL) << "FAILED: `StatusNotOk` `status` attribute from fetched "
                       "Python exception is `None` ["
                    << fetched.FlatMessage() << "]";
  }
  auto statusor_raw_ptr =
      pybind11_abseil::cpp_capsule_tools::RawPtrFromCapsule<absl::Status>(
          py_status, "::absl::Status", "as_absl_Status");
  if (!statusor_raw_ptr.ok()) {
    ABSL_LOG(FATAL)
        << "FAILED: `StatusNotOk` `status` attribute from fetched Python "
           "exception cannot be converted to an `absl::Status` object ["
        << fetched.FlatMessage() << "]";
  }
  if (py_status_owned) {
    Py_DECREF(py_status);
  }
  return *(statusor_raw_ptr.value());
}

namespace {

PyObject* PyStatusNotOkOrNone() {
  static PyObject* kImportedObj = nullptr;
  if (kImportedObj == nullptr) {
    kImportedObj = py_base_utilities::ImportObjectOrReturnNone(
        "google3.third_party.pybind11_abseil.status", "StatusNotOk");
  }
  return kImportedObj;
}

}  // namespace

absl::Status StatusFromPyExcGivenErrOccurred(bool normalize_exception) {
  // Fetching exc IMMEDIATELY to ensure it does not accidentally get clobbered
  // by Python C API calls while it is being processed (e.g. b/216844827).
  py_base_utilities::PyExcFetchGivenErrOccurred fetched;

  // Regarding "OrNone" in PyStatusNotOkOrNone():
  // If StatusNotOk was not imported somewhere else already, it cannot possibly
  // have been used to raise the exception to be matched here.
  if (fetched.Matches(PyStatusNotOkOrNone())) {
    return StatusFromFetchedStatusNotOk(fetched);
  }

  if (normalize_exception) {
    fetched.NormalizeException();
  }
  return StatusFromFetchedExc(fetched);
}

absl::Status StatusFromPyExcMaybeErrOccurred(bool normalize_exception) {
  if (!PyErr_Occurred()) {
    return absl::OkStatus();
  }
  return StatusFromPyExcGivenErrOccurred(normalize_exception);
}

}  // namespace pybind11_abseil::compat
