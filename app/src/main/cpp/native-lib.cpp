#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include "llama.h"

#define LOG_TAG "StudyOffline"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_studyoffline_NativeLib_runInference(JNIEnv* env, jobject /* this */, jstring modelPath, jstring prompt) {
    const char* model_path_cstr = env->GetStringUTFChars(modelPath, nullptr);
    const char* prompt_cstr = env->GetStringUTFChars(prompt, nullptr);

    std::string result;

    llama_backend_init();

    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 0; // CPU only

    llama_model* model = llama_model_load_from_file(model_path_cstr, model_params);
    if (model == nullptr) {
        LOGE("Failed to load model from %s", model_path_cstr);
        result = "ERROR: Failed to load model";
        env->ReleaseStringUTFChars(modelPath, model_path_cstr);
        env->ReleaseStringUTFChars(prompt, prompt_cstr);
        return env->NewStringUTF(result.c_str());
    }

    LOGI("Model loaded successfully from %s", model_path_cstr);

    const llama_vocab* vocab = llama_model_get_vocab(model);

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 2048;

    llama_context* ctx = llama_init_from_model(model, ctx_params);
    if (ctx == nullptr) {
        LOGE("Failed to create context");
        result = "ERROR: Failed to create context";
        llama_model_free(model);
        env->ReleaseStringUTFChars(modelPath, model_path_cstr);
        env->ReleaseStringUTFChars(prompt, prompt_cstr);
        return env->NewStringUTF(result.c_str());
    }

    std::string prompt_str(prompt_cstr);
    std::vector<llama_token> tokens(prompt_str.size() + 8);
    int n_tokens = llama_tokenize(vocab, prompt_str.c_str(), (int32_t)prompt_str.size(),
                                  tokens.data(), (int32_t)tokens.size(), true, true);
    tokens.resize(n_tokens);

    LOGI("Prompt tokenized into %d tokens", n_tokens);

    llama_batch batch = llama_batch_init(512, 0, 1);
    for (size_t i = 0; i < tokens.size(); i++) {
        batch.token[i] = tokens[i];
        batch.pos[i] = (llama_pos)i;
        batch.n_seq_id[i] = 1;
        batch.seq_id[i][0] = 0;
        batch.logits[i] = (i == tokens.size() - 1);
    }
    batch.n_tokens = (int32_t)tokens.size();

    if (llama_decode(ctx, batch) != 0) {
        LOGE("Failed to decode prompt");
        result = "ERROR: Failed to decode prompt";
        llama_batch_free(batch);
        llama_free(ctx);
        llama_model_free(model);
        env->ReleaseStringUTFChars(modelPath, model_path_cstr);
        env->ReleaseStringUTFChars(prompt, prompt_cstr);
        return env->NewStringUTF(result.c_str());
    }

    int n_cur = (int)tokens.size();
    int n_gen_max = 64;
    std::string generated_text;
    int n_vocab = llama_vocab_n_tokens(vocab);

    for (int i = 0; i < n_gen_max; i++) {
        auto* logits = llama_get_logits_ith(ctx, batch.n_tokens - 1);

        llama_token best_token = 0;
        float best_logit = logits[0];
        for (int t = 1; t < n_vocab; t++) {
            if (logits[t] > best_logit) {
                best_logit = logits[t];
                best_token = t;
            }
        }

        if (llama_vocab_is_eog(vocab, best_token)) {
            break;
        }

        char piece[128];
        int piece_len = llama_token_to_piece(vocab, best_token, piece, sizeof(piece), 0, true);
        if (piece_len > 0) {
            generated_text.append(piece, piece_len);
        }

        llama_batch next_batch = llama_batch_init(1, 0, 1);
        next_batch.token[0] = best_token;
        next_batch.pos[0] = n_cur;
        next_batch.n_seq_id[0] = 1;
        next_batch.seq_id[0][0] = 0;
        next_batch.logits[0] = true;
        next_batch.n_tokens = 1;

        if (llama_decode(ctx, next_batch) != 0) {
            llama_batch_free(next_batch);
            break;
        }
        llama_batch_free(next_batch);
        n_cur++;
    }

    LOGI("Generated: %s", generated_text.c_str());

    llama_batch_free(batch);
    llama_free(ctx);
    llama_model_free(model);
    llama_backend_free();

    env->ReleaseStringUTFChars(modelPath, model_path_cstr);
    env->ReleaseStringUTFChars(prompt, prompt_cstr);

    return env->NewStringUTF(generated_text.c_str());
}