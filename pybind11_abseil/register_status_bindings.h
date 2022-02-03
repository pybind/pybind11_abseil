#ifndef PYBIND11_ABSEIL_REGISTER_STATUS_BINDINGS_H_
#define PYBIND11_ABSEIL_REGISTER_STATUS_BINDINGS_H_

#include <pybind11/pybind11.h>

namespace pybind11 {
namespace google {
namespace internal {

// Registers the bindings for the status types in the given module. Can only
// be called once; subsequent calls will fail due to duplicate registrations.
void RegisterStatusBindings(module m);

}  // namespace internal
}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_REGISTER_STATUS_BINDINGS_H_
