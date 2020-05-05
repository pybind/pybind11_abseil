#ifndef THIRD_PARTY_PYBIND11_GOOGLE3_UTILS_ABSL_NUMPY_SPAN_CASTER_H_
#define THIRD_PARTY_PYBIND11_GOOGLE3_UTILS_ABSL_NUMPY_SPAN_CASTER_H_

#include <type_traits>

#include "absl/types/span.h"
#include "third_party/py/numpy/core/include/numpy/ndarrayobject.h"
#include "third_party/py/numpy/core/include/numpy/ndarraytypes.h"
#include "pybind11/cast.h"

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
namespace py_span_internal {
template <typename T>
struct NumpyType;

#ifdef ABSL_NUMPY_MAKE_TYPE_TRAIT
#error "Redefinition of NumPy type trait partial specialization macro"
#endif

#define ABSL_NUMPY_MAKE_TYPE_TRAIT(cxx_type, npy_const) \
  template <>                                           \
  struct NumpyType<cxx_type> {                          \
    static constexpr NPY_TYPES value = npy_const;       \
  }

ABSL_NUMPY_MAKE_TYPE_TRAIT(bool, NPY_BOOL);
ABSL_NUMPY_MAKE_TYPE_TRAIT(signed char, NPY_BYTE);
ABSL_NUMPY_MAKE_TYPE_TRAIT(unsigned char, NPY_UBYTE);
ABSL_NUMPY_MAKE_TYPE_TRAIT(short, NPY_SHORT);            // NOLINT(runtime/int)
ABSL_NUMPY_MAKE_TYPE_TRAIT(unsigned short, NPY_USHORT);  // NOLINT(runtime/int)
ABSL_NUMPY_MAKE_TYPE_TRAIT(int, NPY_INT);
ABSL_NUMPY_MAKE_TYPE_TRAIT(unsigned int, NPY_UINT);
ABSL_NUMPY_MAKE_TYPE_TRAIT(long int, NPY_LONG);          // NOLINT(runtime/int)
ABSL_NUMPY_MAKE_TYPE_TRAIT(unsigned long int, NPY_ULONG  // NOLINT(runtime/int)
);
ABSL_NUMPY_MAKE_TYPE_TRAIT(long long int, NPY_LONGLONG);  // NOLINT(runtime/int)
ABSL_NUMPY_MAKE_TYPE_TRAIT(unsigned long long int,        // NOLINT(runtime/int)
                           NPY_ULONGLONG);
ABSL_NUMPY_MAKE_TYPE_TRAIT(float, NPY_FLOAT);
ABSL_NUMPY_MAKE_TYPE_TRAIT(double, NPY_DOUBLE);
ABSL_NUMPY_MAKE_TYPE_TRAIT(long double, NPY_LONGDOUBLE);

#undef ABSL_NUMPY_MAKE_TYPE_TRAIT
}  // namespace py_span_internal

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
class type_caster<absl::Span<T>, std::enable_if_t<std::is_arithmetic_v<T> &&
                                                  !std::is_const_v<T>>> {
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
    PyObject* source = src.ptr();
    if (!PyArray_Check(source)) {
      // This is not a NumPy array.
      return false;
    }

    auto* npy = reinterpret_cast<PyArrayObject*>(source);
    const int num_dimensions = PyArray_NDIM(npy);
    if (num_dimensions != 1) {
      // We support only 1-dimensional NumPy array dimension.
      return false;
    }
    if (!PyArray_ISCONTIGUOUS(npy)) {
      // Non contiguous NumPy array not supported.
      return false;
    }
    if (!(PyArray_FLAGS(npy) & NPY_ARRAY_WRITEABLE)) {
      // Non writable NumPy array not supported.
      return false;
    }

    const int numpy_type = PyArray_TYPE(npy);
    if (py_span_internal::NumpyType<T>::value != numpy_type) {
      // NumPy element type does not match Span element type.
      return false;
    }
    npy_intp* dimensions = PyArray_DIMS(npy);
    std::size_t size = *dimensions;
    T* numpy_data = reinterpret_cast<T*>(PyArray_DATA(npy));
    value_ = absl::Span<T>(numpy_data, size);
    return true;
  }

 protected:
  absl::Span<T> value_;
};
}  // namespace pybind11::detail

#endif  // THIRD_PARTY_PYBIND11_GOOGLE3_UTILS_ABSL_NUMPY_SPAN_CASTER_H_
