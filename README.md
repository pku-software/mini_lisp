# Mini_lisp - 一种 Lisp 方言解释器的 C++ 实现

> 以下为 Xmake VS Code 配合使用说明

## 使用方法

1. 使用 [VS Code Config Helper v4](https://v4.vscch.tk) 配置一遍。这只是确保你正确安装了编译器。如果你之前配置过，则可以跳过这一步；如果你是第一次配置，按照程序说明完成所有操作。配置完成后，可**直接删除工作文件夹**，这里不需要它。
1. 下载或克隆本仓库。
2. 安装 [Xmake](https://xmake.io/#/zh-cn/guide/installation)。
3. 在 VS Code 中打开本仓库。
4. 在 VS Code 的终端中执行如下 Configure 命令：

```
xmake f
```

**如果你在 Windows 上使用 MinGW，则将上述命令改为 `xmake f -pmingw`。**

## 编辑代码

你可以将你的源代码放在 `src` 文件夹中。本仓库提供了简单的测试代码，放在 `src/main.cpp` 中。在你开始编辑或进行文件操作（比如添加/删除/重命名源文件）时，建议你运行 `update intellisense` 任务以获得良好的 IntelliSense 智能提示。

运行 `update intellisense` 的方法是：按 <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd>，执行 `Tasks: Run task` 命令，选择 `update intellisense` 运行。如果智能提示出现异常（如`#include error detected` 等），则运行此命令可以解决大部分问题。

## 编译与运行

按 <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd> 编译项目。按 <kbd>F6</kbd>（即你在 VS Code Config Helper 中选择的运行快捷键）运行项目。

## 调试

首先安装 [Xmake VS Code 扩展](https://marketplace.visualstudio.com/items?itemName=tboox.xmake-vscode) 以启用调试功能。

请在正确配置 `launch.json` 的前提下，选择菜单栏“运行->开始调试”或者在调试活动栏中点击“播放”按钮以调试。

> **Warning**
>
> 请不要通过 <kbd>F5</kbd> 、Xmake 侧边活动栏或底部状态栏调试（不稳定，易出问题）。若想使用 <kbd>F5</kbd> 快捷键，建议前往“文件->首选项->键盘快捷方式”，删除 Xmake Debug 命令的 <kbd>F5</kbd> 快捷键。

## 其它

- 你可以修改 `xmake.lua` 中的 `target` 函数调用，以修改项目名称和最终生成的可执行文件名称。
- 你可以使用 Clangd 而非 C/C++ 来提供智能提示。请参考 `c_cpp_properties.json` 中的说明操作。
