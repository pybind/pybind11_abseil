// Utility classes functions for absl::Status objects.
// These are needed by both the status module and casters.
#ifndef PYBIND11_ABSEIL_STATUS_UTILS_H_
#define PYBIND11_ABSEIL_STATUS_UTILS_H_

#include <pybind11/pybind11.h>

#include <exception>
#include <stdexcept>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "pybind11_abseil/no_throw_status.h"
#include "pybind11_abseil/status_not_ok_exception.h"

namespace pybind11 {
namespace google {

// Registers the bindings for the status types in the given module. Can only
// be called once; subsequent calls will fail due to duplicate registrations.
void RegisterStatusBindings(module m);

// If modifying the functions below, see
// g3doc/pybind11_abseil/README.md#importing-the-status-module

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

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_STATUS_UTILS_H_
