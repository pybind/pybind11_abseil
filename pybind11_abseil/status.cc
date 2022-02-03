#include <pybind11/pybind11.h>

#include "pybind11_abseil/register_status_bindings.h"

namespace pybind11 {
namespace google {

PYBIND11_MODULE(status, m) { internal::RegisterStatusBindings(m); }

}  // namespace google
}  // namespace pybind11
