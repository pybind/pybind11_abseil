# void_ptr_from_capsule ========================================================

add_library(void_ptr_from_capsule STATIC
    void_ptr_from_capsule.cc
)

target_link_libraries(void_ptr_from_capsule PUBLIC
    absl::status
    absl::statusor
    absl::strings
)

# raw_ptr_from_capsule =========================================================

add_library(raw_ptr_from_capsule INTERFACE)

target_link_libraries(raw_ptr_from_capsule INTERFACE
    void_ptr_from_capsule
    absl::statusor
    # python_headers does not need to be linked
)

# make_shared_ptr_capsule ======================================================

add_library(make_shared_ptr_capsule INTERFACE)

# shared_ptr_from_capsule ======================================================

add_library(shared_ptr_from_capsule INTERFACE)

target_link_libraries(shared_ptr_from_capsule INTERFACE
    void_ptr_from_capsule
    absl::statusor
    # python_headers does not need to be linked
)
