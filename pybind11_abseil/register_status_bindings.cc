#include "pybind11_abseil/register_status_bindings.h"

#include <pybind11/pybind11.h>

#include <exception>
#include <functional>
#include <string>
#include <utility>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "pybind11_abseil/absl_casters.h"
#include "pybind11_abseil/no_throw_status.h"
#include "pybind11_abseil/status_caster.h"
#include "pybind11_abseil/status_not_ok_exception.h"

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

void def_status_factory(
    module& m, const char* name,
    absl::Status (*absl_status_factory)(absl::string_view message)) {
  m.def(
      name,
      [absl_status_factory](absl::string_view message) {
        return DoNotThrowStatus(absl_status_factory(message));
      },
      arg("message"));
}

object BuildStatusNotOk(handle PyStatusNotOk, const StatusNotOk& e) {
  object py_e = PyStatusNotOk(e.what());
  py_e.attr("status") = cast(google::NoThrowStatus<absl::Status>(e.status()));
  // code is int by choice. Sorry it would be a major API break to make this an
  // enum. Use status.code() to obtain the enum.
  py_e.attr("code") = cast(e.status().raw_code());
  py_e.attr("message") = cast(e.status().message());
  return py_e;
}

}  // namespace

namespace internal {

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
      .def("code_int", [](const absl::Status& self) {
          return static_cast<int>(self.code());
      })
      .def("message", &absl::Status::message)
      .def(
          "update",
          (void (absl::Status::*)(const absl::Status &)) & absl::Status::Update,
          arg("other"))
      .def("to_string", [](const absl::Status& s) { return s.ToString(); })
      .def("__repr__", [](const absl::Status& s) { return s.ToString(); })
      .def_static("OkStatus", DoNotThrowStatus(&absl::OkStatus))
      .def("raw_code", &absl::Status::raw_code)
      .def("CanonicalCode", [](const absl::Status& self) {
          return static_cast<int>(self.code());
      })
      .def("error_message", &absl::Status::message)
      .def("IgnoreError", &absl::Status::IgnoreError);

  m.def("is_ok", &IsOk, arg("status_or"),
        "Returns false only if passed a non-ok status; otherwise returns true. "
        "This can be used on the return value of a function which returns a "
        "StatusOr without raising an exception. The .ok() method cannot be "
        "used in this case because an ok status is never returned; instead, a "
        "non-status object is returned, which doesn't have a .ok() method.");

  // Return canonical errors.
  def_status_factory(m, "aborted_error", WrapAbortedError);
  def_status_factory(m, "already_exists_error", WrapAlreadyExistsError);
  def_status_factory(m, "cancelled_error", WrapCancelledError);
  def_status_factory(m, "data_loss_error", WrapDataLossError);
  def_status_factory(m, "deadline_exceeded_error", WrapDeadlineExceededError);
  def_status_factory(m, "failed_precondition_error",
                     WrapFailedPreconditionError);
  def_status_factory(m, "internal_error", WrapInternalError);
  def_status_factory(m, "invalid_argument_error", WrapInvalidArgumentError);
  def_status_factory(m, "not_found_error", WrapNotFoundError);
  def_status_factory(m, "out_of_range_error", WrapOutOfRangeError);
  def_status_factory(m, "permission_denied_error", WrapPermissionDeniedError);
  def_status_factory(m, "resource_exhausted_error", WrapResourceExhaustedError);
  def_status_factory(m, "unauthenticated_error", WrapUnauthenticatedError);
  def_status_factory(m, "unavailable_error", WrapUnavailableError);
  def_status_factory(m, "unimplemented_error", WrapUnimplementedError);
  def_status_factory(m, "unknown_error", WrapUnknownError);

  // Generate the Python exception type.
  static exception<StatusNotOk> PyStatusNotOk(m, "StatusNotOk");

  // Register a custom handler which converts a C++ StatusNotOk to a
  // PyStatusNotOk.
  register_exception_translator([](std::exception_ptr p) {
    try {
      if (p) std::rethrow_exception(p);
    } catch (const StatusNotOk& e) {
      PyErr_SetObject(PyStatusNotOk.ptr(),
                      BuildStatusNotOk(PyStatusNotOk, e).ptr());
    }
  });

  m.def("BuildStatusNotOk", [](int code, const std::string& msg) {
    return BuildStatusNotOk(
        PyStatusNotOk,
        StatusNotOk(absl::Status(static_cast<absl::StatusCode>(code), msg)));
  });
}

}  // namespace internal
}  // namespace google
}  // namespace pybind11
