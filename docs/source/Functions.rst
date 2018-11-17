Functions
=============

By `Robert Stewart <https://github.com/warvstar/>`_
Functions can be created in multiple ways::

    # Assign 8 to x
    repl> x: print(2); return 1 # declared without arguments or return type, semicolon can be used to have multiple statements on one block.
    repl> x!
    1

    repl> x(i32 n)i32: print(n); return n # declared with arguments, and types.
    repl> x(2)
    2
    2

    repl> x(n) print(n); return n # declared with arguments, without types.
    repl> x(2)
    2
    2

Functions can omit the return keyword, last variable is returned::
    barkTwice:
     print("bark")
     print("bark")
     1
    repl>bark!
    "bark"
    "bark"
    1

Functions can exists at any scope::

    class A
     bark:
      woof: print("Woof") # This is a function inside a function
      woof!

    repl>A!.bark!
    "Woof"