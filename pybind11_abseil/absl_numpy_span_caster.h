#ifndef PYBIND11_ABSEIL_ABSL_NUMPY_SPAN_CASTER_H_
#define PYBIND11_ABSEIL_ABSL_NUMPY_SPAN_CASTER_H_

#include <type_traits>

#include "absl/types/span.h"
#include "pybind11/cast.h"
#include "pybind11/numpy.h"

// Support for conversion from NumPy array to template specializations of
// absl::Span. Note that no data is copied, the `Span`ned memory is owned by the
// NumPy array.
//
// Only one direction of conversion is provided: a Span argument can be exposed
// to Python as a NumPy array argument. No conversion is provided for a Span
// return value.
//
// Lifetime management will become complicated if a `Span` returned by a C++
// object is converted into a NumPy array which outlives the C++ object. Such
// memory management concerns are normal for C++ but undesirable to introduce to
// Python.
//
// A `Span` argument can be used to expose an output argument to Python so that
// Python is wholly responsible for memory management of the output object.
// Using an output argument rather than a return value means that memory can be
// reused.
//
// C++:
//  Simulation::Simulation(int buffer_size);
//  void Simulation::RenderFrame(int frame_index, Span<uint8> buffer);
//
// Python:
//  buffer = np.zeroes(1024*768, dtype='uint8')
//  simulation = Simulation(1024*768)
//  simulation.renderFrame(0, buffer)
//  # RGB data can now be read from the buffer.

namespace pybind11::detail {

// Conversion of non-const Span.
// Note that there are two template specialisations for `absl::Span`: the one in
// this file and the one for `const T` in `absl_caster.h`.
// The use of `std::enable_if_t`, and in particular of the `std::is_const_v`
// condition check, is really important and prevents unpleasant ODR (one
// definition rule) violations when linking together different object files with
// different includes.
// TODO(b/155596364) merge this implementation with the absl::Span<const T>
// caster.
template <typename T>
class type_caster<absl::Span<T>,
                  // C++11 compatibility.
                  typename std::enable_if<std::is_arithmetic<T>::value &&
                                          !std::is_const<T>::value>::type> {
 public:
  // Instead of using the macro PYBIND11_TYPE_CASTER we explicitly define value
  // and functions. This is because, the macro has default implementations for
  // move constructors and cast method.
  // Note: we also disable the linter on these methods and variables.
  static constexpr auto name =  // NOLINT
      _("Span[") + make_caster<T>::name + _("]");

  // We do not allow moving because 1) spans are super lightweight, so there is
  // no advantage to moving and 2) the span cannot exist without the caster,
  // so moving leaves an implicit dependency (while a reference or pointer
  // make that dependency explicit).
  operator absl::Span<T>*() {  // NOLINT(google-explicit-constructor)
    return &value_;
  }
  operator absl::Span<T>&() {  // NOLINT(google-explicit-constructor)
    return value_;
  }
  template <typename T_>
  using cast_op_type = cast_op_type<T_>;

  // Conversion Python->C++: convert a NumPy array into a Span.
  bool load(handle src, bool /* convert */) {
    // Extract PyObject from handle.
    if (!pybind11::isinstance<pybind11::array_t<T>>(src)) {
      return false;
    }
    auto array = src.cast<pybind11::array_t<T>>();
    if (!array || array.ndim() != 1 || array.strides()[0] != sizeof(T) ||
        !array.writeable()) {
      return false;
    }
    value_ = absl::Span<T>(static_cast<T*>(array.mutable_data()), array.size());
    return true;
  }

 protected:
  absl::Span<T> value_;
};
}  // namespace pybind11::detail

#endif  // PYBIND11_ABSEIL_ABSL_NUMPY_SPAN_CASTER_H_
