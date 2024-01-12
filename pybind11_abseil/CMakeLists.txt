add_subdirectory(compat)
add_subdirectory(cpp_capsule_tools)

# absl_casters ============================================================
add_library(absl_casters INTERFACE)
add_library(pybind11_abseil::absl_casters ALIAS absl_casters)

target_include_directories(absl_casters
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(
  absl_casters
  INTERFACE absl::cleanup
            absl::btree
            absl::flat_hash_map
            absl::flat_hash_set
            absl::node_hash_map
            absl::node_hash_set
            absl::strings
            absl::time
            absl::optional
            absl::span)

# ok_status_singleton_lib ======================================================

add_library(ok_status_singleton_lib STATIC ok_status_singleton_lib.cc)
add_library(pybind11_abseil::ok_status_singleton_lib ALIAS
            ok_status_singleton_lib)

target_include_directories(ok_status_singleton_lib
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(ok_status_singleton_lib PUBLIC absl::status)

# ok_status_singleton_pyinit_google3 ===========================================

add_library(ok_status_singleton_pyinit_google3 STATIC
            ok_status_singleton_pyinit_google3.cc)

target_link_libraries(ok_status_singleton_pyinit_google3
                      PUBLIC ok_status_singleton_lib)

# ok_status_singleton =======================================================

add_library(ok_status_singleton SHARED ok_status_singleton_py_extension_stub.cc)
add_library(pybind11_abseil::ok_status_singleton ALIAS ok_status_singleton)

target_include_directories(ok_status_singleton
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

set_target_properties(ok_status_singleton PROPERTIES PREFIX "")

target_link_libraries(ok_status_singleton
                      PUBLIC ok_status_singleton_pyinit_google3)

# no_throw_status ==============================================================

add_library(no_throw_status INTERFACE)
add_library(pybind11_abseil::no_throw_status ALIAS no_throw_status)

target_include_directories(no_throw_status
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

# status_not_ok_exception ======================================================

add_library(status_not_ok_exception INTERFACE)
add_library(pybind11_abseil::status_not_ok_exception ALIAS
            status_not_ok_exception)

target_include_directories(status_not_ok_exception
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(status_not_ok_exception INTERFACE absl::status)

# check_status_module_imported =================================================

add_library(check_status_module_imported INTERFACE)

target_link_libraries(check_status_module_imported INTERFACE absl::status)

# status_caster ================================================================

add_library(status_caster INTERFACE)
add_library(pybind11_abseil::status_caster ALIAS status_caster)

target_include_directories(status_caster
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(
  status_caster
  INTERFACE check_status_module_imported
            no_throw_status
            ok_status_singleton_lib
            status_from_py_exc
            status_not_ok_exception
            raw_ptr_from_capsule
            absl::status
            absl::statusor)

# statusor_caster ==============================================================

add_library(statusor_caster INTERFACE)
add_library(pybind11_abseil::statusor_caster ALIAS statusor_caster)

target_include_directories(statusor_caster
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(
  statusor_caster
  INTERFACE check_status_module_imported no_throw_status status_caster
            status_from_py_exc absl::status absl::statusor)

# init_from_tag ================================================================

add_library(init_from_tag INTERFACE)
add_library(pybind11_abseil::init_from_tag ALIAS init_from_tag)

target_include_directories(init_from_tag
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

# utils_pybind11_absl ==========================================================

add_library(utils_pybind11_absl STATIC utils_pybind11_absl.cc)

target_link_libraries(utils_pybind11_absl PUBLIC absl::strings)

# register_status_bindings =====================================================

add_library(register_status_bindings STATIC register_status_bindings.cc)

target_link_libraries(
  register_status_bindings
  PUBLIC absl_casters
         init_from_tag
         no_throw_status
         ok_status_singleton_lib
         status_caster
         status_not_ok_exception
         utils_pybind11_absl
         raw_ptr_from_capsule
         absl::status
         absl::statusor
         absl::strings)

# status_pyinit_google3 ========================================================

add_library(status_pyinit_google3 STATIC status_pyinit_google3.cc)

target_link_libraries(status_pyinit_google3 PUBLIC register_status_bindings)

# status ====================================================================

add_library(status_py_extension_stub SHARED status_py_extension_stub.cc)
set_target_properties(status_py_extension_stub PROPERTIES LIBRARY_OUTPUT_NAME "status")
add_library(pybind11_abseil::status ALIAS status_py_extension_stub)

target_include_directories(status_py_extension_stub INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

set_target_properties(status_py_extension_stub PROPERTIES PREFIX "")

target_link_libraries(status_py_extension_stub PUBLIC status_pyinit_google3 absl::status)

# import_status_module =========================================================

add_library(import_status_module STATIC import_status_module.cc)
add_library(pybind11_abseil::import_status_module ALIAS import_status_module)

target_include_directories(import_status_module
                           INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(import_status_module PUBLIC status_py_extension_stub)

# status_casters ===============================================================

add_library(status_casters INTERFACE)
add_library(pybind11_abseil::status_casters ALIAS status_casters)

target_include_directories(status_casters
  INTERFACE $<BUILD_INTERFACE:${TOP_LEVEL_DIR}>)

target_link_libraries(status_casters INTERFACE import_status_module
  status_caster statusor_caster)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(CMAKE_INSTALL_PYDIR)
  # Copying to two target directories for simplicity. It is currently unknown
  # how to determine here which copy is actually being used.
  install(
    TARGETS status_py_extension_stub ok_status_singleton
    EXPORT pybind11_abseilTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_PYDIR}/pybind11_abseil
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PYDIR}/pybind11_abseil
    RUNTIME DESTINATION ${CMAKE_INSTALL_PYDIR}/pybind11_abseil)

  install(
    TARGETS status_py_extension_stub ok_status_singleton
    EXPORT pybind11_abseil_cppTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

endif()
