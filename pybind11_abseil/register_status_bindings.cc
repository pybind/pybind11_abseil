#include "pybind11_abseil/register_status_bindings.h"

#include <pybind11/embed.h>
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

enum struct InitFromTag { capsule, serialized };

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

// TODO(b/225205409): Move to utility library.
// To avoid clobbering potentially critical error messages with
// `UnicodeDecodeError`.
str decode_utf8_replace(absl::string_view s) {
  PyObject* u = PyUnicode_DecodeUTF8(s.data(), s.size(), "replace");
  if (u == nullptr) {
    throw error_already_set();
  }
  return reinterpret_steal<str>(u);
}

}  // namespace

namespace internal {

void RegisterStatusBindings(module m) {
  enum_<InitFromTag>(m, "InitFromTag")
      .value("capsule", InitFromTag::capsule)
      .value("serialized", InitFromTag::serialized);

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
      .def(init([](InitFromTag init_from_tag, const object& obj) {
             switch (init_from_tag) {
               case InitFromTag::capsule: {
                 PyErr_SetString(PyExc_NotImplementedError,
                                 "Implemented in pending child cl/474244219.");
                 throw error_already_set();
               }
               case InitFromTag::serialized: {
                 auto state = cast<tuple>(obj);
                 if (len(state) != 3) {
                   throw value_error(
                       absl::StrCat("Unexpected len(state) == ", len(state),
                                    " [", __FILE__, ":", __LINE__, "]"));
                 }
                 auto code = cast<absl::StatusCode>(state[0]);
                 auto message = cast<std::string>(state[1]);
                 auto all_payloads = cast<tuple>(state[2]);
                 auto status = std::unique_ptr<absl::Status>{
                     new absl::Status{code, message}};
                 for (auto ap_item_obj : all_payloads) {
                   auto ap_item_tup = cast<tuple>(ap_item_obj);
                   if (len(ap_item_tup) != 2) {
                     throw value_error(absl::StrCat(
                         "Unexpected len(tuple) == ", len(ap_item_tup),
                         " where (type_url, payload) is expected [", __FILE__,
                         ":", __LINE__, "]"));
                   }
                   auto type_url = cast<absl::string_view>(ap_item_tup[0]);
                   auto payload = cast<absl::string_view>(ap_item_tup[1]);
                   status->SetPayload(type_url, absl::Cord(payload));
                 }
                 return status;
               }
             }
             throw std::runtime_error(absl::StrCat(
                 "Meant to be unreachable [", __FILE__, ":", __LINE__, "]"));
           }),
           arg("init_from_tag"), arg("obj"))
      .def(init<absl::StatusCode, std::string>(), arg("code"), arg("msg"))
      .def("ok", &absl::Status::ok)
      .def("code", &absl::Status::code)
      .def("code_int",
           [](const absl::Status& self) {
             return static_cast<int>(self.code());
           })
      .def("message",
           [](const absl::Status& self) {
             return decode_utf8_replace(self.message());
           })
      .def("message_bytes",
           [](const absl::Status& self) {
             return bytes(self.message().data(), self.message().size());
           })
      .def("update",
           (void(absl::Status::*)(const absl::Status&)) & absl::Status::Update,
           arg("other"))
      .def("to_string",
           [](const absl::Status& s) {
             return decode_utf8_replace(s.ToString());
           })
      .def("__repr__",
           [](const absl::Status& s) {
             return decode_utf8_replace(s.ToString());
           })
      .def("to_string_status_not_ok",
           [](const absl::Status& s) {
             return decode_utf8_replace(s.ToString());
           })
      .def_static("OkStatus", DoNotThrowStatus(&absl::OkStatus))
      .def("raw_code", &absl::Status::raw_code)
      .def("CanonicalCode",
           [](const absl::Status& self) {
             return static_cast<int>(self.code());
           })
      .def("error_message",
           [](const absl::Status& self) {
             return decode_utf8_replace(self.message());
           })
      .def("IgnoreError", &absl::Status::IgnoreError)
      .def("SetPayload",
           [](absl::Status& self, absl::string_view type_url,
              absl::string_view payload) {
             self.SetPayload(type_url, absl::Cord(payload));
           })
      .def("ErasePayload",
           [](absl::Status& self, absl::string_view type_url) {
             return self.ErasePayload(type_url);
           })
      .def("AllPayloads",
           [](const absl::Status& s) {
             list key_value_pairs;
             s.ForEachPayload([&key_value_pairs](absl::string_view key,
                                                 const absl::Cord& value) {
               key_value_pairs.append(make_tuple(bytes(std::string(key)),
                                                 bytes(std::string(value))));
             });
             // Make the order deterministic, especially long-term.
             key_value_pairs.attr("sort")();
             return tuple(key_value_pairs);
           })
      .def(
          "__reduce_ex__",
          [](const object& self, int) {
            return make_tuple(
                self.attr("__class__"),
                make_tuple(InitFromTag::serialized,
                           make_tuple(self.attr("code")(),
                                      self.attr("message_bytes")(),
                                      self.attr("AllPayloads")())));
          },
          arg("protocol") = -1);

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

  pybind11::exec(R"(
      class StatusNotOk(Exception):
        def __init__(self, status):
          assert status is not None
          assert not status.ok()
          self._status = status
          Exception.__init__(self, status.to_string_status_not_ok())

        @property
        def status(self):
          return self._status

        @property
        def code(self):
          # code is int by choice. Sorry it would be a major API break to make
          # this an enum.
          return self._status.raw_code()

        @property
        def message(self):
          return self._status.message()
      )",
                 m.attr("__dict__"), m.attr("__dict__"));
  static pybind11::object PyStatusNotOk = m.attr("StatusNotOk");

  // Register a custom handler which converts a C++ StatusNotOk to a
  // PyStatusNotOk.
  register_exception_translator([](std::exception_ptr p) {
    try {
      if (p) std::rethrow_exception(p);
    } catch (const StatusNotOk& e) {
      PyErr_SetObject(
          PyStatusNotOk.ptr(),
          PyStatusNotOk(google::NoThrowStatus<absl::Status>(e.status())).ptr());
    }
  });

  m.def("BuildStatusNotOk", [](int code, const std::string& msg) {
    return PyStatusNotOk(google::NoThrowStatus<absl::Status>(
        absl::Status(static_cast<absl::StatusCode>(code), msg)));
  });
}

}  // namespace internal
}  // namespace google
}  // namespace pybind11
