# Slim

A terminal-native CLI interceptor that wraps any compiler or build tool, captures its error output, filters it with POSIX regex, and forwards the isolated error block to a locally-running LLM via Ollama. The model returns a concise, actionable fix. No data leaves the machine.

---

## Architecture

```
user invokes:  slim gcc main.c
                     |
             scripts/slim  (bash entry point)
                     |
         slim_interceptor  (C binary)
                     |
         forks child process, executes gcc
         captures stderr via popen(cmd, "r") with 2>&1 redirect
         filters output with POSIX regex (regcomp / regexec)
         writes clean error block to stdout
                     |
                  pipe |
                     |
             ask_llm.py  (Python bridge)
                     |
         reads stdin, constructs prompt
         POST /api/chat -> localhost:11434
                     |
             Ollama  (local inference)
                     |
         model returns one-line fix
         printed to terminal
```

---

## Requirements

- Linux or macOS (POSIX-compliant system required)
- GCC or Clang with C11 support
- Python 3.8 or later
- [Ollama](https://ollama.com) installed and running locally
- A pulled model compatible with the Ollama `/api/chat` endpoint

```sh
# Verify dependencies
gcc --version
python3 --version
ollama --version
```

---

## Ollama Setup

```sh
# Install Ollama (Linux)
curl -fsSL https://ollama.com/install.sh | sh

# Pull a model (gemma4:e2b used by default in ask_llm.py)
ollama pull gemma4:e2b

# Start the Ollama server (runs on localhost:11434)
ollama serve
```

Confirm the server is accepting requests:

```sh
curl http://localhost:11434/api/tags
```

---

## Installation

### 1. Clone the repository

```sh
git clone https://github.com/MarawanYakout/Slim.git
cd Slim
```

### 2. Install Python dependency

```sh
pip3 install requests
```

### 3. Compile the C interceptor

```sh
make
```

This produces the `slim` binary in the project root via:

```sh
gcc -Wall -Wextra -std=c11 -o slim src/slim.c
```

### 4. Install system-wide

```sh
make install
```

This runs the following:

```sh
sudo mkdir -p /usr/local/lib/slim
sudo cp slim                  /usr/local/lib/slim/slim_interceptor
sudo cp bridge/ask_llm.py     /usr/local/lib/slim/ask_llm.py
sudo cp scripts/slim          /usr/local/bin/slim
sudo chmod +x                 /usr/local/bin/slim
```

After this step, `slim` is available in any shell session without specifying a path.

---

## Usage

Prefix any compiler or build command with `slim`:

```sh
slim gcc main.c
slim gcc main.c -o output
slim cargo build
slim python3 script.py
```

### Example output

```sh
$ slim gcc main.c

[slim] Running: gcc main.c 2>&1
main.c:5:10: error: 'undefined_var' undeclared (first use in this function)

[slim] Suggestion:
Did you mean: int undefined_var = 0; declared before use in main()
Why: the variable is referenced before being declared in scope.
```

---

## Project Structure

```
Slim/
├── src/
│   └── slim.c              # C interceptor: process spawning, stderr capture, POSIX regex filtering
├── bridge/
│   └── ask_llm.py          # Python bridge: stdin reader, Ollama HTTP client, response formatter
├── scripts/
│   └── slim                # Bash entry point: connects the C binary and Python bridge via pipe
├── Makefile                # Build, install, uninstall, and test targets
└── README.md
```

---

## Makefile Targets

| Target | Description |
|---|---|
| `make` | Compile `src/slim.c` into the `slim` binary |
| `make install` | Install all components to `/usr/local/lib/slim` and `/usr/local/bin` |
| `make uninstall` | Remove all installed files from the system |
| `make clean` | Remove the local compiled binary |
| `make test` | Run the C interceptor against a broken C file to verify error capture |

---

## Configuration

To change the model or inference parameters, edit `bridge/ask_llm.py`:

```python
data = {
    "model": "gemma4:e2b",       # replace with any model pulled via ollama pull
    "stream": False,
    "options": {
        "temperature": 0.5,      # lower = more deterministic output
        "num_predict": 100,      # max tokens in the response
    },
    "keep_alive": "3m",          # keep model loaded in memory for 3 minutes
    "think": False
}
```

---

## Uninstall

```sh
make uninstall
```

Removes `/usr/local/lib/slim/` and `/usr/local/bin/slim` from the system.

---

## Platform Support

| Platform | Supported |
|---|---|
| Linux (x86_64, aarch64) | Yes |
| macOS | Yes |
| Windows (native) | No |
| WSL2 | Yes |

Windows is explicitly unsupported. The C interceptor relies on POSIX APIs (`popen`, `pclose`, POSIX regex) that have no native Windows equivalent. Use WSL2 on Windows.

---

## Author

- Marawan Yakout

## Contributers

- Pinebruiser
