# fel
Fast Embedded Lightweight scripting engine. Despite it stands for something it is also a same so you should spell it like fel and not like F.E.L or something.
Fel also means wrong/error/bad in swedish. It is a bad thing to create your own language, why not use an existing language? One aim of the language is also to generate errors at "compile" time instead of at run time.

[![Build Status](https://travis-ci.org/madeso/fel.svg?branch=master)](https://travis-ci.org/madeso/fel)

# Main features
  * 0 based arrays (as opposed to luas 1)
  * optional/gradual typesafety
  * JIT as a feature, not as a external library
  * Embedable by design, grab the C source and add it to your favorite buildsystem.
  * Customaizable syntax. Don't like {}, switch them out in a header before building
  * Editor/intellisense support(mainly having a [LSP](https://github.com/Microsoft/language-server-protocol)-enabled server).
  * Operator overloading support. (like lua, unlike js)
