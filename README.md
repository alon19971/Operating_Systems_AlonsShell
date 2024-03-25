# Alon's Shell - Operating Systems Assignment

Alon's Shell is a custom Unix-based shell implementation, crafted as a final project for the Operating Systems course. It's designed to demonstrate an understanding of core operating systems concepts such as process management, file operations, and command interpretation, all implemented in the C programming language.

## Features

- **Command Parsing:** Break down user input into executable commands and parameters using the `splitArgument()` function.
- **Current Directory Display:** Custom `getLocation()` function that emulates the terminal prompt, showing the current directory, username, and hostname, all with color-coded output for enhanced readability.
- **Executable Commands:** Ability to execute standard Unix commands as well as a set of built-in commands tailored for educational purposes.
- **Environmental Interaction:** Navigate file systems, manage files, and interact with the environment directly through the shell.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

- A Unix-like operating system (Linux, macOS)
- GCC compiler for compiling the project
- Git for cloning the repository

### Installation

1. **Clone the Repository**

   Begin by cloning the repository to your local machine:

   ```bash
   git clone https://github.com/alon19971/Operating_Systems_AlonsShell.git
   cd Operating_Systems_AlonsShell
