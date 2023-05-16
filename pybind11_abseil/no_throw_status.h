#ifndef PYBIND11_ABSEIL_NO_THROW_STATUS_H_
#define PYBIND11_ABSEIL_NO_THROW_STATUS_H_

#include <functional>
#include <utility>

namespace pybind11 {
namespace google {

// Wrapper type to signal to the type_caster that a non-ok status should not
// be converted into an object rather than a thrown exception. StatusType can
// encapsulate references, e.g. `NoThrowStatus<absl::Status&>`.
template <typename StatusType>
struct NoThrowStatus {
  NoThrowStatus() = default;
  NoThrowStatus(StatusType status_in)
      : status(std::forward<StatusType>(status_in)) {}
  StatusType status;
};

// Convert a absl::Status(Or) into a NoThrowStatus. To use this with references,
// explicitly specify the template parameter rather deducing it, e.g.:
// `return DoNotThrowStatus<const absl::Status&>(my_status);`
// When returning a status by value (by far the most common case), deducing the
// template parameter is fine, e.g.: `return DoNotThrowStatus(my_status);`
template <typename StatusType>
NoThrowStatus<StatusType> DoNotThrowStatus(StatusType status) {
  return NoThrowStatus<StatusType>(std::forward<StatusType>(status));
}
// Convert a function returning a absl::Status(Or) into a function
// returning a NoThrowStatus.
template <typename StatusType, typename... Args>
std::function<NoThrowStatus<StatusType>(Args...)> DoNotThrowStatus(
    std::function<StatusType(Args...)> f) {
  return [f = std::move(f)](Args&&... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>(f(std::forward<Args>(args)...)));
  };
}
template <typename StatusType, typename... Args>
std::function<NoThrowStatus<StatusType>(Args...)> DoNotThrowStatus(
    StatusType (*f)(Args...)) {
  return [f](Args&&... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>(f(std::forward<Args>(args)...)));
  };
}
template <typename StatusType, typename Class, typename... Args>
std::function<NoThrowStatus<StatusType>(Class*, Args...)> DoNotThrowStatus(
    StatusType (Class::*f)(Args...)) {
  return [f](Class* c, Args&&... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>((c->*f)(std::forward<Args>(args)...)));
  };
}
template <typename StatusType, typename Class, typename... Args>
std::function<NoThrowStatus<StatusType>(const Class*, Args...)>
DoNotThrowStatus(StatusType (Class::*f)(Args...) const) {
  return [f](const Class* c, Args&&... args) {
    return NoThrowStatus<StatusType>(
        std::forward<StatusType>((c->*f)(std::forward<Args>(args)...)));
  };
}

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_NO_THROW_STATUS_H_
