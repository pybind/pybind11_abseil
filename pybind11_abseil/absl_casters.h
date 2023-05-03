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

#ifndef PYBIND11_ABSEIL_ABSL_CASTERS_H_
#define PYBIND11_ABSEIL_ABSL_CASTERS_H_

#include <pybind11/cast.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Must NOT appear before at least one pybind11 include.
#include <datetime.h>  // Python datetime builtin.

#include <cmath>
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

// Given a python date or datetime object, figure out an appropriate
// absl::TimeZone with a fixed offset, or default to the local timezone.
inline absl::TimeZone GetTimeZone(handle src) {
  if (!hasattr(src, "tzinfo")) {
    // datetime.date objects lack this property, so assume local.
    return absl::LocalTimeZone();
  }
  object tzinfo = src.attr("tzinfo");
  if (tzinfo.is_none()) {
    // non-tz aware datetime, again assume local time.
    return absl::LocalTimeZone();
  }
  object utc_offset = tzinfo.attr("utcoffset")(src);
  if (utc_offset.is_none()) {
    return absl::LocalTimeZone();
  }
  int64_t offset_seconds =
      std::lround(utc_offset.attr("total_seconds")().cast<double>());
  return absl::FixedTimeZone(offset_seconds);
}

template <>
struct type_caster<absl::TimeZone> {
 public:
  PYBIND11_TYPE_CASTER(absl::TimeZone, const_name("absl::TimeZone"));

  // Conversion part 1 (Python->C++)
  bool load(handle src, bool convert) {
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
    if (src == object(py_duration_t.attr("max"))) {
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
    if (!hasattr(src, "year") || !hasattr(src, "month") ||
        !hasattr(src, "day")) {
      return false;
    }
    if (hasattr(src, "timestamp")) {
      // python datetime.datetime object
      double timestamp = src.attr("timestamp")().cast<double>();
      int64_t as_micros = static_cast<int64_t>(timestamp * 1e6);
      value = absl::FromUnixMicros(as_micros);
    }
#if PY_MAJOR_VERSION < 3
    else if (hasattr(src, "microsecond")) {  // NOLINT(readability/braces)
      // python datetime.datetime object
      // This doesn't rely on datetime.datetime.timestamp(), which is not
      // available in Python 2.
      auto utc = module::import("dateutil.tz").attr("UTC");
      auto datetime = module_::import("datetime").attr("datetime");
      auto gettz = module::import("dateutil.tz").attr("gettz");

      auto epoch_dt = datetime.attr("fromtimestamp")(0, utc);
      auto tz = (src.attr("tzinfo").is_none()) ? gettz() : src.attr("tzinfo");
      auto src_with_tz = src.attr("replace")("tzinfo"_a = tz);

      double timestamp =
          (src_with_tz - epoch_dt).attr("total_seconds")().cast<double>();
      int64_t as_micros = static_cast<int64_t>(timestamp * 1e6);
      value = absl::FromUnixMicros(as_micros);
    }
#endif
    else {  // NOLINT(readability/braces)
      // python datetime.date object
      absl::CivilDay civil_day(GetInt64Attr(src, "year"),
                               GetInt64Attr(src, "month"),
                               GetInt64Attr(src, "day"));
      value = absl::FromCivil(civil_day, GetTimeZone(src));
    }
    return true;
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::Time& src, return_value_policy, handle) {
    // This function truncates fractional microseconds as the python datetime
    // objects cannot support a resolution higher than this.
    auto py_datetime_t = module::import("datetime").attr("datetime");
    auto py_from_timestamp = py_datetime_t.attr("fromtimestamp");
    auto py_timezone_t = module::import("dateutil.tz").attr("gettz");
    auto py_timezone = py_timezone_t(absl::LocalTimeZone().name());
    double as_seconds = static_cast<double>(absl::ToUnixMicros(src)) / 1e6;
    auto py_datetime = py_from_timestamp(as_seconds, "tz"_a = py_timezone);
    return py_datetime.release();
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

// Returns {true, a span referencing the data contained by src} without copying
// or converting the data if possible. Otherwise returns {false, an empty span}.
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value,
                                              bool>::type = true>
std::tuple<bool, absl::Span<T>> LoadSpanFromBuffer(handle src) {
  Py_buffer view;
  int flags = PyBUF_STRIDES | PyBUF_FORMAT;
  if (!std::is_const<T>::value) flags |= PyBUF_WRITABLE;
  if (PyObject_GetBuffer(src.ptr(), &view, flags) == 0) {
    auto cleanup = absl::MakeCleanup([&view] { PyBuffer_Release(&view); });
    if (view.ndim == 1 && view.strides[0] == sizeof(T) &&
        view.format[0] == format_descriptor<T>::c) {
      return {true, absl::MakeSpan(static_cast<T*>(view.buf), view.shape[0])};
    }
  } else {
    // Clear the buffer error (failure is reported in the return value).
    PyErr_Clear();
  }
  return {false, absl::Span<T>()};
}
// If T is not a numeric type, the buffer interface cannot be used.
template <typename T, typename std::enable_if<!std::is_arithmetic<T>::value,
                                              bool>::type = true>
constexpr std::tuple<bool, absl::Span<T>> LoadSpanFromBuffer(handle src) {
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
  using value_type = typename std::remove_cv<T>::type;
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

  static constexpr auto name = _("Span[") + make_caster<T>::name + _("]");

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

    // Attempt to unwrap an opaque std::vector.
    type_caster_base<std::vector<value_type>> caster;
    if (caster.load(src, false)) {
      value_ = get_value(caster);
      return true;
    }

    // Attempt to convert a native sequence. If the is_base_of_v check passes,
    // the elements do not require converting and pointers do not reference a
    // temporary object owned by the element caster. Pointers to converted
    // types are not allowed because they would result a dangling reference
    // when the element caster is destroyed.
    if (convert && std::is_const<T>::value &&
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
  template <typename Caster>
  absl::Span<T> get_value(Caster& caster) {
    return absl::MakeSpan(static_cast<std::vector<value_type>&>(caster));
  }

  using ListCaster = list_caster<std::vector<value_type>, value_type>;
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
  static handle cast(const absl::Cord& src, return_value_policy policy,
                     handle parent) {
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
