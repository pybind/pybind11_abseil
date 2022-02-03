#ifndef PYBIND11_ABSEIL_STATUS_NOT_OK_EXCEPTION_H_
#define PYBIND11_ABSEIL_STATUS_NOT_OK_EXCEPTION_H_

#include <exception>
#include <string>
#include <utility>

#include "absl/status/status.h"

namespace pybind11 {
namespace google {

// Exception class which represents a non-ok status.
//
// This is in the pybind::google namespace because it was originally created to
// use with pybind11, but it does NOT depend on the pybind11 library.
class StatusNotOk : public std::exception {
 public:
  StatusNotOk(absl::Status&& status)
      : status_(std::move(status)), what_(status_.ToString()) {}
  StatusNotOk(const absl::Status& status)
      : status_(status), what_(status_.ToString()) {}
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
