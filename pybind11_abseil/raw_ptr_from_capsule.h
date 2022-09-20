#ifndef PYBIND11_ABSEIL_RAW_PTR_FROM_CAPSULE_H_
#define PYBIND11_ABSEIL_RAW_PTR_FROM_CAPSULE_H_

// Must be first include (https://docs.python.org/3/c-api/intro.html).
#include <Python.h>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"

namespace pybind11_abseil {
namespace raw_ptr_from_capsule {

// Copied from pybind11/pytypes.h, to decouple from exception handling
// requirement.
// Equivalent to obj.__class__.__name__ (or obj.__name__ if obj is a class).
inline const char* obj_class_name(PyObject* obj) {
  if (Py_TYPE(obj) == &PyType_Type) {
    return reinterpret_cast<PyTypeObject*>(obj)->tp_name;
  }
  return Py_TYPE(obj)->tp_name;
}

inline std::string quoted_name_or_null_indicator(
    const char* name, const char* quote = "\"",
    const char* null_indicator = "NULL") {
  return (name == nullptr ? null_indicator : absl::StrCat(quote, name, quote));
}

template <typename T>
absl::StatusOr<T*> RawPtrFromCapsule(PyObject* py_obj, const char* name,
                                     const char* as_capsule_method_name) {
  // Note: https://docs.python.org/3/c-api/capsule.html:
  //       The pointer argument may not be NULL.
  if (PyCapsule_CheckExact(py_obj)) {
    void* void_ptr = PyCapsule_GetPointer(py_obj, name);
    if (PyErr_Occurred()) {
      PyErr_Clear();
      return absl::InvalidArgumentError(absl::StrCat(
          "obj is a capsule with name ",
          quoted_name_or_null_indicator(PyCapsule_GetName(py_obj)), " but ",
          quoted_name_or_null_indicator(name), " is expected."));
    }
    return static_cast<T*>(void_ptr);
  }
  if (as_capsule_method_name == nullptr) {
    return absl::InvalidArgumentError(
        absl::StrCat(obj_class_name(py_obj), " object is not a capsule."));
  }
  PyObject* from_method =
      PyObject_CallMethod(py_obj, as_capsule_method_name, nullptr);
  if (from_method == nullptr) {
    PyObject *ptype = nullptr, *pvalue = nullptr, *ptraceback = nullptr;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
    PyObject* err_msg_str = PyObject_Str(pvalue);
    std::string err_msg;
    if (err_msg_str == nullptr) {
      PyErr_Clear();
      err_msg = "<message unavailable>";
    } else {
      PyObject* err_msg_bytes =
          PyUnicode_AsEncodedString(err_msg_str, "UTF-8", "replace");
      Py_DECREF(err_msg_str);
      if (err_msg_bytes == nullptr) {
        PyErr_Clear();
        err_msg = "<message unavailable>";
      } else {
        const char* err_msg_char_ptr = PyBytes_AsString(err_msg_bytes);
        if (err_msg_char_ptr == nullptr) {
          PyErr_Clear();
          err_msg = "<message unavailable>";
        } else {
          err_msg = err_msg_char_ptr;
        }
        Py_DECREF(err_msg_bytes);
      }
    }
    return absl::InvalidArgumentError(
        absl::StrCat(obj_class_name(py_obj), ".", as_capsule_method_name,
                     "() call failed: ", obj_class_name(ptype), ": ", err_msg));
  }
  if (!PyCapsule_CheckExact(from_method)) {
    std::string returned_obj_type = obj_class_name(from_method);
    Py_DECREF(from_method);
    return absl::InvalidArgumentError(
        absl::StrCat(obj_class_name(py_obj), ".", as_capsule_method_name,
                     "() returned an object (", returned_obj_type,
                     ") that is not a capsule."));
  }
  void* void_ptr = PyCapsule_GetPointer(from_method, name);
  if (!PyErr_Occurred()) {
    Py_DECREF(from_method);
    return static_cast<T*>(void_ptr);
  }
  PyErr_Clear();
  std::string capsule_name =
      quoted_name_or_null_indicator(PyCapsule_GetName(from_method));
  Py_DECREF(from_method);
  return absl::InvalidArgumentError(
      absl::StrCat(obj_class_name(py_obj), ".", as_capsule_method_name,
                   "() returned a capsule with name ", capsule_name, " but ",
                   quoted_name_or_null_indicator(name), " is expected."));
}

}  // namespace raw_ptr_from_capsule
}  // namespace pybind11_abseil

#endif  // PYBIND11_ABSEIL_RAW_PTR_FROM_CAPSULE_H_
