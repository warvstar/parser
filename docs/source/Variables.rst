Variables
=============

By `Robert Stewart <https://github.com/warvstar/>`_

Variables in FutureScript can store anything.
Here are some examples::

    # Assign 8 to x
    repl> x = 8
    8

    # Assign a function to x
    repl> x = ()int: return 1
    Method: ()int
    repl> x!
    1

    # Reassign x's value
    repl> x = 1 + 1 + 1
    3

    
Scope
-----
Accessing parent scoped variables with @
::

    class A
     v = 1
     b:
      setV: @v = 3 # This is a function inside a function
    
    repl>a = A!
    repl>a.v
    1

    # call a.setV
    repl>a.setV!
    repl>a.v
    3

    # Reassign the setV function
    repl>a.setV = v = 1 # setV will not assign 1 to local scope variable v
    repl>a.setV!
    repl>a.v
    3

    # Reassign the setV function
    repl>a.setV = @v = 1 # setV will assign 1 to parent scope variable v, if it exists, otherwise an error is emmited.
    repl>a.setV!
    repl>a.v
    1