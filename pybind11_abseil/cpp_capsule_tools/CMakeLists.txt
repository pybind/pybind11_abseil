# void_ptr_from_capsule ========================================================

add_library(void_ptr_from_capsule STATIC void_ptr_from_capsule.cc)
add_library(pybind11_abseil::cpp_capsule_tools::void_ptr_from_capsule ALIAS
            void_ptr_from_capsule)

target_include_directories(void_ptr_from_capsule
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(void_ptr_from_capsule PUBLIC absl::status absl::statusor
                                                   absl::strings)

# raw_ptr_from_capsule =========================================================

add_library(raw_ptr_from_capsule INTERFACE)
add_library(pybind11_abseil::cpp_capsule_tools::raw_ptr_from_capsule ALIAS
            raw_ptr_from_capsule)

target_include_directories(raw_ptr_from_capsule
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(
  raw_ptr_from_capsule INTERFACE void_ptr_from_capsule absl::statusor
                                 # python_headers does not need to be linked
)

# make_shared_ptr_capsule ======================================================

add_library(make_shared_ptr_capsule INTERFACE)
add_library(pybind11_abseil::cpp_capsule_tools::make_shared_ptr_capsule ALIAS
            make_shared_ptr_capsule)

target_include_directories(make_shared_ptr_capsule
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

# shared_ptr_from_capsule ======================================================

add_library(shared_ptr_from_capsule INTERFACE)
add_library(pybind11_abseil::cpp_capsule_tools::shared_ptr_from_capsule ALIAS
            shared_ptr_from_capsule)


target_include_directories(shared_ptr_from_capsule
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(
  shared_ptr_from_capsule INTERFACE void_ptr_from_capsule absl::statusor
                                    # python_headers does not need to be linked
)