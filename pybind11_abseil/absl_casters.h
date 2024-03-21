// Copyright (c) 2019-2021 The Pybind Development Team. All rights reserved.
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
//
// Type conversion utilities for pybind11 and absl data structures.
//
// Usage: Just include this file in the .cc file with your bindings and add the
// appropriate dependency. Any functions which take or return the supported
// types will have those types automatically converted.
//
// Supported types:
// - absl::Duration- converted to/from python datetime.timedelta
// - absl::CivilTime- converted to/from python datetime.datetime and from date.
// - absl::Time- converted to/from python datetime.datetime and from date.
// - absl::TimeZone- converted to/from python str and from int.
// - absl::Span- converted to python sequences and from python buffers,
//               opaque std::vectors and/or sequences.
// - absl::string_view
// - absl::optional- converts absl::nullopt to/from python None, otherwise
//   converts the contained value.
// - absl::flat_hash_map- converts to/from python dict.
// - absl::flat_hash_set- converst to/from python set.
// - absl::btree_map- converts to/from python dict.
//
// For details, see the README.md.
//
// Author: Ken Oslund

// IWYU pragma: always_keep // See pybind11/docs/type_caster_iwyu.rst

#ifndef PYBIND11_ABSEIL_ABSL_CASTERS_H_
#define PYBIND11_ABSEIL_ABSL_CASTERS_H_

#include <pybind11/cast.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Must NOT appear before at least one pybind11 include.
#include <datetime.h>  // Python datetime builtin.

#include <cmath>
#include <complex>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <vector>

#include "absl/cleanup/cleanup.h"
#include "absl/container/btree_map.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/node_hash_map.h"
#include "absl/container/node_hash_set.h"
#include "absl/strings/cord.h"
#include "absl/strings/string_view.h"
#include "absl/time/civil_time.h"
#include "absl/time/time.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"

namespace pybind11 {
namespace detail {

// Helper function to get an int64_t attribute.
inline int64_t GetInt64Attr(handle src, const char* name) {
  return src.attr(name).cast<int64_t>();
}

template <>
struct type_caster<absl::TimeZone> {
 public:
  PYBIND11_TYPE_CASTER(absl::TimeZone, const_name("absl::TimeZone"));

  // Conversion part 1 (Python->C++)
  bool load(handle src, bool /*convert*/) {
    if (PyUnicode_Check(src.ptr())) {
      if (LoadTimeZone(PyUnicode_AsUTF8(src.ptr()), &value)) {
        return true;
      }
    } else if (PyLong_Check(src.ptr())) {
      value = absl::FixedTimeZone(PyLong_AsLong(src.ptr()));
      return true;
    }
    return false;
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::TimeZone& src, return_value_policy, handle) {
    // Converts to Python str
    return PyUnicode_FromStringAndSize(src.name().data(), src.name().size());
  }
};

namespace internal {

inline void EnsurePyDateTime_IMPORT() {
  if (PyDateTimeAPI == nullptr) {
    PyDateTime_IMPORT;
  }
}

constexpr int64_t GetInt64PythonErrorIndicatorSet = INT64_MAX;

inline int64_t GetTimestampMicrosFromDateTimeObj(PyObject* dt_obj) {
  // Part 1: Integer seconds.
  PyObject* dt_timestamp_py = PyObject_CallMethod(dt_obj, "timestamp", nullptr);
  if (dt_timestamp_py == nullptr) {
    return GetInt64PythonErrorIndicatorSet;
  }
  double dt_timestamp_dbl = PyFloat_AsDouble(dt_timestamp_py);
  Py_DECREF(dt_timestamp_py);
  if (PyErr_Occurred()) {
    return GetInt64PythonErrorIndicatorSet;
  }
  // The fractional part is intentionally discarded here because
  // IEEE 754 binary64 precision (aka double precision) is insufficient for
  // loss-free representation of micro-second resolution timestamps in the
  // [datetime.datetime.min, datetime.datetime.max] range:
  // https://github.com/rwgk/stuff/blob/f688c13c6cf5cefa1b41013d2f636fd10e0ba091/python_datetime/datetime_timestamp_floating_point_behavior_output.txt
  auto dt_timestamp_secs_int64 =
      static_cast<int64_t>(std::floor(dt_timestamp_dbl));

  // Part 2: Integer microseconds.
  auto dt_microsecond = PyDateTime_DATE_GET_MICROSECOND(dt_obj);
  static_assert(sizeof(dt_microsecond) >= 3,
                "Decimal value 999999 needs at least 3 bytes.");

  return dt_timestamp_secs_int64 * 1000000 +
         static_cast<int64_t>(dt_microsecond);
}


// The latest and earliest dates Python's datetime module can represent.
constexpr absl::TimeZone::CivilInfo kDatetimeInfiniteFuture {
    absl::CivilSecond (9999, 12, 31, 23, 59, 59),
    absl::Microseconds(999999)
};
constexpr absl::TimeZone::CivilInfo kDatetimeInfinitePast {
    absl::CivilSecond (1, 1, 1, 0, 0, 0),
    absl::ZeroDuration()
};

// NOTE: Python datetime tzinfo is deliberately ignored.
// Rationale:
// * datetime.datetime.min,max have tzinfo=None.
// * In contrast, the conversions here return datetime.datetime.min,max with
//   tzinfo replaced (UTC).
// * It would be disruptive (and unproductive) to change the behavior of the
//   conversions here.
// * tzinfo for datetime.datetime.min,max is rather meaningless in general,
//   but especially so when those are used as placeholders for infinity.
inline bool is_special_datetime(const absl::TimeZone::CivilInfo& civil_cmp,
  const absl::TimeZone::CivilInfo& civil_special) {
    return civil_cmp.cs == civil_special.cs &&
      civil_cmp.subsecond == civil_special.subsecond;
}

}  // namespace internal

// Convert between absl::Duration and python datetime.timedelta.
template <>
struct type_caster<absl::Duration> {
 public:
  // This macro establishes the name 'absl::Duration' in function signatures
  // and declares a local variable 'value' of type absl::Duration.
  PYBIND11_TYPE_CASTER(absl::Duration, const_name("absl::Duration"));

  // Conversion part 1 (Python->C++)
  bool load(handle src, bool convert) {
    // As early as possible to avoid mid-process surprises.
    internal::EnsurePyDateTime_IMPORT();
    if (!convert) {
      return false;
    }
    if (PyFloat_Check(src.ptr())) {
      value = absl::Seconds(src.cast<double>());
      return true;
    }
    if (PyLong_Check(src.ptr())) {
      value = absl::Seconds(src.cast<int64_t>());
      return true;
    }
    if (PyTime_Check(src.ptr())) {
      value = absl::Hours(PyDateTime_TIME_GET_HOUR(src.ptr())) +
              absl::Minutes(PyDateTime_TIME_GET_MINUTE(src.ptr())) +
              absl::Seconds(PyDateTime_TIME_GET_SECOND(src.ptr())) +
              absl::Microseconds(PyDateTime_TIME_GET_MICROSECOND(src.ptr()));
      return true;
    }
    // Ensure that absl::Duration is converted from a Python
    // datetime.timedelta.
    if (!hasattr(src, "days") || !hasattr(src, "seconds") ||
        !hasattr(src, "microseconds")) {
      return false;
    }
    auto py_duration_t = module::import("datetime").attr("timedelta");
    if (src.is(py_duration_t.attr("max"))) {
      value = absl::InfiniteDuration();
    } else {
      value = absl::Hours(24 * GetInt64Attr(src, "days")) +
              absl::Seconds(GetInt64Attr(src, "seconds")) +
              absl::Microseconds(GetInt64Attr(src, "microseconds"));
    }
    return true;
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::Duration& src, return_value_policy, handle) {
    absl::Duration remainder;
    auto py_duration_t = module::import("datetime").attr("timedelta");
    if (src == absl::InfiniteDuration()) {
      auto py_duration = object(py_duration_t.attr("max"));
      return py_duration.release();
    }
    int64_t secs = absl::IDivDuration(src, absl::Seconds(1), &remainder);
    int64_t microsecs = absl::ToInt64Microseconds(remainder);
    auto py_duration =
        py_duration_t(arg("seconds") = secs, arg("microseconds") = microsecs);
    return py_duration.release();
  }
};

// Convert between absl::Time and python datetime.date, datetime.
template <>
struct type_caster<absl::Time> {
 public:
  // This macro establishes the name 'absl::Time' in function signatures
  // and declares a local variable 'value' of type absl::Time.
  PYBIND11_TYPE_CASTER(absl::Time, const_name("absl::Time"));

  // Conversion part 1 (Python->C++)
  bool load(handle src, bool convert) {
    // As early as possible to avoid mid-process surprises.
    internal::EnsurePyDateTime_IMPORT();
    if (PyDateTime_Check(src.ptr())) {
         absl::TimeZone::CivilInfo civil = {
            absl::CivilSecond(
              PyDateTime_GET_YEAR(src.ptr()),
              PyDateTime_GET_MONTH(src.ptr()),
              PyDateTime_GET_DAY(src.ptr()),
              PyDateTime_DATE_GET_HOUR(src.ptr()),
              PyDateTime_DATE_GET_MINUTE(src.ptr()),
              PyDateTime_DATE_GET_SECOND(src.ptr())),
              absl::Microseconds(PyDateTime_DATE_GET_MICROSECOND(src.ptr()))
        };

      if (internal::is_special_datetime(civil,
        internal::kDatetimeInfiniteFuture)) {
          value = absl::InfiniteFuture();
          return true;
      }
      if (internal::is_special_datetime(civil,
        internal::kDatetimeInfinitePast)) {
          value = absl::InfinitePast();
          return true;
      }
      int64_t dt_timestamp_micros =
          internal::GetTimestampMicrosFromDateTimeObj(src.ptr());
      if (dt_timestamp_micros == internal::GetInt64PythonErrorIndicatorSet) {
        throw error_already_set();
      }
      value = absl::FromUnixMicros(dt_timestamp_micros);
      return true;
    }
    if (convert) {
      if (PyLong_Check(src.ptr())) {
        value = absl::FromUnixSeconds(src.cast<int64_t>());
        return true;
      }
      if (PyFloat_Check(src.ptr())) {
        value = absl::time_internal::FromUnixDuration(absl::Seconds(
            src.cast<double>()));
        return true;
      }
    }
    if (PyDate_Check(src.ptr())) {
      value = absl::FromCivil(absl::CivilSecond(PyDateTime_GET_YEAR(src.ptr()),
                                            PyDateTime_GET_MONTH(src.ptr()),
                                            PyDateTime_GET_DAY(src.ptr()),
                                            0, 0, 0),
                              absl::LocalTimeZone());
      return true;
    }
    return false;
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::Time& src, return_value_policy, handle) {
    // As early as possible to avoid mid-process surprises.
    internal::EnsurePyDateTime_IMPORT();
    // This function truncates fractional microseconds as the python datetime
    // objects cannot support a resolution higher than this.
    if (src == absl::InfiniteFuture()) {
      return PyDateTimeAPI->DateTime_FromDateAndTime(
          9999, 12, 31, 23, 59, 59, 999999, PyDateTimeAPI->TimeZone_UTC,
          PyDateTimeAPI->DateTimeType);
    }
    if (src == absl::InfinitePast()) {
      return PyDateTimeAPI->DateTime_FromDateAndTime(
          1, 1, 1, 0, 0, 0, 0, PyDateTimeAPI->TimeZone_UTC,
          PyDateTimeAPI->DateTimeType);
    }
    absl::TimeZone::CivilInfo info = absl::UTCTimeZone().At(src);
    return PyDateTimeAPI->DateTime_FromDateAndTime(
        info.cs.year(), info.cs.month(), info.cs.day(), info.cs.hour(),
        info.cs.minute(), info.cs.second(),
        static_cast<int>(info.subsecond / absl::Microseconds(1)),
        PyDateTimeAPI->TimeZone_UTC, PyDateTimeAPI->DateTimeType);
  }
};

template <typename CivilTimeUnitType>
struct absl_civil_datetime_caster {
 public:
  PYBIND11_TYPE_CASTER(CivilTimeUnitType, const_name("CivilDateTime"));

  bool load(handle src, bool convert) {
    if (!convert || !hasattr(src, "year") || !hasattr(src, "month") ||
        !hasattr(src, "day")) {
      return false;
    }
    int64_t hour = 0, minute = 0, second = 0;
    if (hasattr(src, "hour") && hasattr(src, "minute") &&
        hasattr(src, "second")) {
      hour = GetInt64Attr(src, "hour");
      minute = GetInt64Attr(src, "minute");
      second = GetInt64Attr(src, "second");
    }
    value =
        CivilTimeUnitType(GetInt64Attr(src, "year"), GetInt64Attr(src, "month"),
                          GetInt64Attr(src, "day"), hour, minute, second);
    return true;
  }

  static handle cast(const CivilTimeUnitType& src, return_value_policy,
                     handle) {
    auto py_datetime_t = module::import("datetime").attr("datetime");
    auto py_datetime = py_datetime_t(src.year(), src.month(), src.day(),
                                     src.hour(), src.minute(), src.second());
    return py_datetime.release();
  }
};

template <typename CivilTimeUnitType>
struct absl_civil_date_caster {
 public:
  PYBIND11_TYPE_CASTER(CivilTimeUnitType, const_name("CivilDate"));

  bool load(handle src, bool convert) {
    if (!convert || !hasattr(src, "year") || !hasattr(src, "month") ||
        !hasattr(src, "day")) {
      return false;
    }
    value =
        CivilTimeUnitType(GetInt64Attr(src, "year"), GetInt64Attr(src, "month"),
                          GetInt64Attr(src, "day"));
    return true;
  }

  static handle cast(const CivilTimeUnitType& src, return_value_policy,
                     handle) {
    auto py_datetime_t = module::import("datetime").attr("date");
    auto py_datetime = py_datetime_t(src.year(), src.month(), src.day());
    return py_datetime.release();
  }
};

template <>
struct type_caster<absl::CivilSecond>
    : public absl_civil_datetime_caster<absl::CivilSecond> {};

template <>
struct type_caster<absl::CivilMinute>
    : public absl_civil_datetime_caster<absl::CivilMinute> {};

template <>
struct type_caster<absl::CivilHour>
    : public absl_civil_datetime_caster<absl::CivilHour> {};

template <>
struct type_caster<absl::CivilDay>
    : public absl_civil_date_caster<absl::CivilDay> {};

template <>
struct type_caster<absl::CivilMonth>
    : public absl_civil_date_caster<absl::CivilMonth> {};

template <>
struct type_caster<absl::CivilYear>
    : public absl_civil_date_caster<absl::CivilYear> {};

namespace internal {

template <typename T>
static constexpr bool is_buffer_interface_compatible_type =
    detail::is_same_ignoring_cvref<T, PyObject*>::value ||
    std::is_arithmetic<std::remove_cv_t<T>>::value ||
    std::is_same<T, std::complex<float>>::value ||
    std::is_same<T, std::complex<double>>::value;

}  // namespace internal

// Returns {true, a span referencing the data contained by src} without copying
// or converting the data if possible. Otherwise returns {false, an empty span}.
template <typename T, typename std::enable_if<
                          internal::is_buffer_interface_compatible_type<T>,
                          bool>::type = true>
std::tuple<bool, absl::Span<T>> LoadSpanFromBuffer(handle src) {
  Py_buffer view;
  int flags = PyBUF_STRIDES | PyBUF_FORMAT;
  if (!std::is_const<T>::value) flags |= PyBUF_WRITABLE;
  if (PyObject_GetBuffer(src.ptr(), &view, flags) == 0) {
    auto cleanup = absl::MakeCleanup([&view] { PyBuffer_Release(&view); });
    if (view.ndim == 1 && view.strides[0] == sizeof(T) &&
        buffer_info(&view, /*ownview=*/false)
            .item_type_is_equivalent_to<std::remove_cv_t<T>>()) {
      return {true, absl::MakeSpan(static_cast<T*>(view.buf), view.shape[0])};
    }
  } else {
    // Clear the buffer error (failure is reported in the return value).
    PyErr_Clear();
  }
  return {false, absl::Span<T>()};
}
template <typename T, typename std::enable_if<
                          !internal::is_buffer_interface_compatible_type<T>,
                          bool>::type = true>
constexpr std::tuple<bool, absl::Span<T>> LoadSpanFromBuffer(handle /*src*/) {
  return {false, absl::Span<T>()};
}

template <typename T,
          typename std::enable_if<
              !std::is_same<std::remove_cv_t<T>, bool>::value, int>::type = 0>
std::tuple<bool, absl::Span<T>> LoadSpanOpaqueVector(handle src) {
  // Attempt to unwrap an opaque std::vector.
  using value_type = std::remove_cv_t<T>;
  type_caster_base<std::vector<value_type>> caster;
  if (caster.load(src, false)) {
    return {true,
            absl::MakeSpan(static_cast<std::vector<value_type>&>(caster))};
  }
  return {false, absl::Span<T>()};
}

template <typename T,
          typename std::enable_if<
              std::is_same<std::remove_cv_t<T>, bool>::value, int>::type = 0>
std::tuple<bool, absl::Span<T>> LoadSpanOpaqueVector(handle /*src*/) {
  // std::vector<bool> is special and cannot directly be converted to a Span
  // (see https://en.cppreference.com/w/cpp/container/vector_bool).
  return {false, absl::Span<T>()};
}

// Helper to determine whether T is a span.
template <typename T>
struct is_absl_span : std::false_type {};
template <typename T>
struct is_absl_span<absl::Span<T>> : std::true_type {};

// Convert between absl::Span and sequence types.
// See http://g3doc/pybind11_abseil/README.md#abslspan
template <typename T>
struct type_caster<absl::Span<T>> {
 public:
  // The type referenced by the span, with const removed.
  using value_type = std::remove_cv_t<T>;
  static_assert(!is_absl_span<value_type>::value,
                "Nested absl spans are not supported.");

  type_caster() = default;
  // Copy and Move operations must ensure the span points to the copied or
  // moved vector (if any), not the original one. Allows implicit conversions.
  template <typename U>
  type_caster(const type_caster<absl::Span<U>>& other) {
    *this = other;
  }
  template <typename U>
  type_caster(type_caster<absl::Span<U>>&& other) {
    *this = std::move(other);
  }
  template <typename U>
  type_caster& operator=(const type_caster<absl::Span<U>>& other) {
    list_caster_ = other.list_caster_;
    value_ = list_caster_ ? get_value(*list_caster_) : other.value_;
    return *this;
  }
  template <typename U>
  type_caster& operator=(type_caster<absl::Span<U>>&& other) {
    list_caster_ = std::move(other.list_caster_);
    value_ = list_caster_ ? get_value(*list_caster_) : other.value_;
    return *this;
  }

  static constexpr auto name =
      const_name("Sequence[") + make_caster<T>::name + const_name("]");

  // We do not allow moving because 1) spans are super lightweight, so there's
  // no advantage to moving and 2) the span cannot exist without the caster,
  // so moving leaves an implicit dependency (while a reference or pointer
  // make that dependency explicit).
  operator absl::Span<T>*() { return &value_; }
  operator absl::Span<T>&() { return value_; }
  template <typename T_>
  using cast_op_type = cast_op_type<T_>;

  bool load(handle src, bool convert) {
    // Attempt to reference a buffer, including np.ndarray and array.arrays.
    bool loaded;
    std::tie(loaded, value_) = LoadSpanFromBuffer<T>(src);
    if (loaded) return true;

    std::tie(loaded, value_) = LoadSpanOpaqueVector<T>(src);
    if (loaded) return true;

    // Attempt to convert a native sequence. If the is_base_of check passes,
    // the elements do not require converting and pointers do not reference a
    // temporary object owned by the element caster. Pointers to converted
    // types are not allowed because they would result a dangling reference
    // when the element caster is destroyed.
    if (convert && std::is_const<T>::value &&
        // See comment for ephemeral_storage_type below.
        !std::is_same<T, const bool>::value &&
        (!std::is_pointer<T>::value ||
         std::is_base_of<type_caster_generic, make_caster<T>>::value)) {
      list_caster_.emplace();
      if (list_caster_->load(src, convert)) {
        value_ = get_value(*list_caster_);
        return true;
      } else {
        list_caster_.reset();
      }
    }

    return false;  // Python type cannot be loaded into a span.
  }

  template <typename CType>
  static handle cast(CType&& src, return_value_policy policy, handle parent) {
    return ListCaster::cast(src, policy, parent);
  }

 private:
  // Unfortunately using std::vector as ephemeral_storage_type creates
  // complications for std::vector<bool>
  // (https://en.cppreference.com/w/cpp/container/vector_bool).
  using ephemeral_storage_type = std::vector<value_type>;

  template <
      typename Caster, typename VT = value_type,
      typename std::enable_if<!std::is_same<VT, bool>::value, int>::type = 0>
  absl::Span<T> get_value(Caster& caster) {
    return absl::MakeSpan(static_cast<ephemeral_storage_type&>(caster));
  }

  // This template specialization is needed to avoid compilation errors.
  // The conditions in load() make this code unreachable.
  template <
      typename Caster, typename VT = value_type,
      typename std::enable_if<std::is_same<VT, bool>::value, int>::type = 0>
  absl::Span<T> get_value(Caster&) {
    throw std::runtime_error("Expected to be unreachable.");
  }

  using ListCaster = list_caster<ephemeral_storage_type, value_type>;
  absl::optional<ListCaster> list_caster_;
  absl::Span<T> value_;
};

// Convert between absl::flat_hash_map and python dict.
template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
struct type_caster<absl::flat_hash_map<Key, Value, Hash, Equal, Alloc>>
    : map_caster<absl::flat_hash_map<Key, Value, Hash, Equal, Alloc>, Key,
                 Value> {};

// Convert between absl::flat_hash_map and python dict.
template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
struct type_caster<absl::node_hash_map<Key, Value, Hash, Equal, Alloc>>
    : map_caster<absl::node_hash_map<Key, Value, Hash, Equal, Alloc>, Key,
                 Value> {};

// Convert between absl::flat_hash_set and python set.
template <typename Key, typename Hash, typename Equal, typename Alloc>
struct type_caster<absl::flat_hash_set<Key, Hash, Equal, Alloc>>
    : set_caster<absl::flat_hash_set<Key, Hash, Equal, Alloc>, Key> {};

// Convert between absl::btree_map and python dict.
template <typename Key, typename Value, typename Compare, typename Alloc>
struct type_caster<absl::btree_map<Key, Value, Compare, Alloc>>
    : map_caster<absl::btree_map<Key, Value, Compare, Alloc>, Key, Value> {};

// Convert between absl::node_hash_set and python set.
template <typename Key, typename Hash, typename Equal, typename Alloc>
struct type_caster<absl::node_hash_set<Key, Hash, Equal, Alloc>>
    : set_caster<absl::node_hash_set<Key, Hash, Equal, Alloc>, Key> {};

// Convert between absl::string_view and python.
//
// pybind11 supports std::string_view, and absl::string_view is meant to be a
// drop-in replacement for std::string_view, so we can just use the built in
// implementation. This is only needed until absl::string_view becomes an alias
// for std::string_view.
#ifndef ABSL_USES_STD_STRING_VIEW
template <>
struct type_caster<absl::string_view> : string_caster<absl::string_view, true> {
};
#endif

template <>
struct type_caster<absl::Cord> {
 public:
  using StringViewCaster = make_caster<absl::string_view>;
  PYBIND11_TYPE_CASTER(absl::Cord, const_name("absl::Cord"));

  // Conversion part 1 (Python->C++)
  bool load(handle src, bool convert) {
    auto caster = StringViewCaster();
    if (caster.load(src, convert)) {
      absl::string_view view = cast_op<absl::string_view>(std::move(caster));
      value = view;
      return true;
    }
    return false;
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::Cord& src, return_value_policy /*policy*/,
                     handle /*parent*/) {
    return bytes(std::string(src)).release();
  }
};

// Convert between absl::optional and python.
//
// pybind11 supports std::optional, and absl::optional is meant to be a
// drop-in replacement for std::optional, so we can just use the built in
// implementation.
#ifndef ABSL_USES_STD_OPTIONAL
template <typename T>
struct type_caster<absl::optional<T>>
    : public optional_caster<absl::optional<T>> {};
template <>
struct type_caster<absl::nullopt_t> : public void_caster<absl::nullopt_t> {};
#endif

// This is a simple port of the pybind11 std::variant type_caster, applied to
// absl::variant. See pybind11 stl.h.
#ifndef ABSL_HAVE_STD_VARIANT
template <typename... Ts>
struct type_caster<absl::variant<Ts...>>
    : variant_caster<absl::variant<Ts...>> {};

#endif

}  // namespace detail

}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_ABSL_CASTERS_H_
