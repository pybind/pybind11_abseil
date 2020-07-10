// Type conversion utilities for pybind11 and util::Status/StatusOr.
//
// Usage: Just include this file in the .cc file with your bindings and add the
// appropriate dependency. Any functions which take or return the supported
// types will have those types automatically converted.
//
// Supported types:
// - util::Status- converts a non-ok return status into a python exception.
//   Can be passed as an argument too if you import the status pybind module.
// - util::StatusOr- converts a non-ok return status into a python exception,
//   otherwise converts/returns the payload. Can only be used as a return value.
//
// For details, see the README.md.
//
// Author: Ken Oslund (kenoslund@)
#ifndef PYBIND11_ABSEIL_STATUS_CASTERS_H_
#define PYBIND11_ABSEIL_STATUS_CASTERS_H_

#include <pybind11/cast.h>
#include <pybind11/pybind11.h>

#include <stdexcept>
#include <utility>

#include "pybind11_abseil/status_utils.h"
#include "util/task/status.h"
#include "util/task/statusor.h"

namespace pybind11 {
namespace detail {

// Convert NoThrowStatus by dispatching to a caster for StatusType with the
// argument throw_exception = false. StatusType should be a util::Status
// (rvalue, lvalue, reference, or pointer), or a util::StatusOr value.
// Only return values trigger exceptions, so NoThrowStatus has no meaning for
// input values. Therefore only C++->Python casting is supported.
template <typename StatusType>
struct type_caster<google::NoThrowStatus<StatusType>> {
  using InputType = google::NoThrowStatus<StatusType>;
  using StatusCaster = make_caster<StatusType>;
  static constexpr auto name = StatusCaster::name;

  // Convert C++->Python.
  static handle cast(const InputType& src, return_value_policy policy,
                     handle parent) {
    // pybind11::cast applies a const qualifier, so this takes a const reference
    // argument. The qualifiers we care about are in StatusType, and we will
    // forward those, but to do so, we must strip the cost off the InputType.
    return StatusCaster::cast(
        std::forward<StatusType>(const_cast<InputType&>(src).status), policy,
        parent, false);
  }
};

// Convert util::Status.
template <>
struct type_caster<util::Status> : public type_caster_base<util::Status> {
 public:
  // Conversion part 1 (Python->C++) handled by built in caster.
  bool load(handle src, bool convert) {
    google::ImportStatusModule();
    return type_caster_base<util::Status>::load(src, convert);
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const util::Status* src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    if (!src) return none().release();
    return cast_impl<const util::Status&>(*src, policy, parent,
                                          throw_exception);
  }

  static handle cast(const util::Status& src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    return cast_impl<const util::Status&>(src, policy, parent, throw_exception);
  }

  static handle cast(util::Status&& src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    return cast_impl<util::Status&&>(std::move(src), policy, parent,
                                     throw_exception);
  }

 private:
  template <typename CType>
  static handle cast_impl(CType src, return_value_policy policy, handle parent,
                          bool throw_exception) {
    google::ImportStatusModule();
    if (!throw_exception) {
      // Use the built-in/standard pybind11 caster.
      return type_caster_base<util::Status>::cast(std::forward<CType>(src),
                                                  policy, parent);
    } else if (!src.ok()) {
      // Convert a non-ok status into an exception.
      throw google::StatusNotOk(std::forward<CType>(src));
    } else {
      // Return none for an ok status.
      return none().release();
    }
  }
};

// Convert a util::StatusOr.
template <typename PayloadType>
struct type_caster<util::StatusOr<PayloadType>> {
 public:
  using PayloadCaster = make_caster<PayloadType>;
  using StatusCaster = make_caster<util::Status>;
  static constexpr auto name = _("StatusOr[") + PayloadCaster::name + _("]");

  // Conversion part 2 (C++ -> Python).
  static handle cast(util::StatusOr<PayloadType>&& src,
                     return_value_policy policy, handle parent,
                     bool throw_exception = true) {
    if (src.ok()) {
      // Convert and return the payload.
      return PayloadCaster::cast(std::forward<PayloadType>(*src), policy,
                                 parent);
    } else {
      // Convert and return the error.
      return StatusCaster::cast(std::move(src.status()),
                                return_value_policy::move, parent,
                                throw_exception);
    }
  }
};

}  // namespace detail
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_STATUS_CASTERS_H_
