#include <jni.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <android/log.h>
#include "whisper.h"

#define LOG_TAG "NativeWhisper"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static struct whisper_context *g_ctx = nullptr;

whisper_context *whisper_init_from_file_with_params(const char *string);

static std::mutex g_mutex;

whisper_context *whisper_init_from_file_with_params(const char *string) {
    return nullptr;
}

extern "C"

extern "C" JNIEXPORT jstring JNICALL
Java_com_axo_transcribidor_NativeWhisper_nativeTranscribeChunk(
        JNIEnv *env, jobject /*this*/, jbyteArray audioData) {
    if (!g_ctx) {
        LOGE("Whisper context not initialized.");
        return env->NewStringUTF("Error: model not initialized");
    }

    jsize length = env->GetArrayLength(audioData);
    std::vector<float> pcmf32(length / 2); // assuming 16-bit mono PCM

    jbyte *audioBytes = env->GetByteArrayElements(audioData, nullptr);
    for (int i = 0; i < length / 2; i++) {
        short sample = ((short *)audioBytes)[i];
        pcmf32[i] = sample / 32768.0f;
    }
    env->ReleaseByteArrayElements(audioData, audioBytes, JNI_ABORT);

    whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.print_realtime = false;
    params.print_progress = false;
    params.single_segment = true;
    params.translate = false;
    params.no_context = true;

    std::lock_guard<std::mutex> lock(g_mutex);
    if (whisper_full(g_ctx, params, pcmf32.data(), pcmf32.size()) != 0) {
        LOGE("Whisper transcription failed.");
        return env->NewStringUTF("Transcription failed");
    }

    const int n_segments = whisper_full_n_segments(g_ctx);
    std::string result;
    for (int i = 0; i < n_segments; ++i) {
        result += whisper_full_get_segment_text(g_ctx, i);
    }

    LOGI("Transcription result: %s", result.c_str());
    return env->NewStringUTF(result.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_axo_transcribidor_NativeWhisper_releaseModel(JNIEnv *env, jobject /*this*/) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_ctx) {
        whisper_free(g_ctx);
        g_ctx = nullptr;
        LOGI("Whisper model released successfully.");
    }
}
