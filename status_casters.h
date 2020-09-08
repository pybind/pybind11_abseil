// Type conversion utilities for pybind11 and absl::Status/StatusOr.
//
// Usage:
// 1) Include this file in the .cc file with your bindings.
// 2) Call `pbyind11::google::ImportStatusModule()` in your PYBIND11_MODULE
//    definition.
//
// Supported types:
// - absl::Status- converts a non-ok return status into a python exception.
//   Can be passed as an argument too if you import the status pybind module.
// - absl::StatusOr- converts a non-ok return status into a python exception,
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

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "pybind11_abseil/status_utils.h"

namespace pybind11 {
namespace google {

// The value of PYBIND11_ABSEIL_STATUS_MODULE_PATH will be different depending
// on whether this is being used inside or outside of google3. The value used
// inside of google3 is defined here. Outside of google3, change this value by
// passing "-DPYBIND11_ABSEIL_STATUS_MODULE_PATH=..." on the commandline.
#ifndef PYBIND11_ABSEIL_STATUS_MODULE_PATH
#define PYBIND11_ABSEIL_STATUS_MODULE_PATH \
  pybind11_abseil.status
#endif

// Imports the bindings for the status types. Like regular imports, this
// can be called from any number of different modules; everything after the
// first will be a no-op.
inline module ImportStatusModule() {
  auto m = reinterpret_borrow<module>(PyImport_AddModule(
      PYBIND11_TOSTRING(PYBIND11_ABSEIL_STATUS_MODULE_PATH)));
  if (!detail::get_type_info(typeid(absl::Status))) RegisterStatusBindings(m);
  // else no-op because bindings are already loaded.
  return m;
}

}  // namespace google

namespace detail {

// Convert NoThrowStatus by dispatching to a caster for StatusType with the
// argument throw_exception = false. StatusType should be an absl::Status
// (rvalue, lvalue, reference, or pointer), or an absl::StatusOr value.
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
    // forward those, but to do so, we must strip the const off the InputType.
    return StatusCaster::cast(
        std::forward<StatusType>(const_cast<InputType&>(src).status), policy,
        parent, false);
  }
};

// Convert absl::Status.
template <>
struct type_caster<absl::Status> : public type_caster_base<absl::Status> {
 public:
  // Conversion part 1 (Python->C++) handled by built in caster.
  bool load(handle src, bool convert) {
    google::ImportStatusModule();  // TODO(b/167413620): Eliminate this.
    return type_caster_base<absl::Status>::load(src, convert);
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::Status* src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    if (!src) return none().release();
    return cast_impl<const absl::Status&>(*src, policy, parent,
                                          throw_exception);
  }

  static handle cast(const absl::Status& src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    return cast_impl<const absl::Status&>(src, policy, parent, throw_exception);
  }

  static handle cast(absl::Status&& src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    return cast_impl<absl::Status&&>(std::move(src), policy, parent,
                                     throw_exception);
  }

 private:
  template <typename CType>
  static handle cast_impl(CType src, return_value_policy policy, handle parent,
                          bool throw_exception) {
    google::ImportStatusModule();  // TODO(b/167413620): Eliminate this.
    if (!throw_exception) {
      // Use the built-in/standard pybind11 caster.
      return type_caster_base<absl::Status>::cast(std::forward<CType>(src),
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

// Convert an absl::StatusOr.
template <typename PayloadType>
struct type_caster<absl::StatusOr<PayloadType>> {
 public:
  using PayloadCaster = make_caster<PayloadType>;
  using StatusCaster = make_caster<absl::Status>;
  static constexpr auto name = _("StatusOr[") + PayloadCaster::name + _("]");

  // Conversion part 2 (C++ -> Python).
  static handle cast(absl::StatusOr<PayloadType>&& src,
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
