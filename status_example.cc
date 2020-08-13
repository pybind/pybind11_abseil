#include <pybind11/pybind11.h>

#include "absl/status/statusor.h"
#include "pybind11/detail/common.h"
#include "pybind11_abseil/status_casters.h"
#include "util/task/status.h"

namespace pybind11 {
namespace test {

struct IntValue {
  IntValue() = default;
  IntValue(int value_in) : value(value_in) {}
  int value;
};

class TestClass {
 public:
  util::Status MakeStatus(absl::StatusCode code, const std::string& text = "") {
    return util::Status(code, text);
  }

  util::Status MakeStatusConst(absl::StatusCode code,
                               const std::string& text = "") const {
    return util::Status(code, text);
  }

  absl::StatusOr<int> MakeFailureStatusOr(absl::StatusCode code,
                                          const std::string& text = "") {
    return util::Status(code, text);
  }
};

bool CheckStatus(const util::Status& status, absl::StatusCode code) {
  return status.code() == code;
}

util::Status ReturnStatus(absl::StatusCode code, const std::string& text = "") {
  return util::Status(code, text);
}

pybind11::object ReturnStatusManualCast(absl::StatusCode code,
                                        const std::string& text = "") {
  return pybind11::cast(google::DoNotThrowStatus(util::Status(code, text)));
}

const util::Status& ReturnStatusRef(absl::StatusCode code,
                                    const std::string& text = "") {
  static util::Status static_status;
  static_status = util::Status(code, text);
  return static_status;
}

const util::Status* ReturnStatusPtr(absl::StatusCode code,
                                    const std::string& text = "") {
  static util::Status static_status;
  static_status = util::Status(code, text);
  return &static_status;
}

absl::StatusOr<int> ReturnFailureStatusOr(absl::StatusCode code,
                                          const std::string& text = "") {
  return util::Status(code, text);
}

pybind11::object ReturnFailureStatusOrManualCast(absl::StatusCode code,
                                                 const std::string& text = "") {
  return pybind11::cast(google::DoNotThrowStatus(util::Status(code, text)));
}

absl::StatusOr<int> ReturnValueStatusOr(int value) { return value; }

absl::StatusOr<const IntValue*> ReturnPtrStatusOr(int value) {
  static IntValue static_object;
  static_object.value = value;
  return &static_object;
}

absl::StatusOr<std::unique_ptr<IntValue>> ReturnUniquePtrStatusOr(int value) {
  return absl::make_unique<IntValue>(value);
}

PYBIND11_MODULE(status_example, m) {
  class_<IntValue>(m, "IntValue").def_readonly("value", &IntValue::value);

  class_<TestClass>(m, "TestClass")
      .def(init())
      .def("make_status", google::DoNotThrowStatus(&TestClass::MakeStatus),
           arg("code"), arg("text") = "")
      .def("make_status_const",
           google::DoNotThrowStatus(&TestClass::MakeStatusConst), arg("code"),
           arg("text") = "")
      .def("make_failure_status_or",
           google::DoNotThrowStatus(&TestClass::MakeFailureStatusOr),
           arg("code"), arg("text") = "");

  // util::Status bindings
  m.def("check_status", &CheckStatus, arg("status"), arg("code"));
  m.def("return_status", &ReturnStatus, "Raise an error if code is not OK.",
        arg("code"), arg("text") = "");
  m.def("make_status", google::DoNotThrowStatus(&ReturnStatus),
        "Return a status without raising an error, regardless of what it is.",
        arg("code"), arg("text") = "");
  m.def("make_status_manual_cast", ReturnStatusManualCast,
        "Return a status without raising an error, regardless of what it is.",
        arg("code"), arg("text") = "");
  m.def("make_status_ref", google::DoNotThrowStatus(&ReturnStatusRef),
        "Return a reference to a static status value without raising an error.",
        arg("code"), arg("text") = "", return_value_policy::reference);
  m.def("make_status_ptr", google::DoNotThrowStatus(&ReturnStatusPtr),
        "Return a reference to a static status value without raising an error.",
        arg("code"), arg("text") = "", return_value_policy::reference);

  // util::StatusOr bindings
  m.def("return_value_status_or", &ReturnValueStatusOr, arg("value"));
  m.def("return_failure_status_or", &ReturnFailureStatusOr,
        "Raise an error with the given code.", arg("code"), arg("text") = "");
  m.def("make_failure_status_or",
        google::DoNotThrowStatus(&ReturnFailureStatusOr), arg("code"),
        arg("text") = "", "Return a status without raising an error.");
  m.def("make_failure_status_or_manual_cast", &ReturnFailureStatusOrManualCast,
        arg("code"), arg("text") = "", "Return a status.");
  m.def("return_ptr_status_or", &ReturnPtrStatusOr, arg("value"),
        "Return a reference in a status or to a static value.",
        return_value_policy::reference);
  m.def("return_unique_ptr_status_or", &ReturnUniquePtrStatusOr, arg("value"));
}

}  // namespace test
}  // namespace pybind11
