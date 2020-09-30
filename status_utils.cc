#include "pybind11_abseil/status_utils.h"

#include <pybind11/pybind11.h>

#include "pybind11_abseil/absl_casters.h"

namespace pybind11 {
namespace google {
namespace {

// Returns false if status_or represents a non-ok status object, and true in all
// other cases (including the case that this is passed a non-status object).
bool IsOk(handle status_or) {
  detail::make_caster<absl::Status> caster;
  // "Not a status" means "ok" for StatusOr.
  if (!caster.load(status_or, true)) return true;
  return static_cast<absl::Status &>(caster).ok();
}

}  // namespace

void RegisterStatusBindings(module m) {
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

  class_<absl::Status>(m, "Status")
      .def(init())
      .def(init<absl::StatusCode, std::string>())
      .def("ok", &absl::Status::ok)
      .def("code", &absl::Status::code)
      .def("message", &absl::Status::message)
      .def(
          "update",
          (void (absl::Status::*)(const absl::Status &)) & absl::Status::Update,
          arg("other"))
      .def("to_string", &absl::Status::ToString)
      .def("__repr__", &absl::Status::ToString);

  m.def("is_ok", &IsOk, arg("status_or"),
        "Returns false only if passed a non-ok status; otherwise returns true. "
        "This can be used on the return value of a function which returns a "
        "StatusOr without raising an exception. The .ok() method cannot be "
        "used in this case because an ok status is never returned; instead, a "
        "non-status object is returned, which doesn't have a .ok() method.");

  // status_casters.h has not been included, so the functions below will
  // return a wrapped status, not raise an exception.

  // Return canonical errors.
  m.def("aborted_error", &absl::AbortedError, arg("message"));
  m.def("already_exists_error", &absl::AlreadyExistsError, arg("message"));
  // CanceledError has an overload which takes no arguments, so we must cast it.
  m.def("cancelled_error",
        (absl::Status(*)(absl::string_view)) & absl::CancelledError,
        arg("message"));
  m.def("data_loss_error", &absl::DataLossError, arg("message"));
  m.def("deadline_exceeded_error", &absl::DeadlineExceededError,
        arg("message"));
  m.def("failed_precondition_error", &absl::FailedPreconditionError,
        arg("message"));
  m.def("internal_error", &absl::InternalError, arg("message"));
  m.def("invalid_argument_error", &absl::InvalidArgumentError, arg("message"));
  m.def("not_found_error", &absl::NotFoundError, arg("message"));
  m.def("out_of_range_error", &absl::OutOfRangeError, arg("message"));
  m.def("permission_denied_error", &absl::PermissionDeniedError,
        arg("message"));
  m.def("resource_exhausted_error", &absl::ResourceExhaustedError,
        arg("message"));
  m.def("unauthenticated_error", &absl::UnauthenticatedError, arg("message"));
  m.def("unavailable_error", &absl::UnavailableError, arg("message"));
  m.def("unimplemented_error", &absl::UnimplementedError, arg("message"));
  m.def("unknown_error", &absl::UnknownError, arg("message"));

  // Register the exception.
  static pybind11::exception<StatusNotOk> status_not_ok(m, "StatusNotOk");

  // Register a custom handler which converts a C++ StatusNotOk to a Python
  // StatusNotOk exception and adds the status field.
  register_exception_translator([](std::exception_ptr p) {
    try {
      if (p) std::rethrow_exception(p);
    } catch (StatusNotOk& e) {
      auto rvalue_e = std::move(e);
      status_not_ok.attr("status") = cast(rvalue_e.status());
      status_not_ok(rvalue_e.what());
    }
  });
}

}  // namespace google
}  // namespace pybind11
