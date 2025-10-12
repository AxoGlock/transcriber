#include "native_whisper.h"
#include "whisper.h"
#include <android/log.h>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <sys/stat.h>
#define LOG_TAG "NativeWhisper"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Static context
static whisper_context* g_ctx = nullptr;
static std::string g_language = "en";

extern "C" {

// Native implementations


bool nativeInit(const char* modelPath, const char* language) {
    // Free previous context if exists
    if (g_ctx) {
        whisper_free(g_ctx);
        g_ctx = nullptr;
    }

    // Check if file exists
    struct stat st;
    if (stat(modelPath, &st) != 0) {
        LOGE("Model file NOT found: %s", modelPath);
        return false;
    }
    LOGI("Model file size: %ld bytes", st.st_size);

    // Initialize Whisper context safely
    g_ctx = whisper_init_from_file(modelPath);
    if (!g_ctx) {
        LOGE("Failed to initialize Whisper model at %s", modelPath);
        return false;
    }

    // Optional: set language
    if (language) {
        g_language = language;
        LOGI("Language set to: %s", language);
    }

    LOGI("Whisper model initialized successfully!");
    return true;
}


// Simple chunk transcription
const char* nativeTranscribeChunk(const uint8_t* buffer, int len) {
    static std::string result;

    if (!g_ctx || !buffer || len <= 0) return "";

    // Convert PCM16 to float
    std::vector<float> pcmf32(len);
    for (int i = 0; i < len; ++i) {
        pcmf32[i] = buffer[i] / 32768.0f;
    }

    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.translate = false;
    wparams.language = g_language.c_str();
    wparams.print_progress = false;

    int ret = whisper_full(g_ctx, wparams, pcmf32.data(), pcmf32.size());
    if (ret != 0) {
        LOGE("whisper_full failed with code %d", ret);
        return "";
    }

    result.clear();
    int n_segments = whisper_full_n_segments(g_ctx);
    for (int i = 0; i < n_segments; ++i) {
        result += whisper_full_get_segment_text(g_ctx, i);
    }

    return result.c_str();
}

// JNI wrappers
JNIEXPORT jboolean JNICALL
Java_com_axo_transcribidor_MainActivity_nativeInit(JNIEnv* env, jobject thiz,
                                                   jstring jModelPath, jstring jLanguage) {
    const char* modelPath = env->GetStringUTFChars(jModelPath, nullptr);
    const char* language = jLanguage ? env->GetStringUTFChars(jLanguage, nullptr) : nullptr;
    bool ok = nativeInit(modelPath, language);
    env->ReleaseStringUTFChars(jModelPath, modelPath);
    if (jLanguage) env->ReleaseStringUTFChars(jLanguage, language);
    return ok;
}

JNIEXPORT void JNICALL
Java_com_axo_transcribidor_MainActivity_nativeSetLanguage(
    JNIEnv* env, jobject /*thiz*/, jstring language) 
{
    const char* langCStr = env->GetStringUTFChars(language, nullptr);
    
    // Store the language somewhere global, e.g. g_language
    g_language = langCStr;

    env->ReleaseStringUTFChars(language, langCStr);
}

JNIEXPORT jstring JNICALL
Java_com_axo_transcribidor_MainActivity_nativeTranscribeChunk(JNIEnv* env, jobject thiz,
                                                              jbyteArray buffer) {
    jsize len = env->GetArrayLength(buffer);
    std::vector<uint8_t> buf(len);
    env->GetByteArrayRegion(buffer, 0, len, reinterpret_cast<jbyte*>(buf.data()));

    const char* result = nativeTranscribeChunk(buf.data(), len);
    return env->NewStringUTF(result);
}

} // extern "C"
