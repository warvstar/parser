Day 1 - Parser
Day 2 - Interpreter
Day 3 - JIT
Day 4 - Lua

add syntax

import { std, fs, ffi, Engine, Window, InputManager }

__c:
 window = Window 400 800
 inputManager = InputManager
 resourceManager = ResourceManager
 engine = Engine(window, inputManager, resourceManager) //have it so it has a sort of intellisense
__d:
 print "program exit"


//implement resource counting and possibly GC modes
window = unique(Window 400 800)
window = _u(Window 400 800)
handle = shared(100)
handle = _s(100)

app = App

 //will have to parse numbers that get multiplied, to emit code like so
 //v0 = Vec3(1,0,1) + Vec3(1,0,0)
 //v0 = Vec3(1,0,1).Add(Vec3(1,0,0))



 Can run each class initialization and function once to get optimized code

 move over wglGetProcAddress to fs