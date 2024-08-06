// Author: Ken Oslund (kenoslund@)

// IWYU pragma: always_keep // See pybind11/docs/type_caster_iwyu.rst

#ifndef PYBIND11_ABSEIL_STATUSOR_CASTER_H_
#define PYBIND11_ABSEIL_STATUSOR_CASTER_H_

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/type_caster_pyobject_ptr.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "pybind11_abseil/check_status_module_imported.h"
#include "pybind11_abseil/compat/status_from_py_exc.h"
#include "pybind11_abseil/no_throw_status.h"
#include "pybind11_abseil/status_caster.h"

namespace pybind11 {
namespace detail {

template <typename PayloadType>
struct NoThrowStatusType<absl::StatusOr<PayloadType>> {
  using NoThrowAbslStatus = type_caster_base<absl::Status>;
  static constexpr auto name = const_name("Union[") + NoThrowAbslStatus::name +
                               const_name(", ") +
                               make_caster<PayloadType>::name + const_name("]");
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

#if defined(PYBIND11_HAS_RETURN_VALUE_POLICY_PACK)
  using return_value_policy_t = const return_value_policy_pack&;
  PYBIND11_TYPE_CASTER_RVPP(absl::StatusOr<PayloadType>, PayloadCaster::name);
#else
  using return_value_policy_t = return_value_policy;
  PYBIND11_TYPE_CASTER(absl::StatusOr<PayloadType>, PayloadCaster::name);
#endif

  bool load(handle src, bool convert) {
    PayloadCaster payload_caster;
    if (payload_caster.load(src, convert)) {
      value = cast_op<PayloadType>(std::move(payload_caster));
      return true;
    }
    if (src.is_none()) {
      throw cast_error(
          "None is not a valid value for a StatusOr<T> argument.");
    }
    StatusCaster status_caster;
    if (status_caster.load(src, convert)) {
      absl::Status status = cast_op<absl::Status>(std::move(status_caster));
      if (status.ok()) {
        throw cast_error(
            "An OK status is not a valid constructor argument to StatusOr<T>.");
      } else {
        value = status;
      }
      return true;
    }
    return false;
  }

  // Convert C++ -> Python.
  static handle cast(const absl::StatusOr<PayloadType>* src,
                     return_value_policy_t policy, handle parent,
                     bool throw_exception = true) {
    if (!src) return none().release();
    return cast_impl(*src, policy, parent, throw_exception);
  }

  static handle cast(const absl::StatusOr<PayloadType>& src,
                     return_value_policy_t policy, handle parent,
                     bool throw_exception = true) {
    return cast_impl(src, policy, parent, throw_exception);
  }

  static handle cast(absl::StatusOr<PayloadType>&& src,
                     return_value_policy_t policy, handle parent,
                     bool throw_exception = true) {
    return cast_impl(std::move(src), policy, parent, throw_exception);
  }

 private:
  template <typename CType>
  static handle cast_impl(CType&& src, return_value_policy_t policy,
                          handle parent, bool throw_exception) {
    google::internal::CheckStatusModuleImported();
    if (src.ok()) {
      // Convert and return the payload.
    #if defined(PYBIND11_HAS_RETURN_VALUE_POLICY_PACK)
      auto policy_for_payload = policy.get(0);
    #else
      auto policy_for_payload = policy;
    #endif
      return PayloadCaster::cast(std::forward<CType>(src).value(),
                                 policy_for_payload, parent);
    } else {
      // Convert and return the error.
      return StatusCaster::cast(std::forward<CType>(src).status(),
                                return_value_policy::move, parent,
                                throw_exception);
    }
  }
};

#if defined(PYBIND11_HAS_TYPE_CASTER_STD_FUNCTION_SPECIALIZATIONS)

// IMPORTANT:
//     KEEP
//         type_caster<absl::StatusOr<PayloadType>>
//         func_wrapper<absl::StatusOr<PayloadType>, Args...>
//     IN THE SAME HEADER FILE
//     to avoid surprising behavior differences and ODR violations.

namespace type_caster_std_function_specializations {

template <typename PayloadType, typename... Args>
struct func_wrapper<absl::StatusOr<PayloadType>, Args...> : func_wrapper_base {
  using func_wrapper_base::func_wrapper_base;
  // NOTE: `noexcept` to guarantee that no C++ exception will ever escape.
  absl::StatusOr<PayloadType> operator()(Args... args) const noexcept {
    gil_scoped_acquire acq;
    try {
      object py_result =
#if defined(PYBIND11_HAS_RETURN_VALUE_POLICY_PACK)
          hfunc.f.call_with_policies(rvpp, std::forward<Args>(args)...);
#else
          hfunc.f(std::forward<Args>(args)...);
#endif
      try {
        auto cpp_result =
            py_result.template cast<absl::StatusOr<PayloadType>>();
        // Intentionally not `if constexpr`: runtime overhead is insignificant.
        if (is_same_ignoring_cvref<PayloadType, PyObject*>::value) {
          // Ownership of the Python reference was transferred to cpp_result.
          py_result.release();
        }
        return cpp_result;
      } catch (cast_error& e) {
        return absl::Status(absl::StatusCode::kInvalidArgument, e.what());
      }
    }
    // See comment for the corresponding `catch` in status_caster.h.
    catch (error_already_set& e) {
      e.restore();
      return pybind11_abseil::compat::StatusFromPyExcGivenErrOccurred();
    }
  }
};

}  // namespace type_caster_std_function_specializations

#endif

}  // namespace detail
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_STATUSOR_CASTER_H_
