# fel
Fast Embedded Lightweight scripting engine. Despite it standing for something it is also a name, so you should spell it like fel and not like F.E.L or something otherwordly. It is pronunced hell but with a F instead of a H.

Fel also means wrong/error/bad in swedish. Given that it is considerd a bad thing to create your own language instead of using an existing language this not bad name for a language. Finally, one aim of the language is also to generate errors at "compile" time instead of at run time and we have yet another meaning behind the name. It is also probably a bad idea to allow modifications to the language syntax as a core feature.

[![Build Status](https://travis-ci.org/madeso/fel.svg?branch=master)](https://travis-ci.org/madeso/fel)

# Main features
  * 0 based arrays (as opposed to luas 1)
  * optional/gradual typesafety
  * JIT as a feature, not as a external library
  * Embedable by design, grab the source and add it to your favorite buildsystem.
  * Editor/intellisense support(mainly having a [LSP](https://github.com/Microsoft/language-server-protocol)-enabled server).
  * Operator overloading support. (like lua, unlike js)

# Workplan
  * Make sure the basic features work.
  * Make fast and lean
  * Switch from c++ to c
  * Customaizable syntax. Don't like {}, switch them out in a header before building

# Todo
  * parse comments
  * call functions with strings
  * call functions with basic arithmetics (+)
  * functions can error and have return values
  * parse scopes
  * variables
  * if statements
  * double type
  * more arithmetics
  * objects
  * functions as variables
  * custom functions
  * captures
  * arrays
  * meta objects/tables
  * optional types

