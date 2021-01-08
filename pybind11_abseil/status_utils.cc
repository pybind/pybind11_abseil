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

// Status factory wrappers.
//
// Abseil does not support taking the address of functions, per
// https://abseil.io/about/compatibility.  To avoid breakages, we wrap these
// factory functions so that we can use them unambiguously in the bindings
// below.
absl::Status WrapAbortedError(absl::string_view message) {
  return absl::AbortedError(message);
}
absl::Status WrapAlreadyExistsError(absl::string_view message) {
  return absl::AlreadyExistsError(message);
}
absl::Status WrapCancelledError(absl::string_view message) {
  return absl::CancelledError(message);
}
absl::Status WrapDataLossError(absl::string_view message) {
  return absl::DataLossError(message);
}
absl::Status WrapDeadlineExceededError(absl::string_view message) {
  return absl::DeadlineExceededError(message);
}
absl::Status WrapFailedPreconditionError(absl::string_view message) {
  return absl::FailedPreconditionError(message);
}
absl::Status WrapInternalError(absl::string_view message) {
  return absl::InternalError(message);
}
absl::Status WrapInvalidArgumentError(absl::string_view message) {
  return absl::InvalidArgumentError(message);
}
absl::Status WrapNotFoundError(absl::string_view message) {
  return absl::NotFoundError(message);
}
absl::Status WrapOutOfRangeError(absl::string_view message) {
  return absl::OutOfRangeError(message);
}
absl::Status WrapPermissionDeniedError(absl::string_view message) {
  return absl::PermissionDeniedError(message);
}
absl::Status WrapResourceExhaustedError(absl::string_view message) {
  return absl::ResourceExhaustedError(message);
}
absl::Status WrapUnauthenticatedError(absl::string_view message) {
  return absl::UnauthenticatedError(message);
}
absl::Status WrapUnavailableError(absl::string_view message) {
  return absl::UnavailableError(message);
}
absl::Status WrapUnimplementedError(absl::string_view message) {
  return absl::UnimplementedError(message);
}
absl::Status WrapUnknownError(absl::string_view message) {
  return absl::UnknownError(message);
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
  m.def("aborted_error", &WrapAbortedError, arg("message"));
  m.def("already_exists_error", &WrapAlreadyExistsError, arg("message"));
  m.def("cancelled_error", &WrapCancelledError, arg("message"));
  m.def("data_loss_error", &WrapDataLossError, arg("message"));
  m.def("deadline_exceeded_error", &WrapDeadlineExceededError, arg("message"));
  m.def("failed_precondition_error", &WrapFailedPreconditionError,
        arg("message"));
  m.def("internal_error", &WrapInternalError, arg("message"));
  m.def("invalid_argument_error", &WrapInvalidArgumentError, arg("message"));
  m.def("not_found_error", &WrapNotFoundError, arg("message"));
  m.def("out_of_range_error", &WrapOutOfRangeError, arg("message"));
  m.def("permission_denied_error", &WrapPermissionDeniedError, arg("message"));
  m.def("resource_exhausted_error", &WrapResourceExhaustedError,
        arg("message"));
  m.def("unauthenticated_error", &WrapUnauthenticatedError, arg("message"));
  m.def("unavailable_error", &WrapUnavailableError, arg("message"));
  m.def("unimplemented_error", &WrapUnimplementedError, arg("message"));
  m.def("unknown_error", &WrapUnknownError, arg("message"));

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
