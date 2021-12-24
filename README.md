
# mupen64plus-core-p64api

This is a fork of [mupen64plus-core](https://github.com/mupen64plus/mupen64plus-core) that adds a [Project64-compatible](https://hack64.net/docs/pj64d/apidoc-unstable.php) Javascript Scripting API:

See the original README for more information.

Built with:
 - C++20
 - [Duktape](https://duktape.org/), a tiny Javascript runtime.

# API Overview
    - Control the emulator
    - Print information to user
    - Manipulate memory
    - Access registers
    - Disassemble code
    - Control the debugger
    - Subscribe to events
    - Access the file system
    - Use sockets
    - Interact with other scripts
    - and more

## Why a scripting API?

A scripting API allows for debugging and game modifications without the need to re-compile.

## Why Javascript?

It's popular. It's a nice transpilation target. Duktape was easy to integrate.

## Why C++?

Vectors, maps, strings, optionals, variants, formatting, exceptions, lambdas, iteration, inference, generics, etc, etc.

## NAPI vs SAPI?

Native API vs Scripting API

# API Status

- Duktape Integration
  - ✓ Context creation/destruction NAPI
  - ✓ Context heap metadata NAPI
  - ✓ SAPI injection

- NAPI
    - ✓ Core-Plugin callback `EvaluateJs`

- SAPI
    - console.*
        - ✓ `.log`
        - `.print`
        - `.error`
        - `.clear`
        - `.listen`
    - emu.*
        - `.open`
        - `.close`
        - `.reset`
        - `.pause`
        - `.resume`
        - `.savestate`
        - `.loadstate`
    - mem.*
        - `.u8|u16|u32|s8|s16|s32|f32|f64`
        - `.bindvar`
        - `.bindvars`
        - `.bindstruct`
        - `.typedef`
        - `.getblock`
        - `.setblock`
        - `.getstring`
        - `.setstring`
        - `.ramSize`
        - `.romSize`
        - `.ptr`
    - events.*
        - ✓ `.on`
        - `.onstatechange`
        - `.onexec`
        - `.onread`
        - `.onwrite`
        - `.onopcode`
        - `.ongprvalue`
        - `.onpifread`
        - `.onsptask`
        - `.onpidma`
        - `.ondraw`
        - `.onmousedown`
        - `.onmouseup`
        - `.onmousemove`
        - `.remove`
    - ctx.*
        - `.drawtext`
        - `.measuretext`
        - `.drawimage`
        - `.fillrect`
        - `.strokerect`
        - `.beginpath`
        - `.moveto`
        - `.lineto`
        - `.stroke`
        - `.fill`
        - `.width`
        - `.height`
        - `.fillColor`
        - `.strokeColor`
        - `.strokeWidth`
        - `.fontFamily`
        - `.fontWeight`
        - `.fontSize`
        - `.ptr`
    - image.*
        - `.fromPNG`
        - `.format`
        - `.bpp`
        - `.toPNG`
        - `.update`
        - `.width`
        - `.height`
        - `.pixels`
        - `.palette`
    - cpu.*
        - `.pc`
        - `.hi`
        - `.lo`
        - `.uhi`
        - `.ulo`
        - `.gpr.r0|at|v0|v1..ra`
        - `.ugpr.r0|at|v0|v1..ra`
        - `.fpr.f0..f31`
        - `.dfpr.f0..f30`
        - `.cop0.index..errorpc`
    - debug.*
        - `.breakhere`
        - `.resume`
        - `.step`
        - `.skip`
        - `.showmemory`
        - `.showcommands`
        - `.paused`
    - asm.*
        - `.encode`
        - `.decode`
        - `.gprname`
    - fs.*
        - `.open`
        - `.close`
        - `.write`
        - `.writefile`
        - `.read`
        - `.readfile`
        - `.fstat`
        - `.stat`
        - `.unlink`
        - `.mkdir`
        - `.rmdir`
        - `.readdir`
        - `.Stats`
    - socket.*
        - `.connect`
        - `.write`
        - `.end`
        - `.close`
        - `.on('connect')`
        - `.on('lookup')`
        - `.on('data')`
        - `.on('end')`
        - `.on('close')`
        - `.on('drain')`
        - `.on('error')`
        - `.off`
        - `.remoteAddress`
        - `.remotePort`
        - `.localAddress`
        - `.localPort`
        - `.addressFamily`
    - server.*
        - `.listen`
        - `.close`
        - `.on('connection')`
        - `.on('listening')`
        - `.on('close')`
        - `.on('error')`
        - `.off`
        - `.port`
        - `.address`
        - `.addressFamily`
    - script.*
        - `.keepalive`
        - `.timeout`
    - globals
        - `global`
        - `require`
        - `exec`
        - `alert`
        - `setTimeout`
        - `clearTimeout`
        - `setInterval`
        - `number.hex`
