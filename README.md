# Mini_lisp - 一种 Lisp 方言解释器的 C++ 实现

开发环境：VS Code + Xmake

## 一般环境

```
xmake f
xmake
```

`bin` 中即包含了可执行文件。

## WASM

[安装](https://emscripten.org/docs/getting_started/downloads.html) Emscripten 环境。激活该环境。

```
xmake f -p wasm
xmake
```

`bin` 中即包含了 `mini_lisp.js` 与 `mini_lisp.wasm`。