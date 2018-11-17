Internals
=============

By `Robert Stewart <https://github.com/warvstar/>`_

FutureScript has three modes.

Bytecode interpreter
Just in time compilation
Ahead of time compilation with optional conversion to C++ for llvms optimizations.

All three modes work can work on the same code base.

The idea for FutureScript came from the desire to have an effecient dynamic language with quick compilation.

In the pursuit of such a language, I have observed the following languages and compilers as been nearly what I needed.

C++:
 Pros:
  * Has quick compilation, very optimized machine code. A jit compiler is hardly needed as compile times can be nearly instant if keeping modules small.
 Cons:
  * Some platforms do not allow dynamic linking of code at runtime unless packaged with app. (I'm looking at you Android7.0+)
  * Lacks flexibility in langauge.

Lua:
 Pros:
  * Small library size, has a very fast Jit compiler.
 Cons:
  * I do not like the call by sharing system.
  * I do not like 1 based indexing.
  * Requires allot of boiler plate code for OOP.

Julia:
 Pros:
  * Fast effecient machine langauge is also generated. Very flexible language.
 Cons:
  * 1 based indexing.
  * Huge library size, 14mb+.
  