#ifndef PYBIND11_ABSEIL_OK_STATUS_SINGLETON_LIB_H_
#define PYBIND11_ABSEIL_OK_STATUS_SINGLETON_LIB_H_

#include <Python.h>

#include "absl/status/status.h"

namespace pybind11_abseil {

const absl::Status* OkStatusSingleton();
PyObject* PyOkStatusSingleton();

}  // namespace pybind11_abseil

#endif  // PYBIND11_ABSEIL_OK_STATUS_SINGLETON_LIB_H_
