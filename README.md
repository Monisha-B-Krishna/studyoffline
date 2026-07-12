# StudyOffline

A fully offline, on-device study companion for Android. Load your notes and PDFs once, then study, ask questions, and get quizzed — entirely in airplane mode. No cloud AI calls, ever.

## Problem

Most "offline" study apps aren't actually offline where it counts: they let you *view* content offline, but the AI generation (Q&A, quizzes, grading) still calls a cloud LLM API. That breaks the moment there's no signal — which is a real, daily problem in parts of my university with no indoor mobile coverage. It also breaks the moment a student deliberately puts their phone in airplane mode to focus, which many students do on purpose.

Airplane mode isn't an edge case for this app. It's the state it's built for.

## Solution

StudyOffline runs the entire AI pipeline — document retrieval, question answering, and quiz generation — on the phone itself, using a small quantized language model and a small quantized embedding model bundled with the app. Once your documents are loaded (via local file picker, Bluetooth, Wi-Fi Direct, or USB — no internet required for ingestion either), the app never needs a network connection again.

## On-Device AI Usage

- **Local LLM inference**: Qwen2.5-0.5B-Instruct, GGUF format, Q4_K_M quantization, run via [llama.cpp](https://github.com/ggml-org/llama.cpp)'s Android bindings. Used for RAG-based Q&A and quiz generation.
- **Local embeddings**: all-MiniLM-L6-v2, quantized, run via ONNX Runtime Mobile. Used to embed document chunks for retrieval.
- **Local vector search**: cosine similarity over an in-memory/SQLite-backed index — no external vector database.
- **Local storage**: SQLite for document chunks, embeddings, quiz history, and weak-topic tracking.
- **Structured output reliability**: quiz JSON is constrained at generation time using a GBNF grammar in llama.cpp, so the model cannot emit malformed output — rather than relying on prompt engineering and retries alone.

No API calls to OpenAI, Gemini, Claude, or any cloud LLM/embedding service are made anywhere in the core pipeline.

## Tech Stack

| Layer | Choice |
|---|---|
| Platform | Android (native Kotlin) |
| LLM inference | llama.cpp (JNI/NDK) |
| LLM model | Qwen2.5-0.5B-Instruct-Q4_K_M.gguf |
| Embeddings | ONNX Runtime Mobile + all-MiniLM-L6-v2 |
| PDF parsing | PdfBox-Android |
| Storage | SQLite (Room) |
| Document transfer | Android file picker, Nearby Share / Wi-Fi Direct |

## Setup and Usage

_(filled in as build progresses — see PROGRESS.md for current state)_

## Demo and Screenshots

_(added closer to submission)_

## License

MIT — see [LICENSE](LICENSE).