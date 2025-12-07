"""
Extension to configure LLVM and GCC toolchains.
"""

load("@rules_nixpkgs_cc//:cc.bzl", "nixpkgs_cc_configure")
load("@toolchains_llvm//toolchain:rules.bzl", "llvm_toolchain")

_cxx_standard = "c++26"

_common_toolchain_warnings = [
    "-Werror",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wconversion",
    "-Wnon-virtual-dtor",
    "-Wold-style-cast",
    "-Wcast-align",
    "-Wunused",
    "-Woverloaded-virtual",
    "-Wmisleading-indentation",
    "-Wnull-dereference",
    "-Wdouble-promotion",
    "-Wformat=2",
    "-Wimplicit-fallthrough",
    "-Wextra-semi",
    "-Wunreachable-code",
]

def _cc_configure_impl(_):
    llvm_toolchain(
        name = "llvm_toolchain",
        cxx_flags = {
            "": [
                "-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE",
                "-std=" + _cxx_standard,
                "-stdlib=libc++",
                "-Wshadow",
                "-Wdocumentation-unknown-command",
                "-Wunreachable-code-return",
            ] + _common_toolchain_warnings,
        },
        extra_coverage_compile_flags = {
            "": [
                # https://github.com/bazelbuild/bazel/issues/14970#issuecomment-1894565761
                "-ffile-compilation-dir=.",
            ],
        },
        llvm_version = "20.1.4",
    )

    nixpkgs_cc_configure(
        name = "nixpkgs_config_gcc",
        repository = "@nixpkgs",
        # Provide a GCC toolchain definition for macOS.
        # GCC 15 does not build on darwin-aarch64:
        # https://github.com/NixOS/nixpkgs/issues/425992
        nix_file_content = """
let
  pkgs = import <nixpkgs> {};
  gcc = pkgs.gcc14;
in
  (pkgs.buildEnv {
    name = "gcc-toolchain";
    paths = [
      gcc
    ] ++ pkgs.lib.optionals pkgs.stdenv.isDarwin [
      pkgs.darwin.cctools
    ];
    pathsToLink = [
        "/bin"
        "/lib"
        "/lib64"
        "/libexec"
        "/include"
    ];
  }) // {
    inherit (gcc) targetPrefix isGNU isClang;
  }
        """,
        register = False,
        fail_not_supported = True,
        cc_std = _cxx_standard,
        cc_libstd = "stdc++",
        require_support_nix = False,
        toolchain_workspace = "gcc_toolchain",
        extra_toolchain_flags = {
            "compile_flags": [
                "-D_GLIBCXX_ASSERTIONS",
                "-fdiagnostics-color",
                "-Wno-psabi",
                "-Wno-unused-but-set-variable",
            ] + _common_toolchain_warnings,
        },
    )

cc_configure = module_extension(
    implementation = _cc_configure_impl,
)
