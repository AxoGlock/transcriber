// whisper_native.cpp
#include <jni.h>
#include <android/log.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <cstring>

#define LOG_TAG "NativeWhisper"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Whisper + GGML headers (adjust paths if needed)
#include "whisper.h"
#include "ggml/ggml.h"
#include "ggml/ggml-backend.h"   // ✅ Needed for ggml_backend_register_cpu()
#include "ggml-backend-impl.h"

// Global context
static whisper_context *g_ctx = nullptr;
static std::string g_language = "en";

// ----------------------
// Minimal stubs for missing symbols on Android (CPU-only build)
// ----------------------
extern "C" {

// Stub for CoreML hooks (they exist in upstream but not on Android)
int whisper_coreml_init(void* /*ctx*/) { return -1; }
void whisper_coreml_free(void* /*ctx*/) {}
int whisper_coreml_encode(void* /*ctx*/, void* /*state*/, int /*n_inputs*/, int /*n_outputs*/) { return -1; }

} // extern "C"

// ----------------------
// Native helpers
// ----------------------
static bool file_exists(const char* path) {
    struct stat st;
    return (stat(path, &st) == 0);
}

static void pcm16_to_float(const int16_t *in, size_t n, std::vector<float> &out) {
    out.resize(n);
    for (size_t i = 0; i < n; ++i) {
        out[i] = in[i] / 32768.0f;
    }
}

// ----------------------
// Simple native API (not JNI)
// ----------------------
extern "C" {

bool nativeInitModel(const char* modelPath, const char* language) {
    if (!modelPath) return false;

    if (!file_exists(modelPath)) {
        LOGE("Model file NOT found: %s", modelPath);
        return false;
    }

    if (g_ctx) {
        whisper_free(g_ctx);
        g_ctx = nullptr;
    }

    // ✅ Register CPU backend before any whisper_init call
#ifdef GGML_USE_CPU
#if defined(ggml_backend_register_cpu)
    ggml_backend_register_cpu();
#elif defined(GGML_BACKEND_REG_CPU)
    ggml_backend_register(GGML_BACKEND_REG_CPU());
#else
    if (ggml_backend_cpu_reg) {
        // Register manually (older GGML API)
        ggml_backend_reg_t reg = ggml_backend_cpu_reg();
        if (reg) {
            // nothing to do, registration happens automatically in whisper_init
        }
    }
#endif
#endif


    whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = false;

    g_ctx = whisper_init_from_file_with_params(modelPath, cparams);
    if (!g_ctx) {
        LOGE("whisper_init_from_file_with_params FAILED for %s", modelPath);
        return false;
    }

    if (language) g_language = language;

    LOGI("Model initialized (CPU-only): %s", modelPath);
    return true;
}

const char* nativeTranscribeBuffer(const int16_t* pcm16, int n_samples) {
    static std::string s_out;
    s_out.clear();

    if (!g_ctx || !pcm16 || n_samples <= 0) return s_out.c_str();

    std::vector<float> pcmf32;
    pcm16_to_float(pcm16, (size_t)n_samples, pcmf32);

    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.print_progress = false;
    wparams.print_realtime = false;
    wparams.translate = false;
    wparams.language = g_language.c_str();

    int rv = whisper_full(g_ctx, wparams, pcmf32.data(), (int)pcmf32.size());
    if (rv != 0) {
        LOGE("whisper_full returned %d", rv);
        return s_out.c_str();
    }

    const int n_segments = whisper_full_n_segments(g_ctx);
    for (int i = 0; i < n_segments; ++i) {
        const char* seg = whisper_full_get_segment_text(g_ctx, i);
        if (seg) s_out += seg;
    }

    return s_out.c_str();
}

} // extern "C"

// ----------------------
// JNI Wrappers
// ----------------------
extern "C" JNIEXPORT jboolean JNICALL
Java_com_axo_transcribidor_MainActivity_nativeInit(
        JNIEnv* env, jobject /*thiz*/, jstring modelPath, jstring language) {

    const char* model_path = env->GetStringUTFChars(modelPath, nullptr);
    const char* lang = env->GetStringUTFChars(language, nullptr);

    LOGI("Initializing whisper model from %s", model_path);

    if (g_ctx) {
        whisper_free(g_ctx);
        g_ctx = nullptr;
    }

    // ✅ Must call before whisper_init_from_file_with_params
#ifdef GGML_USE_CPU
#if defined(ggml_backend_register_cpu)
    ggml_backend_register_cpu();
#elif defined(GGML_BACKEND_REG_CPU)
    ggml_backend_register(GGML_BACKEND_REG_CPU());
#else
    if (ggml_backend_cpu_reg) {
        // Register manually (older GGML API)
        ggml_backend_reg_t reg = ggml_backend_cpu_reg();
        if (reg) {
            // nothing to do, registration happens automatically in whisper_init
        }
    }
#endif
#endif


    whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = false;

    g_ctx = whisper_init_from_file_with_params(model_path, cparams);
    if (!g_ctx) {
        LOGE("whisper_init_from_file_with_params FAILED for %s", model_path);
        env->ReleaseStringUTFChars(modelPath, model_path);
        env->ReleaseStringUTFChars(language, lang);
        return JNI_FALSE;
    }

    if (lang) {
        g_language = lang;
        LOGI("Language set to: %s", g_language.c_str());
    }

    env->ReleaseStringUTFChars(modelPath, model_path);
    env->ReleaseStringUTFChars(language, lang);
    LOGI("Whisper initialized successfully!");
    return JNI_TRUE;
}
extern "C" {
#include "ggml/ggml.h"
#include "ggml/ggml-backend.h"

// Prevent recursive static initialization
static bool g_cpu_backend_registered = false;

// Dummy backend registry structure (minimal)
static ggml_backend_reg_t g_dummy_backend_reg = nullptr;

// Safe no-op implementation to avoid __cxa_guard recursion
ggml_backend_reg_t ggml_backend_cpu_reg(void) {
    if (!g_cpu_backend_registered) {
        g_cpu_backend_registered = true;

        // Try to retrieve the built-in CPU backend (if compiled in)
        ggml_backend_reg_t reg = ggml_backend_reg_get("CPU");
        if (reg) {
            g_dummy_backend_reg = reg;
        } else {
            // Minimal fallback (avoid nullptr to prevent ggml_abort)
            static struct ggml_backend dummy_backend;
            g_dummy_backend_reg = reinterpret_cast<ggml_backend_reg_t>(&dummy_backend);
        }
    }

    return g_dummy_backend_reg;
}
}

