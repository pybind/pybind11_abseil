# Pybind11 bindings for the Abseil C++ Common Libraries

![build_and_test](https://github.com/pybind/pybind11_abseil/workflows/build_and_test/badge.svg)

[TOC]

## Overview

These adapters make Abseil types work with Pybind11 bindings. For more
information on using Pybind11, see
g3doc/third_party/pybind11/google3_utils/README.md.

To use the converters listed below, just include the header
in the .cc file with your bindings:

```
#include "pybind11_abseil/absl_casters.h"
```

Support for non-const `absl::Span` for numeric types is also available by
including a separated header file:

```
#include "pybind11_abseil/absl_numpy_span_caster.h"
```

## Installation

You will need to depend on `pybind11`, `pybind11_bazel`(see
[doc](https://github.com/pybind/pybind11_bazel#installation), and on
`pybind11_abseil`, e.g.

```
git_repository(
    name = "pybind11_bazel",
    remote = "https://github.com/pybind/pybind11_bazel.git",
    branch = "master",
)

http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-2.6.2",
  sha256 = "8ff2fff22df038f5cd02cea8af56622bc67f5b64534f1b83b9f133b8366acff2",
  urls = ["https://github.com/pybind/pybind11/archive/v2.6.2.tar.gz"],
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")

git_repository(
    name = "pybind11_abseil",
    remote = "https://github.com/pybind/pybind11_abseil.git",
    branch = "master",
)
```

Then, in your BUILD file:

```
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
```

## absl::Duration

`absl::Duration` objects are converted to/ from python datetime.timedelta objects.
Therefore, C code cannot mutate any datetime.timedelta objects from python.

## absl::Time

`absl::Time` objects are converted to/from python datetime.datetime objects.
Additionally, datetime.date objects can be converted to `absl::Time` objects.
C code cannot mutate any datetime.datetime objects from python.

Python date objects effectively truncate the time to 0 (i.e., midnight).
Python time objects are not supported because `absl::Time` would implicitly
assume a year, which could be confusing.

### Time zones
Python `datetime` objects include timezone information, while
`absl::Time` does not. When converting from Python to C++, if a timezone is
specified then it will be used to determine the `absl::Time` instant. If no
timezone is specified by the Python `datetime` object, the local timezone is
assumed.

When converting back from C++ to Python, the resultant time will be presented in
the local timezone and the `tzinfo` property set on the `datetime` object to
reflect that. This means that the caller may receive a datetime formatted
in a different timezone to the one they passed in. To handle this safely, the
caller should take care to check the `tzinfo` of any returned `datetime`s.

## absl::CivilTime
`absl::CivilTime` objects are converted to/from Python datetime.datetime
objects. Fractional Python datetime components are truncated when converting to
less granular C++ types, and time zone information is ignored.

## absl::Span

For non-const `absl::Span` and conversion from `numpy` arrays, see
[non-const absl::Span](#non-const-abslspan) later.

When `absl::Span<const T>` (i.e. the `const` version) is considered, there is
full support to mapping into Python sequences.
Currently, this will always result in the list being copied, so you lose the
efficiency gains of spans in native C++, but you still get the API versatility.

The value type in the span can be any type that pybind knows about. However, it
must be immutable (i.e., `absl::Span<const ValueType>`). Theoretically mutable
ValueTypes could be supported, but with some subtle limitations, and this is
not needed right now, so the implementation has been deferred.

The `convert` and `return_value_policy` parameters will apply to the *elements*.
The list containing those elements will aways be converted/copied.

### non-const absl::Span
Support for non-cost `absl::Span`, for numeric types only, is provided for
`numpy` arrays. Support is only for output function parameters and not for
returned value. The rationale behind this decision is that, if a `absl::Span`
were to be returned, the C++ object would have needed to outlive the mapped
Python object. Given the complexity of memory management across languages, we
did not add support of returned `absl::Span`.
That is the following is supported:

```
void Foo(absl::Span<double> some_span);
```
while the following is not (it will generate a compile error):
```
absl::Span<double> Bar();
```

Note: It is possible to use the non-const `absl::Span` bindings to wrap a
function with `absl::Span<const T>` argument if you are using `numpy` arrays
and you do not want a copy to be performed. This can be done by defining a
lambda function in the `pybind11` wrapper, as in the following example. See
b/155596364 for more details.

```
void MyConstSpanFunction(absl::Span<const double> a_span);
...

PYBIND11_MODULE(bindings, m) {
  m.def(
      "wrap_span",
      [](absl::Span<double> span) {
        MyConstSpanFunction(span);
      });
}
```


## absl::string_view

Supported exactly the same way pybind11 supports `std::string_view`.

## absl::optional

Supported exactly the same way pybind11 supports `std::optional`.

## absl::flat_hash_map

Supported exactly the same way pybind11 supports `std::map`.

## absl::flat_hash_set

Supported exactly the same way pybind11 supports `std::set`.

## absl::Status[Or]

To use the Status[Or] casters:

1. Include the header file `pybind11_abseil/status_casters.h`
   in the .cc file with your bindings.
1. Call `pybind11::google::ImportStatusModule();` in your `PYBIND11_MODULE`
   definition.

By default, an ok status will be converted into `None`, and a non-ok status will
raise a `status.StatusNotOk` exception. This has a `status` attribute which can
be used to access the status object and check the code/ message.

To get a `status.Status` object rather than having an exception thrown, pass
either the `Status` object or a function returning a `Status` to
`pybind11::google::DoNotThrowStatus` before casting or binding. This works with
references and pointers to `absl::Status` objects too.

It isn't possible to specify separate return value policies for a `StatusOr`
object and its payload. Since `StatusOr` is processed and not ever actually
represented in Python, the return value policy applies to the payload. E.g., if
you return a StatusOr<MyObject*> (note the * is inside the `StatusOr`) with a
take_ownership return val policy and the status is OK (i.e., it has a payload),
Python will take ownership of that payload and free it when it is garbage
collected.

However, if you return a `StatusOr<MyObject>*` (note: the `*` is outside the
`StatusOr` rather than inside it now) with a `take_ownership` return val policy,
Python does not take ownership of the `StatusOr` and will not free it (because
again, that policy applies to `MyObject`, not `StatusOr`).

See `status_utils.cc` in this directory for details about what methods are
available in wrapped `absl::Status` objects.

Example:

```cpp
#include "pybind11_abseil/status_casters.h"

absl::Status StatusReturningFunction() {
  return absl::Status(...);
}

pybind11::object StatusHandlingFunction() {
  return pybind11::cast(pybind11::google::DoNotThrowStatus(StatusReturningFunction()));
}

PYBIND11_MODULE(test_bindings, m) {
  pybind11::google::ImportStatusModule();

  m.def("return_status", &StatusReturningFunction,
        "Return None if StatusCode is OK, otherwise raise an error.");
  m.def("make_status", google::DoNotThrowStatus(&StatusReturningFunction),
        "Return a wrapped status object without raising an error.");
  m.def("status_handling_function", &StatusHandlingFunction,
        "Same effect as make_status, but cast is done internally.");
};
```

Python:

```python
from pybind11_abseil import status
import test_bindings

my_status = make_status()
if my_status.code():
  ...

try:
  return_status()
except status.StatusNotOk as e:
  print(e.status)
```

### absl::StatusOr

`absl::StatusOr` objects behave exactly like `absl::Status` objects, except:

- There is no support for passing StatusOr objects. You can only return them.
- Instead of returning None or a wrapped status with OK, this casts and
  returns the payload when there is no error.

As with `absl::Status`, the default behavior is to throw an error when casting
a non-ok status. You may pass a StatusOr object or StatusOr returning function
to `pybind11::google::DoNotThrowStatus` in exactly the same way as with
`absl::Status` to change this behavior.

`absl::StatusOr` objects must be returned by value (not reference or pointer).
Why? Because the implementation takes advantage of the fact that python is a
dynamically typed language to cast and return the payload *or* the
`absl::Status` object (or raise an exeception). Python has no concept of a
`absl::StatusOr` object, so it's also impossible to apply the
return_value_policy to a `absl::StatusOr`. Therefore returning a reference or
pointer to a `absl::StatusOr` is meaningless.

Pointers *can* be used as the payload type, and the return_value_policy will
be applied to the payload if the status is OK. However, references cannot be
used as the payload type, because that's a restriction on `absl::StatusOr` in
general, not pybind11 (see https://yaqs/5903163345338368).

This can handle any type of payload that pybind knows about. unique_ptrs (i.e.,
`absl::StatusOr<std::unique_ptr<...>>`) to wrapped classes or structs (i.e., any
type which you created bindings for using `pybind11::class_<...>`) can be used,
but unique_ptrs to converted types (e.g., `int`, `string`, `absl::Time`,
`absl::Duration`, etc.) cannot be used.

### absl::StatusCode

The `status` module provides `pybind11::enum_` bindings for `absl::StatusCode`.
These use python constant style, e.g. `status.StatusCode.OK`,
`status.StatusCode.CANCELLED`, etc.

Warning: Pybind enums are their own type, and will never compare equally to
integers due to being a different type, regardless of their value. In particular,
note that the [status proto](http://google3/util/task/status.proto)
`code` field is an integer, so it will never directly compare as equal to a
`StatusCode`. To fix this, convert an integer to a `StatusCode` or vice-versa.

```python
status_code = 0  # An integer.

if status_code == status.StatusCode.OK:  # Wrong: always evaluates to false.
  ...

if status.StatusCode(status_code) == status.StatusCode.OK:  # Correct.
  ...

if status_code == int(status.StatusCode.OK):  # Also correct.
  ...
```

### Aliasing parts of the status module

The need to import the `status` module can be eliminated by aliasing the parts
of the status module that are needed in your own module:

```cpp
PYBIND11_MODULE(test_bindings, m) {
  auto status_module = pybind11::google::ImportStatusModule();
  m.attr("StatusNotOk") = status_module.attr("StatusNotOk");

  ...
}
```

Python:

```python
import test_bindings

try:
  return_status()
except test_bindings.StatusNotOk as e:
  print(e.status)

```

### Importing the status module

The status module uses the same import mechansim as the proto module; see [its
documentation](../pybind11_protobuf/README.md#importing-the-proto-module)
for details. For now there is a `#ifdef` to allow `ImportStatusModule` to work
with python 2 rather than giving an error, but this will be removed eventually.

If modifying the following functions, make the same changes in the
corresponding proto functions:
- ImportStatusModule
- IsStatusModuleImported
- CheckStatusModuleImported

### Use Outside of Google3

The path used for the status module may be changed by altering the value of
`PYBIND11_ABSEIL_STATUS_MODULE_PATH` defined in `status_casters.h`. This uses
the same mechanism as the proto module, so see [its documentation
](../pybind11_protobuf/README.md?cl=head#use-outside-of-google3)
for details.
