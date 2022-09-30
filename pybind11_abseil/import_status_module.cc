#include "pybind11_abseil/import_status_module.h"

#include <pybind11/pybind11.h>

namespace pybind11 {
namespace google {

module_ ImportStatusModule(bool bypass_regular_import) {
  if (!PyGILState_Check()) {
    pybind11_fail("ImportStatusModule() PyGILState_Check() failure.");
  }
  if (bypass_regular_import) {
    throw std::runtime_error(
        "ImportStatusModule(bypass_regular_import=true) is no longer supported."
        " Please change the calling code to"
        " call this function without arguments.");
  }
  return module_::import(PYBIND11_TOSTRING(PYBIND11_ABSEIL_STATUS_MODULE_PATH));
}

}  // namespace google
}  // namespace pybind11
