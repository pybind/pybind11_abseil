// Utility classes functions for absl::Status objects.
// These are needed by both the status module and casters.
#ifndef PYBIND11_ABSEIL_STATUS_UTILS_H_
#define PYBIND11_ABSEIL_STATUS_UTILS_H_

#include <pybind11/pybind11.h>

#include <exception>
#include <stdexcept>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace pybind11 {
namespace google {

// Wrapper type to signal to the type_caster that a non-ok status should not
// be converted into an object rather than a thrown exception.
template <typename StatusType>
struct NoThrowStatus {
  NoThrowStatus(StatusType status_in)
      : status(std::forward<StatusType>(status_in)) {}
  StatusType status;
};

// Convert a absl::Status(Or) into a NoThrowStatus.
template <typename StatusType>
NoThrowStatus<StatusType> DoNotThrowStatus(StatusType status) {
  return NoThrowStatus<StatusType>(std::forward<StatusType>(status));
}
// Convert a function returning a absl::Status(Or) into a function
// returning a NoThrowStatus.
template <typename StatusType, typename... Args>
std::function<NoThrowStatus<StatusType>(Args...)> DoNotThrowStatus(
    std::function<StatusType(Args...)> f) {
  return [f = std::move(f)](Args... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>(f(std::forward<Args>(args)...)));
  };
}
template <typename StatusType, typename... Args>
std::function<NoThrowStatus<StatusType>(Args...)> DoNotThrowStatus(
    StatusType (*f)(Args...)) {
  return [f](Args... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>(f(std::forward<Args>(args)...)));
  };
}
template <typename StatusType, typename Class, typename... Args>
std::function<NoThrowStatus<StatusType>(Class*, Args...)> DoNotThrowStatus(
    StatusType (Class::*f)(Args...)) {
  return [f](Class *c, Args... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>((c->*f)(std::forward<Args>(args)...)));
  };
}
template <typename StatusType, typename Class, typename... Args>
std::function<NoThrowStatus<StatusType>(const Class*, Args...)>
DoNotThrowStatus(StatusType (Class::*f)(Args...) const) {
  return [f](const Class* c, Args... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>((c->*f)(std::forward<Args>(args)...)));
  };
}

// Exception to throw when we return a non-ok status.
class StatusNotOk : public std::exception {
 public:
  StatusNotOk(absl::Status&& status)
      : status_(std::move(status)), what_(status_.ToString()) {}
  StatusNotOk(const absl::Status& status)
      : status_(status), what_(status_.ToString()) {}
  const absl::Status& status() const { return status_; }
  const char* what() const noexcept override { return what_.c_str(); }

 private:
  absl::Status status_;
  std::string what_;
};

// Registers the bindings for the status types in the given module. Can only
// be called once; subsequent calls will fail due to duplicate registrations.
void RegisterStatusBindings(module m);

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_STATUS_UTILS_H_
