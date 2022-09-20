#ifndef PYBIND11_ABSEIL_INIT_FROM_TAG_H_
#define PYBIND11_ABSEIL_INIT_FROM_TAG_H_

namespace pybind11 {
namespace google {

enum struct InitFromTag { capsule, capsule_direct_only, serialized };

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_INIT_FROM_TAG_H_
