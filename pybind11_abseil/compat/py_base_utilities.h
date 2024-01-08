#ifndef PYBIND11_ABSEIL_COMPAT_PY_BASE_UTILITIES_H_
#define PYBIND11_ABSEIL_COMPAT_PY_BASE_UTILITIES_H_

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>

namespace pybind11_abseil::compat::py_base_utilities {

// Equivalent to obj.__class__.__name__ (or obj.__name__ if obj is a class).
const char* ClassName(PyObject* obj);

std::string PyStrAsStdString(PyObject* str_object);

class PyExcFetchMaybeErrOccurred {
 public:
  PyExcFetchMaybeErrOccurred();
  ~PyExcFetchMaybeErrOccurred();

  // Rule of Five completeness:
  PyExcFetchMaybeErrOccurred(const PyExcFetchMaybeErrOccurred& other) = delete;
  PyExcFetchMaybeErrOccurred(PyExcFetchMaybeErrOccurred&& other) = delete;
  PyExcFetchMaybeErrOccurred& operator=(
      const PyExcFetchMaybeErrOccurred& other) = delete;
  PyExcFetchMaybeErrOccurred& operator=(PyExcFetchMaybeErrOccurred&& other) =
      delete;

  std::string FlatMessage() const;

  PyObject* Type() const { return p_type_; }
  PyObject* Value() const { return p_value_; }
  PyObject* TraceBack() const { return p_traceback_; }

 private:
  PyObject* p_type_ = nullptr;
  PyObject* p_value_ = nullptr;
  PyObject* p_traceback_ = nullptr;

  friend class PyExcFetchGivenErrOccurred;
};

class PyExcFetchGivenErrOccurred : public PyExcFetchMaybeErrOccurred {
 public:
  PyExcFetchGivenErrOccurred();

  // WARNING: Calling this method has the potential to mask bugs.
  //          This problem will go away with Python 3.12:
  //          https://github.com/python/cpython/issues/102594
  void NormalizeException();

  bool Matches(PyObject* exc) const;
};

PyObject* ImportModuleOrDie(const char* fq_mod);
PyObject* ImportObjectOrDie(const char* fq_mod, const char* mod_attr);

PyObject* ImportModuleOrReturnNone(const char* fq_mod);
PyObject* ImportObjectOrReturnNone(const char* fq_mod, const char* mod_attr);

}  // namespace pybind11_abseil::compat::py_base_utilities

#endif  // PYBIND11_ABSEIL_COMPAT_PY_BASE_UTILITIES_H_
