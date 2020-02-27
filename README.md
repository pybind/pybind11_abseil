# Pybind11 bindings for the Abseil C++ Common Libraries

These adapters make Abseil types work with Pybind11 bindings. For more
information on using Pybind11, see
g3doc/third_party/pybind11/google3_utils/README.md.

To use the converters listed below, just include the header
in the .cc file with your bindings:

```
#include "pybind11_abseil/absl_casters.h"
```

## absl::Duration

`absl::Duration` objects are converted to/ from python datetime.timedelta objects.
Therefore, C code cannot mutate any datetime.timedelta objects from python.

## absl::Time

`absl::Time` objects are converted to/from python datetime.datetime objects.
Additionally, datetime.date objects can be converted to `absl::Time` objects.
C code cannot mutate any datetime.datetime objects from python.

Python date objects effectively truncate the time to 0 (ie, midnight).
Python time objects are not supported because `absl::Time` would implicitly
assume a year, which could be confusing.

Python datetime objects include timezone information, while
`absl::Time` does not. Therefore, it would be possible to adjust for the
timezone when going from python->C, but it would not be possible to reverse
that adjustment when going the other way, since the desired timezone is
unknown. This could easily result in unexpected behavior- eg, passing in a
Pacific time zone time and getting back a UTC time. To make this conversion
symetric, this class ignores the python timezone information.

## absl::Span

Python sequences can be converted to/from an absl::Span. Currently, this
will always result in the list being copied, so you lose the efficiency gains
of spans in native C++, but you still get the API versatility.

The value type in the span can be any type that pybind knows about. However, it
must be immutable (ie, `absl::Span<const ValueType>`). Theoretically mutable
ValueTypes could be supported, but with some subtle limitations, and this is
not needed right now, so the implementation has been deferred.

The `convert` and `return_value_policy` parameters will apply to the *elements*.
The list containing those elements will aways be converted/copied.

## absl::string_view

Supported exactly the same way pybind11 supports `std::string_view`.

## absl::optional

Supported exactly the same way pybind11 supports `std::optional`.
