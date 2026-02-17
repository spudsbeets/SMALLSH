# SMALLSH – A Minimal Unix Shell in C

## Overview

**smallsh** is a simplified Unix-like shell written in C. It supports execution of foreground and background processes, input/output redirection, and basic built-in commands.

---

## Features

### ✔ Built-in Commands

#### `exit`
Terminates the shell and sends `SIGTERM` to any running background processes before exiting.

#### `cd [path]`
Changes the current working directory.

- If no path is provided, it defaults to `$HOME`.

#### `status`
Displays the exit status or terminating signal of the most recent foreground process.

---

### ✔ Foreground & Background Execution

- Commands run in the foreground by default.
- Appending `&` runs a command in the background (unless foreground-only mode is active).
- Background process completion is checked each prompt cycle using `waitpid()` with `WNOHANG`.

---

### ✔ Input and Output Redirection

- Redirect input with `<`
- Redirect output with `>`

---

### ✔ Signal Handling

#### SIGINT (Ctrl+C)

- Ignored by the shell itself.
- Foreground child processes use default behavior (can be terminated).
- Background processes ignore `SIGINT`.

#### SIGTSTP (Ctrl+Z)

- Toggles foreground-only mode.
- When enabled, `&` is ignored.
- Implemented using `volatile sig_atomic_t` to safely track state inside the signal handler.

---

## Compilation

Compile using `gcc`:

`bash`
`gcc -std=gnu99 -Wall -Wextra -o smallsh smallsh.c`

## Running

`./smallsh`
