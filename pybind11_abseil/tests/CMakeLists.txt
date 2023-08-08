# cpp_capsule_tools_testing ====================================================

pybind11_add_module(cpp_capsule_tools_testing SHARED
    cpp_capsule_tools_testing.cc
)

target_link_libraries(cpp_capsule_tools_testing PUBLIC
    make_shared_ptr_capsule
    raw_ptr_from_capsule
    shared_ptr_from_capsule
    absl::statusor
)
# cpp_capsule_tools_testing_test ===============================================

# absl_example =================================================================

pybind11_add_module(absl_example SHARED
    absl_example.cc
)

target_link_libraries(absl_example PRIVATE
    absl_casters
    absl::btree
    absl::flat_hash_set
    absl::strings
    absl::time
    absl::optional
    absl::span
)

# absl_test ====================================================================

# ok_status_singleton_test =====================================================

# missing_import ===============================================================

pybind11_add_module(missing_import SHARED
    missing_import.cc
)

target_compile_options(missing_import PUBLIC -UNDEBUG)

target_link_libraries(missing_import PRIVATE
    status_casters
    absl::status
    absl::statusor
)

# missing_import_test ==========================================================

# status_test ==================================================================

# status_example ===============================================================

pybind11_add_module(status_example SHARED
    status_example.cc
)

target_link_libraries(status_example PRIVATE
    status_casters
    absl::status
    absl::statusor
)
# status_example_test ==========================================================

