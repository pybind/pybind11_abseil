// Type conversion utilities for pybind11 and absl::Status/StatusOr.
//
// Usage:
// 1) Include this file in the .cc file with your bindings.
// 2) Call `pybind11::google::ImportStatusModule()` in your PYBIND11_MODULE
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
#include <type_traits>
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

// Imports the bindings for the status types. This not thread safe and should
// only be called from a PYBIND11_MODULE definition. If modifying this,
// see g3doc/pybind11_abseil/README.md#importing-the-status-module
inline module ImportStatusModule() {
#if PY_MAJOR_VERSION >= 3
  auto m = reinterpret_borrow<module>(PyImport_AddModule(
      PYBIND11_TOSTRING(PYBIND11_ABSEIL_STATUS_MODULE_PATH)));
  if (!IsStatusModuleImported()) RegisterStatusBindings(m);
  // else no-op because bindings are already loaded.
  return m;
#else
  return module::import(PYBIND11_TOSTRING(PYBIND11_ABSEIL_STATUS_MODULE_PATH));
#endif
}

}  // namespace google

namespace detail {

template <typename StatusType>
struct NoThrowStatusType {
  // NoThrowStatus should only wrap absl::Status or absl::StatusOr.
  using NoThrowAbslStatus = type_caster_base<absl::Status>;
  static constexpr auto name = NoThrowAbslStatus::name;
};

template <typename PayloadType>
struct NoThrowStatusType<absl::StatusOr<PayloadType>> {
  using NoThrowAbslStatus = type_caster_base<absl::Status>;
  static constexpr auto name = _("Union[") + NoThrowAbslStatus::name + _(", ") +
                               make_caster<PayloadType>::name + _("]");
};

// Convert NoThrowStatus by dispatching to a caster for StatusType with the
// argument throw_exception = false. StatusType should be an absl::Status
// (rvalue, lvalue, reference, or pointer), or an absl::StatusOr value.
// Only return values trigger exceptions, so NoThrowStatus has no meaning for
// input values. Therefore only C++->Python casting is supported.
template <typename StatusType>
struct type_caster<google::NoThrowStatus<StatusType>> {
  using InputType = google::NoThrowStatus<StatusType>;
  using StatusCaster = make_caster<StatusType>;
  static constexpr auto name = NoThrowStatusType<StatusType>::name;

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
  static constexpr auto name = _("None");
  //  Convert C++ -> Python.
  static handle cast(const absl::Status* src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    if (!src) return none().release();
    return cast_impl(*src, policy, parent, throw_exception);
  }

  static handle cast(const absl::Status& src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    return cast_impl(src, policy, parent, throw_exception);
  }

  static handle cast(absl::Status&& src, return_value_policy policy,
                     handle parent, bool throw_exception = true) {
    return cast_impl(std::move(src), policy, parent, throw_exception);
  }

 private:
  template <typename CType>
  static handle cast_impl(CType&& src, return_value_policy policy,
                          handle parent, bool throw_exception) {
    google::CheckStatusModuleImported();
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

// Convert absl::StatusOr<T>.
// It isn't possible to specify separate return value policies for the container
// (StatusOr) and the payload. Since StatusOr is processed and not ever actually
// represented in python, the return value policy applies to the payload. Eg, if
// you return a StatusOr<MyObject*> (note the * is inside the StatusOr) with a
// take_ownership return val policy and the status is ok (ie, it has a payload),
// python will take ownership of that payload and free it when it is garbage
// collected.
// However, if you return a StatusOr<MyObject>* (note the * is outside the
// StatusOr rather than inside it now) with a take_ownership return val policy,
// python does not take ownership of the StatusOr and will not free it (because
// again, that policy applies to MyObject, not StatusOr).
template <typename PayloadType>
struct type_caster<absl::StatusOr<PayloadType>> {
 public:
  using PayloadCaster = make_caster<PayloadType>;
  using StatusCaster = make_caster<absl::Status>;
  static constexpr auto name = PayloadCaster::name;

  // Convert C++ -> Python.
  static handle cast(const absl::StatusOr<PayloadType>* src,
                     return_value_policy policy, handle parent,
                     bool throw_exception = true) {
    if (!src) return none().release();
    return cast_impl(*src, policy, parent, throw_exception);
  }

  static handle cast(const absl::StatusOr<PayloadType>& src,
                     return_value_policy policy, handle parent,
                     bool throw_exception = true) {
    return cast_impl(src, policy, parent, throw_exception);
  }

  static handle cast(absl::StatusOr<PayloadType>&& src,
                     return_value_policy policy, handle parent,
                     bool throw_exception = true) {
    return cast_impl(std::move(src), policy, parent, throw_exception);
  }

 private:
  template <typename CType>
  static handle cast_impl(CType&& src, return_value_policy policy,
                          handle parent, bool throw_exception) {
    google::CheckStatusModuleImported();
    if (src.ok()) {
      // Convert and return the payload.
      return PayloadCaster::cast(std::forward<CType>(src).value(), policy,
                                 parent);
    } else {
      // Convert and return the error.
      return StatusCaster::cast(std::forward<CType>(src).status(),
                                return_value_policy::move, parent,
                                throw_exception);
    }
  }
};

}  // namespace detail
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_STATUS_CASTERS_H_
