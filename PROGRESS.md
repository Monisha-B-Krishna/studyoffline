## July 11
 - Brainstroming ideas

## July 12
- Repo initialized, MIT license added.
- Set up local development environment (Git, VS Code).
- Locked technical decisions: native Kotlin, llama.cpp for LLM inference,
  Qwen2.5-0.5B-Instruct (Q4_K_M GGUF), ONNX Runtime Mobile + MiniLM for
  embeddings, SQLite for storage.
- Android Studio installed, project scaffolded (Kotlin + Jetpack Compose).
- Physical device (Realme RMX5000) connected via USB debugging.
- App builds and runs successfully on real hardware - confirmed "Hello Android!" on screen 
- Next: Integrate llama.cpp for on-device LLM interface.
## July 15
- Fixed native build: restricted ABI to arm64-v8a only (fixes 32-bit ARM float16 intrinsic errors, also much faster builds).
- Removed unnecessary 'common' library link that caused linker errors.
- Full build (Kotlin + Compose + llama.cpp native inference code) succeeds in ~30s.
- Model file (Qwen2.5-0.5B-Instruct Q4_K_M, 491MB) pushed to device at /sdcard/Download/model.gguf.
- Next: wire Kotlin UI to call native inference function and display real generated text on screen.
