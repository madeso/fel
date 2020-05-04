# fel
Fast Embedded Lightweight scripting engine. Despite it standing for something it is also a name, so you should spell it like fel and not like F.E.L or something otherwordly. It is pronunced hell but with a F instead of a H.

[Language reference](https://github.com/madeso/fel/blob/master/language.md)

Fel also means wrong/error/bad in swedish. Given that it is considerd a bad thing to create your own language instead of using an existing language this not bad name for a language. Finally, one aim of the language is also to generate errors at "compile" time instead of at run time and we have yet another meaning behind the name. It is also probably a bad idea to allow modifications to the language syntax as a core feature (that's another thing I want to add further down the line).

[![Build Status](https://travis-ci.org/madeso/fel.svg?branch=master)](https://travis-ci.org/madeso/fel)
[![Coverage Status](https://coveralls.io/repos/github/madeso/fel/badge.svg?branch=master)](https://coveralls.io/github/madeso/fel?branch=master)

# Key features
  * 0 based arrays (as opposed to luas 1)
  * optional/gradual typesafety
  * JIT as a feature, not as a external library
  * Embedable by design, grab the source and add it to your favorite buildsystem.
  * Editor/intellisense support(mainly having a [LSP](https://github.com/Microsoft/language-server-protocol)-enabled server).
  * Operator overloading support. (like lua, unlike js)

# Workplan
1. Make sure the basic barebones features work.
2. Quality of life updates and API finalizations. Should be able to replace lua/js at this point.
3. Add perfomance tests agains other scripting languages and become "fastish"
4. Optional/gradual types.

