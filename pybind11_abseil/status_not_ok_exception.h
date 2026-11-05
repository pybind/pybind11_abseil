#ifndef PYBIND11_ABSEIL_STATUS_NOT_OK_EXCEPTION_H_
#define PYBIND11_ABSEIL_STATUS_NOT_OK_EXCEPTION_H_

#include <exception>
#include <string>
#include <utility>

#include "absl/status/status.h"

// pybind11 builds extension modules with hidden symbol visibility. If the
// type_info of an exception class is hidden, an exception thrown in one
// extension module may not match a `catch` for the same class in another (seen
// on macOS), so `StatusNotOk` thrown by a binding would not reach the
// exception translator registered by the status module.
#if defined(__GNUC__) || defined(__clang__)
#define PYBIND11_ABSEIL_EXPORT_EXCEPTION \
  __attribute__((visibility("default")))
#else
#define PYBIND11_ABSEIL_EXPORT_EXCEPTION
#endif

namespace pybind11 {
namespace google {

// Exception class which represents a non-ok status.
//
// This is in the pybind::google namespace because it was originally created to
// use with pybind11, but it does NOT depend on the pybind11 library.
class PYBIND11_ABSEIL_EXPORT_EXCEPTION StatusNotOk
    : public std::exception {
 public:
  StatusNotOk(absl::Status&& status)
      : status_(std::move(status)),
        what_(status_.ToString(absl::StatusToStringMode::kWithEverything)) {}
  StatusNotOk(const absl::Status& status)
      : status_(status),
        what_(status_.ToString(absl::StatusToStringMode::kWithEverything)) {}
  const absl::Status& status() const& { return status_; }
  absl::Status&& status() && { return std::move(status_); }
  const char* what() const noexcept override { return what_.c_str(); }

 private:
  absl::Status status_;
  std::string what_;
};

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_STATUS_NOT_OK_EXCEPTION_H_
