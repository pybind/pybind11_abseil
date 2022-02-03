#ifndef PYBIND11_ABSEIL_CHECK_STATUS_MODULE_IMPORTED_H_
#define PYBIND11_ABSEIL_CHECK_STATUS_MODULE_IMPORTED_H_

#include <pybind11/pybind11.h>

#include "absl/status/status.h"

namespace pybind11 {
namespace google {
namespace internal {

// Returns true if the status module has been imported.
inline bool IsStatusModuleImported() {
  return detail::get_type_info(typeid(absl::Status));
}

// In debug mode, throws a type error if the proto module is not imported.
// No-opt if NDEBUG is defined, and inlined so the compiler can optimize it out.
inline void CheckStatusModuleImported() {
#ifndef NDEBUG
  if (!IsStatusModuleImported())
    throw type_error(
        "Status module has not been imported. Did you call ::pybind11::google"
        "::ImportStatusModule() in your PYBIND11_MODULE definition?");
#endif
}

}  // namespace internal
}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_CHECK_STATUS_MODULE_IMPORTED_H_
