# Ctrl+B 一键编译运行（VS Code + GCC）

## 1. 编译器

WinLibs GCC 16.2，装在 D 盘：

```
D:\softwares\winlibs\mingw64\bin\g++.exe
```

安装命令：

```bash
winget install --id BrechtSanders.WinLibs.POSIX.UCRT -e --location "D:\softwares\winlibs"
```

## 2. 编译运行任务 `.vscode/tasks.json`

放在 VS Code **打开的那个文件夹**的
`.vscode` 里才生效。

```jsonc
{
    "version": "2.0.0",
    "tasks": [
        {
            // 任务名，快捷键靠它找到任务
            "label": "编译运行",
            "type": "shell",
            // 编译当前文件 → 成功就运行
            // JSON 里路径要写双反斜杠 \\
            "command": "chcp 65001 >nul && \"D:\\softwares\\winlibs\\mingw64\\bin\\g++.exe\" -std=c++17 -O2 -Wall -static \"${file}\" -o \"${fileDirname}\\${fileBasenameNoExtension}.exe\" && \"${fileDirname}\\${fileBasenameNoExtension}.exe\"",
            "options": {
                "cwd": "${fileDirname}",
                // 用 cmd 跑，&& 才好用
                "shell": { "executable": "cmd.exe", "args": ["/d", "/c"] }
            },
            "group": { "kind": "build", "isDefault": true },
            "presentation": {
                "reveal": "always",
                "focus": true, // 焦点给终端，好输入
                "panel": "shared",
                "clear": true
            },
            // 编译错误显示到"问题"面板
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

## 3. 快捷键（用户级）

文件：`%APPDATA%\Code\User\keybindings.json`

```jsonc
[
    {
        // 只在 C/C++ 文件里接管 Ctrl+B
        // 其他地方仍是开关侧边栏
        "key": "ctrl+b",
        "command": "workbench.action.tasks.runTask",
        "args": "编译运行",
        "when": "editorLangId == cpp || editorLangId == c"
    }
]
```

## 4. 代码提示（去掉头文件红线）

`.vscode/settings.json`：

```jsonc
{
    // 运行前自动保存
    "task.saveBeforeRun": "always",
    // 让提示用 GCC，认识 bits/stdc++.h
    "C_Cpp.default.compilerPath": "D:/softwares/winlibs/mingw64/bin/g++.exe",
    "C_Cpp.default.cppStandard": "c++17"
}
```

## 5. 踩过的坑

- 没装编译器 → 头文件报错
- VS Code 打开的是上级目录
  → 本目录 tasks.json 不生效
  → 上级 `.vscode` 也放一份
- JSON 路径单反斜杠被当转义
  → 必须写 `\\`

## 6. 测试用例（schedule.cpp）

输入：

```
3 7
1 1 2
0 1 5 1
1 1 3 2
2 1 4 5
3 1 2 1
0 2 4 1
1 2 2 3
2 2 1 3
```

- 第 1 行：设备数 m、样品数 n
- 第 2 行：每台设备的类型
- 之后每行：到达 类型 耗时 优先级

期望输出（开始 结束）：

```
0 5
1 4
4 8
5 7
0 4
4 6
6 7
```
