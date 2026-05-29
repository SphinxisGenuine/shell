SPX - sphinx (just a random name )
a custom Unix shell in C
# sPx (Sphinx Shell)

I built this shell mainly to understand some low-level Unix concepts and how a shell actually works under the hood.
Instead of just reading about things like `stdin`, `stdout`, processes, pipes, file descriptors, and signals, 
I wanted to build something that would force me to use them. The goal was not to create a production-ready shell, but to learn by implementing the pieces myself.
While building it, I got a much better understanding of:

* How user input is read and processed
* Parsing and tokenization
* `stdin`, `stdout`, and standard I/O
* Process creation with `fork()`
* Executing programs with `execvp()`
* Parent and child processes
* File descriptors and redirection
* Pipes and inter-process communication
* Signal handling with `signal()`
* Shell startup configuration files

## Current Features

* Command execution
* Built-in commands:

  * `cd`
  * `pwd`
* Tokenizer and command parser
* Input redirection (`<`)
* Output redirection (`>`)
* Append redirection (`>>`)
* Pipe parsing and execution (`|`)
* Multi-process execution for pipelines
* Signal handling (`Ctrl+C` kills the running process, not the shell itself)
* Startup RC file support (`.sPxrc`)
* Customizable shell startup behavior

## Example

Here's a small example of the shell in action:

```sh
sPx>> pwd
/home/user/projects/sPx

sPx>> ls | wc -l
12

sPx>> echo hello > output.txt

sPx>> cat output.txt
hello
```

### Screenshot

A screenshot of the shell running:

## What I Would Add Next

There are still many things that a real shell does that this project doesn't handle yet.

Some things I would add next:
 
* `export` builtin
* command history
* aliases
* environment variable expansion
* support for multiple pipes
* improved error handling
* job control
* tab completion
* a better prompt

Something like:

```sh
username@hostname:~/current/directory $
```

## Why I Built It

The main goal was to understand how shells actually work instead of treating them as a black box.

Building even a small shell makes you touch a surprising number of operating system concepts: processes, signals, file descriptors, pipes, redirection, and command execution. It turned out to be one of the most practical ways to learn Unix internals.

