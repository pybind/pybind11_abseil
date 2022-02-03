#include "pybind11_abseil/import_status_module.h"

#include <pybind11/pybind11.h>

#include "absl/status/status.h"
#include "pybind11_abseil/check_status_module_imported.h"
#include "pybind11_abseil/register_status_bindings.h"

namespace pybind11 {
namespace google {

module ImportStatusModule(bool bypass_regular_import) {
  if (!PyGILState_Check()) {
    pybind11_fail("ImportStatusModule() PyGILState_Check() failure.");
  }
  if (bypass_regular_import) {
    auto m = reinterpret_borrow<module>(PyImport_AddModule(
        PYBIND11_TOSTRING(PYBIND11_ABSEIL_STATUS_MODULE_PATH)));
    if (!internal::IsStatusModuleImported()) {
      internal::RegisterStatusBindings(m);
    }
    // else no-op because bindings are already loaded.
    return m;
  }
  return module::import(PYBIND11_TOSTRING(PYBIND11_ABSEIL_STATUS_MODULE_PATH));
}

}  // namespace google
}  // namespace pybind11
