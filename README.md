# fel
Fast Embedded Lightweight scripting engine. Despite it standing for something it is also a name, so you should spell it like fel and not like F.E.L or something otherwordly. It is pronunced hell but with a F instead of a H.

[Language reference](https://github.com/madeso/fel/blob/master/language.md)

Fel also means wrong/error/bad in swedish. Given that it is considerd a bad thing to create your own language instead of using an existing language this not bad name for a language. Finally, one aim of the language is also to generate errors at "compile" time instead of at run time and we have yet another meaning behind the name. It is also probably a bad idea to allow modifications to the language syntax as a core feature (that's another thing I want to add further down the line).

[![Build Status](https://travis-ci.org/madeso/fel.svg?branch=master)](https://travis-ci.org/madeso/fel)
[![Coverage Status](https://coveralls.io/repos/github/madeso/fel/badge.svg?branch=master)](https://coveralls.io/github/madeso/fel?branch=master)

# Key ideas (or key features if you like)
  * 0 based arrays (as opposed to luas 1)
  * optional/gradual typesafety (similar to python or typescript)
  * JIT as a feature, not as a external library
  * Embedable by design, grab the source and add it to your favorite buildsystem.
  * Editor/intellisense support(mainly having a [LSP](https://github.com/Microsoft/language-server-protocol)-enabled server and a official (fully featured) vs code plugin).
  * Operator overloading support. (like lua, unlike js)
  * debugging library with a 'dear imgui' implementation
  * [jupyter kernel](https://jupyter-client.readthedocs.io/en/stable/kernels.html) via [xeus](https://github.com/jupyter-xeus/xeus/blob/master/example/src/custom_interpreter.cpp) for documentation of the language and playing around
  * Compile time function execution (like converting a string to a hashed value)
  * Easily inspect and reflect compiled code
  * 'visual scripting enabled' with a 'dear imgui' implementation

# Workplan
1. Make sure the basic barebones features work.
2. Quality of life updates and API finalizations. Should be able to replace lua/js at this point.
3. Add perfomance tests agains other scripting languages and become "fastish"
4. Optional/gradual types.

