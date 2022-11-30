#include "pybind11_abseil/ok_status_singleton_lib.h"

#include <Python.h>

#include "absl/status/status.h"

namespace pybind11_abseil {

const absl::Status* OkStatusSingleton() {
  static const absl::Status* singleton = new absl::Status();
  return singleton;
}

PyObject* PyOkStatusSingleton() {
  static bool first_call = true;
  static PyObject* py_singleton = nullptr;
  if (first_call) {
    PyObject* imported_mod =
        PyImport_ImportModule("pybind11_abseil.status");
    if (imported_mod == nullptr) {
      PyErr_Clear();
      py_singleton =
          PyCapsule_New(const_cast<absl::Status*>(OkStatusSingleton()),
                        "::absl::Status", nullptr);
      first_call = false;
      if (py_singleton == nullptr) {
        return nullptr;
      }
    } else {
      PyObject* make_fn =
          PyObject_GetAttrString(imported_mod, "_make_py_ok_status_singleton");
      Py_DECREF(imported_mod);
      if (make_fn == nullptr) {
        first_call = false;
        return nullptr;
      }
      PyObject* call_result = PyObject_CallObject(make_fn, nullptr);
      Py_DECREF(make_fn);
      if (call_result == nullptr) {
        first_call = false;
        return nullptr;
      }
      assert(call_result != Py_None);
      py_singleton = call_result;
    }
    first_call = false;
  }
  if (py_singleton == nullptr) {
    PyErr_SetString(PyExc_SystemError,
                    "FAILED: pybind11_abseil::PyOkStatusSingleton()");
    return nullptr;
  }
  Py_INCREF(py_singleton);
  return py_singleton;
}

}  // namespace pybind11_abseil
