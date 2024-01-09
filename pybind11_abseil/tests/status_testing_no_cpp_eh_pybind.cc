#if true  // go/pybind11_include_order
#include <pybind11/pybind11.h>
#endif

#include <pybind11/functional.h>
#include <pybind11/type_caster_pyobject_ptr.h>

#include "pybind11_abseil/import_status_module.h"
#include "pybind11_abseil/status_caster.h"
#include "pybind11_abseil/statusor_caster.h"
#include "pybind11_abseil/tests/status_testing_no_cpp_eh_lib.h"

namespace pybind11_abseil_tests {
namespace status_testing_no_cpp_eh {

PYBIND11_MODULE(status_testing_no_cpp_eh_pybind, m) {
  pybind11::google::ImportStatusModule();

  m.def("CallCallbackWithStatusReturn", &CallCallbackWithStatusReturn);
  m.def("CallCallbackWithStatusOrIntReturn",
        &CallCallbackWithStatusOrIntReturn);
  m.def("CallCallbackWithStatusOrObjectReturn",
        &CallCallbackWithStatusOrObjectReturn,
        pybind11::return_value_policy::take_ownership);
  m.def("GenerateErrorStatusNotOk", &GenerateErrorStatusNotOk);
}

}  // namespace status_testing_no_cpp_eh
}  // namespace pybind11_abseil_tests
