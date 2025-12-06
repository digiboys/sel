"""
Extension to configure a GCC toolchain.
"""

load("@rules_nixpkgs_cc//:cc.bzl", "nixpkgs_cc_configure")

def _cc_configure_impl(_):
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
        cc_std = "c++26",
        cc_libstd = "stdc++",
        require_support_nix = False,
        toolchain_workspace = "gcc_toolchain",
        extra_toolchain_flags = {
            "compile_flags": [
                "-D_GLIBCXX_ASSERTIONS",
                "-fdiagnostics-color",
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
                "-Wno-psabi",
                "-Wno-error=unused-but-set-variable",
                "-Wno-unused-but-set-variable",
            ],
        },
    )

cc_configure = module_extension(
    implementation = _cc_configure_impl,
)
