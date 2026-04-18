# Slim
Terminal Connected CLI that uses a Local connected LLM using Ollama / LM Studio to give suggestions when mistakes are made.



## Idea


- The CLI Wrapper (C instead of Rust/Python): Instead of using Python or Rust for the parser as originally pitched, we use statically compiled C. The C binary intercepts the target command (like gcc or cargo build), reads stderr, and uses POSIX regex to isolate the exact block containing the ERROR or PANIC state. It strips out the timestamps and successful compilation lines instantly.

- The LLM Bridge (Python): The C parser pipes that highly constrained error block to your Python script. Python packages it with the system prompt: "You are an expert systems programmer. Explain this error in one sentence, and provide the most likely code fix."

- The Inference Engine (llama.cpp): Python sends the prompt to your local 4B model (like Qwen 2.5 3B). Because the context window is strictly constrained by the C parser, the model doesn't hallucinate. It processes the request rapidly, leveraging Vulkan on your AMD hardware for fast local inference.

## PLan


- The C Interceptor: A compiled C program (explain) takes your target command as arguments (e.g., explain gcc main.c). It executes the command, intercepts stderr, uses C regex to extract the actual error block, and pipes that clean string out.

- The Python Bridge: A Python script (ask_llm.py) receives the clean error string, constructs the strict system prompt, and communicates with your local LLM server.

- The LLM Backend: A local 3B/4B model running via llama.cpp.


To ensure the parser compiles on any CPU architecture without modification, we must avoid architecture-specific intrinsics or platform-locked system calls (like Linux-only epoll or Windows CreateProcess). The aim is to make the C interceptor truly architecture-agnostic (running flawlessly across x86_64 for Intel/AMD and aarch64 for ARM).

---


Phase 1: Local LLM Infrastructure

Phase 2: The C Interceptor (Execution & Parsing)

Phase 3: The Python Bridge (The Semantic Engine)

Phase 4: Integration & Polish


Tuning the Parser. This will likely find that different compilers format errors differently. This can be fixed by expanding C regex logic to detect the environment (e.g., if the command is gcc, use the C-style regex; if it's cargo, use the Rust-style regex).
