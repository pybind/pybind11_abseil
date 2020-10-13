// A pybind11 module which uses the status caster but does not call
// ImportStatusModule (as it should), to test the behavior in that case.
#include <pybind11/pybind11.h>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "pybind11_abseil/status_casters.h"

namespace pybind11 {
namespace test {

absl::Status ReturnStatus() { return absl::InternalError("test"); }
absl::StatusOr<int> ReturnStatusOr() { return absl::InternalError("test"); }

PYBIND11_MODULE(missing_import, m) {
  m.def("returns_status", &ReturnStatus);
  m.def("returns_status_or", &ReturnStatusOr);
}

}  // namespace test
}  // namespace pybind11
