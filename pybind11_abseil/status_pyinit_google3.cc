#include <Python.h>
#include <pybind11/pybind11.h>

#include "pybind11_abseil/register_status_bindings.h"

namespace {

PyObject* this_module_init() noexcept {
  PYBIND11_CHECK_PYTHON_VERSION
  PYBIND11_ENSURE_INTERNALS_READY
  static pybind11::module_::module_def module_def_status;
  auto m = pybind11::module_::create_extension_module("status", nullptr,
                                                      &module_def_status);
  try {
    pybind11::google::internal::RegisterStatusBindings(m);
    return m.ptr();
  }
  PYBIND11_CATCH_INIT_EXCEPTIONS
}

}  // namespace

extern "C" PyObject*
GooglePyInit_google3_third__party_pybind11__abseil_status() {
  return this_module_init();
}
