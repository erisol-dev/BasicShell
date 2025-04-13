# CustomShell

A mini shell program written in C that supports task management, foreground/background execution, piping, chaining, signal handling, and command I/O redirection.

## Features
- Foreground and background task execution
- Task listing and deletion
- Piping (`pipe`) and command chaining (`chain`)
- IO redirection (`<`, `>`, `>>`)
- Signal handling: `suspend`, `resume_fg`, `resume_bg`, `kill`
- Built-in commands: `list`, `start_fg`, `start_bg`, `fg`, `quit`, etc.

## Requirements
- POSIX-compliant environment (Linux, macOS, or WSL)
- GCC with C11 support
- Won't run on Windows MinGW (due to fork/wait/pipe)

## Build
```bash
gcc -o shell src/shell.c -Iinc
