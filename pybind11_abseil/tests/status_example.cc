#include <pybind11/pybind11.h>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "pybind11_abseil/status_casters.h"

namespace pybind11 {
namespace test {

struct IntValue {
  IntValue() = default;
  IntValue(int value_in) : value(value_in) {}
  int value;
};

class TestClass {
 public:
  absl::Status MakeStatus(absl::StatusCode code, const std::string& text = "") {
    return absl::Status(code, text);
  }

  absl::Status MakeStatusConst(absl::StatusCode code,
                               const std::string& text = "") const {
    return absl::Status(code, text);
  }

  absl::StatusOr<int> MakeFailureStatusOr(absl::StatusCode code,
                                          const std::string& text = "") {
    return absl::Status(code, text);
  }
};

bool CheckStatus(const absl::Status& status, absl::StatusCode code) {
  return status.code() == code;
}

absl::Status ReturnStatus(absl::StatusCode code, const std::string& text = "") {
  return absl::Status(code, text);
}

pybind11::object ReturnStatusManualCast(absl::StatusCode code,
                                        const std::string& text = "") {
  return pybind11::cast(google::DoNotThrowStatus(absl::Status(code, text)));
}

const absl::Status& ReturnStatusRef(absl::StatusCode code,
                                    const std::string& text = "") {
  static absl::Status static_status;
  static_status = absl::Status(code, text);
  return static_status;
}

const absl::Status* ReturnStatusPtr(absl::StatusCode code,
                                    const std::string& text = "") {
  static absl::Status static_status;
  static_status = absl::Status(code, text);
  return &static_status;
}

absl::StatusOr<int> ReturnFailureStatusOr(absl::StatusCode code,
                                          const std::string& text = "") {
  return absl::Status(code, text);
}

pybind11::object ReturnFailureStatusOrManualCast(absl::StatusCode code,
                                                 const std::string& text = "") {
  return pybind11::cast(google::DoNotThrowStatus(absl::Status(code, text)));
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

class IntGetter {
 public:
  virtual ~IntGetter() { }
  virtual absl::StatusOr<int> Get(int i) const = 0;
};

class PyIntGetter : public IntGetter {
 public:
  using IntGetter::IntGetter;
  absl::StatusOr<int> Get(int i) const override {
    PYBIND11_OVERRIDE_PURE(absl::StatusOr<int>, IntGetter, Get, i);
  }
};

absl::StatusOr<int> CallGetRedirectToPython(IntGetter* ptr, int i) {
  if (ptr) {
    return ptr->Get(i);
  }
  return absl::InvalidArgumentError(
      "Function parameter should not be nullptr.");
}

PYBIND11_MODULE(status_example, m) {
  auto status_module = pybind11::google::ImportStatusModule();
  m.attr("StatusNotOk") = status_module.attr("StatusNotOk");

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

  // absl::Status bindings
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

  // absl::StatusOr bindings
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
  m.def("return_status_or_pointer", []() {
    static absl::StatusOr<int>* ptr = new absl::StatusOr<int>(42);
    return ptr;
  });
  m.def("return_failure_status_or_pointer", []() {
    static absl::StatusOr<int>* ptr =
        new absl::StatusOr<int>(absl::InvalidArgumentError("Uh oh!"));
    return ptr;
  });

  class_<IntGetter, PyIntGetter>(m, "IntGetter")
      .def(init())
      .def("Get", &IntGetter::Get);
  m.def("call_get_redirect_to_python", &CallGetRedirectToPython,
        arg("ptr"), arg("i"));

  // Needed to exercise raw_code() != code().
  m.def("status_from_int_code", [](int code, const std::string& msg) {
    return google::DoNotThrowStatus(
        absl::Status(static_cast<absl::StatusCode>(code), msg));
  });
}

}  // namespace test
}  // namespace pybind11
