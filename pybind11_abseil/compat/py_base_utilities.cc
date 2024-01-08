#include "pybind11_abseil/compat/py_base_utilities.h"

#include <Python.h>

#include <string>

#include "absl/log/absl_check.h"
#include "absl/log/absl_log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"

namespace pybind11_abseil::compat::py_base_utilities {

const char* ClassName(PyObject* obj) {
  if (PyType_Check(obj)) {
    return reinterpret_cast<PyTypeObject*>(obj)->tp_name;
  }
  return Py_TYPE(obj)->tp_name;
}

std::string PyStrAsStdString(PyObject* str_object) {
  Py_ssize_t value_size = 0;
  const char* value = PyUnicode_AsUTF8AndSize(str_object, &value_size);
  ABSL_CHECK(value != nullptr) << "FAILED: PyUnicode_AsUTF8AndSize()";
  return std::string(value, value_size);
}

PyExcFetchMaybeErrOccurred::PyExcFetchMaybeErrOccurred() {
  if (PyErr_Occurred()) {
    PyErr_Fetch(&p_type_, &p_value_, &p_traceback_);
  }
}

PyExcFetchGivenErrOccurred::PyExcFetchGivenErrOccurred() {
  ABSL_CHECK(Type()) << "PyErr_Occurred() was false at entry.";
}

PyExcFetchMaybeErrOccurred::~PyExcFetchMaybeErrOccurred() {
  Py_XDECREF(p_type_);
  Py_XDECREF(p_value_);
  Py_XDECREF(p_traceback_);
}

void PyExcFetchGivenErrOccurred::NormalizeException() {
  PyErr_NormalizeException(&p_type_, &p_value_, &p_traceback_);
}

bool PyExcFetchGivenErrOccurred::Matches(PyObject* exc) const {
  ABSL_CHECK(p_type_ != nullptr);
  return PyErr_GivenExceptionMatches(p_type_, exc);
}

std::string PyExcFetchMaybeErrOccurred::FlatMessage() const {
  if (p_type_ == nullptr) {
    return "PyErr_Occurred() FALSE";
  }
  ABSL_CHECK(p_value_ != nullptr) << ClassName(p_type_);
  PyObject* str = PyObject_Str(p_value_);
  if (str == nullptr) {
    ABSL_LOG(FATAL) << "FAILED (while processing " << ClassName(p_type_)
                    << "): PyObject_Str(p_value_) ["
                    << PyExcFetchMaybeErrOccurred().FlatMessage() << "]";
  }
  Py_ssize_t utf8_str_size = 0;
  const char* utf8_str = PyUnicode_AsUTF8AndSize(str, &utf8_str_size);
  if (utf8_str == nullptr) {
    ABSL_LOG(FATAL) << "FAILED (while processing " << ClassName(p_type_)
                    << "): PyUnicode_AsUTF8AndSize() ["
                    << PyExcFetchMaybeErrOccurred().FlatMessage() << "]";
  }
  auto msg = absl::StrCat(ClassName(p_type_), ": ",
                          absl::string_view(utf8_str, utf8_str_size));
  Py_DECREF(str);
  return msg;
}

PyObject* ImportModuleOrDie(const char* fq_mod) {
  PyObject* imported_mod = PyImport_ImportModule(fq_mod);
  if (imported_mod == nullptr || PyErr_Occurred()) {
    ABSL_LOG(FATAL) << "FAILED: PyImport_ImportModule(\"" << fq_mod << "\") ["
                    << PyExcFetchMaybeErrOccurred().FlatMessage() << "]";
  }
  return imported_mod;
}

PyObject* ImportObjectOrDie(const char* fq_mod, const char* mod_attr) {
  PyObject* imported_mod = ImportModuleOrDie(fq_mod);
  PyObject* imported_obj = PyObject_GetAttrString(imported_mod, mod_attr);
  Py_DECREF(imported_mod);
  if (imported_obj == nullptr || PyErr_Occurred()) {
    ABSL_LOG(FATAL) << "FAILED: PyObject_GetAttrString(\"" << mod_attr
                    << "\") [" << PyExcFetchMaybeErrOccurred().FlatMessage()
                    << "]";
  }
  return imported_obj;
}

PyObject* ImportModuleOrReturnNone(const char* fq_mod) {
  ABSL_CHECK(!PyErr_Occurred());
  PyObject* imported_mod = PyImport_ImportModule(fq_mod);
  if (PyErr_Occurred()) {
    ABSL_CHECK(imported_mod == nullptr);
    PyErr_Clear();
    Py_RETURN_NONE;
  }
  ABSL_CHECK(imported_mod != nullptr);
  return imported_mod;
}

PyObject* ImportObjectOrReturnNone(const char* fq_mod, const char* mod_attr) {
  PyObject* imported_mod = ImportModuleOrReturnNone(fq_mod);
  if (imported_mod == Py_None) {
    return imported_mod;
  }
  PyObject* imported_obj = PyObject_GetAttrString(imported_mod, mod_attr);
  Py_DECREF(imported_mod);
  if (PyErr_Occurred()) {
    ABSL_CHECK(imported_obj == nullptr);
    PyErr_Clear();
    Py_RETURN_NONE;
  }
  ABSL_CHECK(imported_obj != nullptr);
  return imported_obj;
}

}  // namespace pybind11_abseil::compat::py_base_utilities
