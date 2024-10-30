# Valgrind Memory Leak Checker

This repository provides a set of GitHub Actions workflows to automate testing for memory leaks in C/C++ projects using [Valgrind](http://valgrind.org/). 
These workflows can be used as a reusable GitHub Action in any repository with a compiled C/C++ binary, facilitating continuous integration for memory management.

## Features

- **Compilation Workflow**: Compiles C source code and saves the output binary as an artifact.
- **Valgrind Workflow**: Downloads the compiled binary and runs it through Valgrind to identify memory leaks.
- **Reusable**: The Valgrind Workflow can be integrated with other repositories to validate memory management in compiled executables.

---

## Workflows

### 1. Compile C Project Workflow

This workflow compiles the C project, creating an executable artifact that will later be tested with Valgrind.

#### Workflow Configuration

```yaml
name: Compile C Project

on:
  workflow_call: {}  # Allows this workflow to be called by other workflows

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - name: Checkout code
      uses: actions/checkout@v4

    - name: Install dependencies
      run: sudo apt update && sudo apt install -y build-essential

    - name: Compile C project
      run: |
        gcc $(find . -name "*.c")  # Compiles all .c files, output will be a.out by default
        ls -lah  # List the compiled files to verify

    - name: Save binary as artifact
      uses: actions/upload-artifact@v4
      with:
        name: compiled-binary
        path: ./a.out  # Default binary output name after compilation
        retention-days: 3
```

### 2. Valgrind Memory Leak Test Workflow

This workflow retrieves the compiled binary artifact and runs it through Valgrind to detect memory leaks and other memory issues.

#### Workflow Configuration

```yaml
name: Valgrind Workflow

on:
  workflow_call:
    inputs:
      name:
        type: string
        description: 'Name of the compiled binary artifact'
        required: false
        default: 'a.out'

jobs:
  run-valgrind:
    name: Run Valgrind on the C binary
    runs-on: ubuntu-latest

    steps:
      - name: Download compiled binary
        uses: actions/download-artifact@v4
        with:
          name: compiled-binary
          path: ./

      - name: Make the binary executable
        run: chmod +x ./${{ inputs.name }}

      - name: Install Valgrind
        run: sudo apt update && sudo apt install -y valgrind

      - name: Run Valgrind
        run: |
          valgrind --leak-check=full \
                   --track-origins=yes \
                   --show-reachable=yes \
                   --error-exitcode=1 \
                   ./${{ inputs.name }}
```

---

## Usage

### Step 1: Add the Compile Workflow to Your Repository (if needed)

To start using this workflow in your project, copy the `Compile C Project` workflow file into your repository under `.github/workflows/compile.yml`.
This file will compile your C/C++ project and save the binary as an artifact for the Valgrind tests.

### Step 2: Add the Valgrind Workflow

Copy the `Valgrind Workflow` YAML file into your repository as `.github/workflows/valgrind.yml`. This workflow will run Valgrind on the compiled binary to check for memory leaks.

### Step 3: Using the Workflows Together

Here is an example of integrating the workflows into another repository where you want to test memory leaks in a compiled C/C++ binary.

In the target repository:

```yaml
name: Makefile CI

on:
  push: null

permissions:
  contents: read
  statuses: write

jobs:
  compile:
    name: Compile C++ Project
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Build Project and Run Tests
      run: make distcheck

    - name: Create artifact
      uses: actions/upload-artifact@v4
      with:
        name: compiled-binary
        path: ./bin/test_suite
        retention-days: 3

  test-valgrind:
    name: Test Memory Leaks with Valgrind
    needs: compile
    uses: YourGitHubUsername/Valgrind/.github/workflows/valgrind.yml@main
    with:
      name: test_suite
```

In this example:
- **`compile` Job**: Compiles the project using `make distcheck`, creating an executable `test_suite`.
- **`test-valgrind` Job**: Uses the Valgrind Workflow from the `Valgrind` repository to check for memory leaks in the compiled binary `test_suite`.

---

## Customizing Valgrind Options

The Valgrind Workflow **will** includes customizable options to control the level of detail in memory checks:

- **`--leak-check=full`**: Provides detailed leak information.
- **`--track-origins=yes`**: Tracks the origin of uninitialized values.
- **`--show-reachable=yes`**: Shows reachable blocks that are not freed but can still be accessed.
- **`--error-exitcode=1`**: Causes Valgrind to return an error if memory leaks are detected.

---

## Example Output

Upon successful execution, Valgrind will output memory leak details directly in the GitHub Actions log. 
If any leaks are detected, the `error-exitcode=1` option will cause the workflow to fail, making it easy to detect memory management issues in CI.

---

## License

This repository is open source and available under the [MIT License](LICENSE).
