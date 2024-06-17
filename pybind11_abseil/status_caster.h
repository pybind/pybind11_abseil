// Author: Ken Oslund (kenoslund@)

// IWYU pragma: always_keep // See pybind11/docs/type_caster_iwyu.rst

#ifndef PYBIND11_ABSEIL_STATUS_CASTER_H_
#define PYBIND11_ABSEIL_STATUS_CASTER_H_

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

#include "absl/status/status.h"
#include "pybind11_abseil/check_status_module_imported.h"
#include "pybind11_abseil/compat/status_from_py_exc.h"
#include "pybind11_abseil/cpp_capsule_tools/raw_ptr_from_capsule.h"
#include "pybind11_abseil/no_throw_status.h"
#include "pybind11_abseil/ok_status_singleton_lib.h"
#include "pybind11_abseil/status_not_ok_exception.h"

namespace pybind11 {
namespace detail {

template <typename StatusType>
struct NoThrowStatusType {
  // NoThrowStatus should only wrap absl::Status or absl::StatusOr.
  using NoThrowAbslStatus = type_caster_base<absl::Status>;
  static constexpr auto name = NoThrowAbslStatus::name;
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
  static constexpr auto name = const_name("None");
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

  bool load(handle src, bool convert) {
    if (type_caster_base<absl::Status>::load(src, convert)) {
      // Behavior change 2023-02-09: previously `value` was simply left as
      // `nullptr`.
      if (!value) {
        value = const_cast<absl::Status*>(pybind11_abseil::OkStatusSingleton());
      }
      return true;
    }
    if (convert) {
      absl::StatusOr<void*> raw_ptr =
          pybind11_abseil::cpp_capsule_tools::RawPtrFromCapsule<void>(
              src.ptr(), "::absl::Status", "as_absl_Status");
      if (raw_ptr.ok()) {
        value = raw_ptr.value();
        return true;
      }
    }
    return false;
  }

 private:
  template <typename CType>
  static handle cast_impl(CType&& src, return_value_policy policy,
                          handle parent, bool throw_exception) {
    google::internal::CheckStatusModuleImported();
#if defined(PYBIND11_HAS_RETURN_VALUE_POLICY_CLIF_AUTOMATIC)
    if (src.ok() && policy == return_value_policy::_clif_automatic) {
      return pybind11_abseil::PyOkStatusSingleton();
    }
#endif
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

#if defined(PYBIND11_HAS_RETURN_VALUE_POLICY_PACK)

// This code requires https://github.com/google/pybind11k
// IMPORTANT:
//     KEEP
//         type_caster<absl::Status>
//         func_wrapper<absl::Status, Args...>
//     IN THE SAME HEADER FILE
//     to avoid surprising behavior differences and ODR violations.

namespace type_caster_std_function_specializations {

template <typename... Args>
struct func_wrapper<absl::Status, Args...> : func_wrapper_base {
  using func_wrapper_base::func_wrapper_base;
  // NOTE: `noexcept` to guarantee that no C++ exception will ever escape.
  absl::Status operator()(Args... args) const noexcept {
    gil_scoped_acquire acq;
    try {
      object py_result =
          hfunc.f.call_with_policies(rvpp, std::forward<Args>(args)...);
      try {
        return py_result.template cast<absl::Status>();
      } catch (cast_error& e) {
        return absl::Status(absl::StatusCode::kInvalidArgument, e.what());
      }
    }
    // All exceptions derived from std::exception are handled here:
    // https://github.com/pybind/pybind11/blob/aec6cc5406edb076f5a489c2d7f84bb07052c4a3/include/pybind11/detail/internals.h#L363-L420
    // Design choice for safety: Intentionally no `catch (...)`:
    // Occurrence of such exceptions in this context is considered a bug in
    // user code. The `noexcept` above will lead to process termination.
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

#endif  // PYBIND11_ABSEIL_STATUS_CASTER_H_
