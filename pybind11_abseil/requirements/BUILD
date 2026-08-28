load("@python//3.10:defs.bzl", compile_pip_requirements_3_10 = "compile_pip_requirements")
load("@python//3.11:defs.bzl", compile_pip_requirements_3_11 = "compile_pip_requirements")
load("@python//3.12:defs.bzl", compile_pip_requirements_3_12 = "compile_pip_requirements")
load("@python//3.13:defs.bzl", compile_pip_requirements_3_13 = "compile_pip_requirements")
load("@python//3.14:defs.bzl", compile_pip_requirements_3_14 = "compile_pip_requirements")

package(
    default_visibility = ["//visibility:private"],
)

compile_pip_requirements_3_14(
    name = "requirements_3_14",
    src = "requirements.in",
    requirements_txt = "requirements_lock_3_14.txt",
)

compile_pip_requirements_3_13(
    name = "requirements_3_13",
    src = "requirements.in",
    requirements_txt = "requirements_lock_3_13.txt",
)

compile_pip_requirements_3_12(
    name = "requirements_3_12",
    src = "requirements.in",
    requirements_txt = "requirements_lock_3_12.txt",
)

compile_pip_requirements_3_11(
    name = "requirements_3_11",
    src = "requirements.in",
    requirements_txt = "requirements_lock_3_11.txt",
)

compile_pip_requirements_3_10(
    name = "requirements_3_10",
    src = "requirements.in",
    requirements_txt = "requirements_lock_3_10.txt",
)
