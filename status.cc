#include <pybind11/detail/common.h>
#include <pybind11/pybind11.h>

#include <exception>
#include <stdexcept>

// absl_casters handle absl::string_view.
#include "pybind11_abseil/absl_casters.h"
#include "pybind11_abseil/status_utils.h"
#include "third_party/pybind11_protobuf/proto_casters.h"
#include "util/task/canonical_errors.h"
#include "util/task/status.h"
#include "util/task/status.proto.h"

namespace pybind11 {
namespace google {

// Returns false if status_or represents a non-ok status object, and true in all
// other cases (including the case that this is passed a non-status object).
bool IsOk(handle status_or) {
  detail::make_caster<util::Status> caster;
  // "Not a status" means "ok" for StatusOr.
  if (!caster.load(status_or, true)) return true;
  return static_cast<util::Status &>(caster).ok();
}

PYBIND11_MODULE(status, m) {
  enum_<absl::StatusCode>(m, "StatusCode")
      .value("OK", absl::StatusCode::kOk)
      .value("CANCELLED", absl::StatusCode::kCancelled)
      .value("UNKNOWN", absl::StatusCode::kUnknown)
      .value("INVALID_ARGUMENT", absl::StatusCode::kInvalidArgument)
      .value("DEADLINE_EXCEEDED", absl::StatusCode::kDeadlineExceeded)
      .value("NOT_FOUND", absl::StatusCode::kNotFound)
      .value("ALREADY_EXISTS", absl::StatusCode::kAlreadyExists)
      .value("PERMISSION_DENIED", absl::StatusCode::kPermissionDenied)
      .value("RESOURCE_EXHAUSTED", absl::StatusCode::kResourceExhausted)
      .value("FAILED_PRECONDITION", absl::StatusCode::kFailedPrecondition)
      .value("ABORTED", absl::StatusCode::kAborted)
      .value("OUT_OF_RANGE", absl::StatusCode::kOutOfRange)
      .value("UNIMPLEMENTED", absl::StatusCode::kUnimplemented)
      .value("INTERNAL", absl::StatusCode::kInternal)
      .value("UNAVAILABLE", absl::StatusCode::kUnavailable)
      .value("DATA_LOSS", absl::StatusCode::kDataLoss)
      .value("UNAUTHENTICATED", absl::StatusCode::kUnauthenticated);

  class_<util::Status>(m, "Status")
      .def(init())
      .def(init<absl::StatusCode, std::string>())
      .def("ok", &util::Status::ok)
      .def("code", &util::Status::code)
      .def("message", &util::Status::message)
      .def(
          "update",
          (void (util::Status::*)(const util::Status &)) & util::Status::Update,
          arg("other"))
      .def("to_string", &util::Status::ToString)
      .def("__repr__", &util::Status::ToString);

  m.def("is_ok", &IsOk, arg("status_or"),
        "Returns false only if passed a non-ok status; otherwise returns true. "
        "This can be used on the return value of a function which returns a "
        "StatusOr without raising an exception. The .ok() method cannot be "
        "used in this case because an ok status is never returned; instead, a "
        "non-status object is returned, which doesn't have a .ok() method.");

  // Because status_casters has not been included, the functions below will
  // return a wrapped status, not raise an exception.
  // Note that we cannot include status_casters here because that imports this
  // module and therefore would create a circular dependency.

  // Return a status proto.
  m.def("make_status_proto", []() { return util::StatusProto(); });
  // Return a status object from a proto.
  m.def("make_status_from_proto",
        (util::Status(*)(const util::StatusProto &proto)) &
            util::MakeStatusFromProto,
        arg("proto"));
  // Save the given status object to a proto.
  m.def("save_status_to_proto", &util::SaveStatusToProto, arg("status"),
        arg("proto").noconvert());

  // Return canonical errors.
  m.def("aborted_error", &util::AbortedError, arg("message"));
  m.def("already_exists_error", &util::AlreadyExistsError, arg("message"));
  // CanceledError has an overload which takes no arguments, so we must cast it.
  m.def("cancelled_error",
        (util::Status(*)(absl::string_view)) & util::CancelledError,
        arg("message"));
  m.def("data_loss_error", &util::DataLossError, arg("message"));
  m.def("deadline_exceeded_error", &util::DeadlineExceededError,
        arg("message"));
  m.def("failed_precondition_error", &util::FailedPreconditionError,
        arg("message"));
  m.def("internal_error", &util::InternalError, arg("message"));
  m.def("invalid_argument_error", &util::InvalidArgumentError, arg("message"));
  m.def("not_found_error", &util::NotFoundError, arg("message"));
  m.def("out_of_range_error", &util::OutOfRangeError, arg("message"));
  m.def("permission_denied_error", &util::PermissionDeniedError,
        arg("message"));
  m.def("resource_exhausted_error", &util::ResourceExhaustedError,
        arg("message"));
  m.def("unauthenticated_error", &util::UnauthenticatedError, arg("message"));
  m.def("unavailable_error", &util::UnavailableError, arg("message"));
  m.def("unimplemented_error", &util::UnimplementedError, arg("message"));
  m.def("unknown_error", &util::UnknownError, arg("message"));

  // Register the exception.
  static pybind11::exception<StatusNotOk> status_not_ok(m, "StatusNotOk");

  // Register a custom handler which converts a C++ StatusNotOk to a Python
  // StatusNotOk exception and adds the status field.
  register_exception_translator([](std::exception_ptr p) {
    try {
      if (p) std::rethrow_exception(p);
    } catch (const StatusNotOk &e) {
      status_not_ok.attr("status") = cast(e.status());
      status_not_ok(e.what());
    }
  });
}

}  // namespace google
}  // namespace pybind11
