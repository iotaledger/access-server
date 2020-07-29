# Engineering Specification

## Summary

This document contains technical information about **IOTA Access**. It is meant to guide engineers willing to collaborate with the project.

## Programming Language

The **IOTA Access Core Server** is designed to run on Embedded Systems and IoT devices. Thus, it is imperative that it is implemented in the C programming language ([C99](http://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf)), in order to ensure ABI compatibility with these target devices.

[CMake](https://cmake.org/) v3.11 is used as Build System.

[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) is used to ensure that the codebase is formatted under the same style. [GNU  style](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#configurable-format-style-options) is used with a few modifications, namely:

```
AlwaysBreakAfterDefinitionReturnType: None
AlwaysBreakAfterReturnType: None
BraceWrapping:
  IndentBraces: false
BreakBeforeBraces: Custom
IndentCaseLabels: true
IndentWidth: 4
SpaceBeforeParens: ControlStatements

```
