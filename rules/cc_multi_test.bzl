"""
Macro to create multiple configurations of a 'cc_test' target.
"""

load("@rules_cc//cc:cc_test.bzl", "cc_test")
load(
    "@rules_multitransition//:defs.bzl",
    "multitransition_test",
    "transition_config",
)

default_transitions = [
    transition_config(
        name = "{}-{}".format(toolchain, compilation_mode),
        compilation_mode = compilation_mode,
        extra_toolchains = ["//toolchain:{}_toolchain".format(toolchain)],
    )
    for toolchain in [
        "gcc",
        "llvm",
    ]
    for compilation_mode in [
        "opt",
        "fastbuild",
    ]
]

def cc_multi_test(
        *,
        timeout = "short",
        transitions = default_transitions,
        **kwargs):
    multitransition_test(
        cc_test,
        transitions = transitions,
        timeout = timeout,
        **kwargs
    )
