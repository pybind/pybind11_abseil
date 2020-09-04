#include <pybind11/pybind11.h>

#include "pybind11_abseil/status_utils.h"

namespace pybind11 {
namespace google {

PYBIND11_MODULE(status, m) { RegisterStatusBindings(m); }

}  // namespace google
}  // namespace pybind11
