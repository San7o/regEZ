cc_library(
    name = "regez",
    srcs = ["regez.cpp"],
    hdrs = ["//include/regez:regez.hpp"],  # Refer to the header in the `include/regez` directory
    includes = ["//include"],  # Add the base include directory to the include path
    copts = ["-Iinclude"],
    visibility = ["//visibility:public"],  # Allow the main package to depend on this library
)
