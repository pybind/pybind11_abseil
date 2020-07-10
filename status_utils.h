// Utility classes functions for util::Status objects.
// These are needed by both the status module and casters.
#ifndef PYBIND11_ABSEIL_STATUS_UTILS_H_
#define PYBIND11_ABSEIL_STATUS_UTILS_H_

#include <pybind11/pybind11.h>

#include <exception>

#include "util/task/status.h"
#include "util/task/statusor.h"

namespace pybind11 {
namespace google {

// Use a macro so this path can be set from the commandline when building.
#ifndef PYBIND11_ABSEIL_IMPORT_PATH
#define PYBIND11_ABSEIL_IMPORT_PATH google3.third_party.pybind11_abseil
#endif

// The import path will need to change if this is ever open sourced, so
// providing a helper function makes that a single point change.
constexpr char kGoogle3UtilsStatusModule[] =
    PYBIND11_TOSTRING(PYBIND11_ABSEIL_IMPORT_PATH) ".status";

// Imports the status module.
inline void ImportStatusModule() { module::import(kGoogle3UtilsStatusModule); }

// Wrapper type to signal to the type_caster that a non-ok status should not
// be converted into an object rather than a thrown exception.
template <typename StatusType>
struct NoThrowStatus {
  NoThrowStatus(StatusType status_in)
      : status(std::forward<StatusType>(status_in)) {}
  StatusType status;
};

// Convert a util::Status(Or) into a NoThrowStatus.
template <typename StatusType>
NoThrowStatus<StatusType> DoNotThrowStatus(StatusType status) {
  return NoThrowStatus<StatusType>(std::forward<StatusType>(status));
}
// Convert a function returning a util::Status(Or) into a function
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
  StatusNotOk(util::Status&& status)
      : status_(std::move(status)), what_(status_.ToString()) {}
  StatusNotOk(const util::Status& status)
      : status_(status), what_(status_.ToString()) {}
  const util::Status& status() const { return status_; }
  const char* what() const noexcept override { return what_.c_str(); }

 private:
  util::Status status_;
  std::string what_;
};

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_STATUS_UTILS_H_
