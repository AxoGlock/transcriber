// native_whisper.cpp
#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <cstring>
#include "whisper.h"

// NOTE: match your package/class name used from Kotlin.
// This file exposes functions for Java_com_axo_transcribidor_NativeWhisper_*

static whisper_context *g_ctx = nullptr;

// Simple WAV loader: expects 16-bit PCM WAV (mono or stereo). Converts to float ([-1..1]) mono by averaging channels
static bool load_wav_pcm16_mono(const char *path, std::vector<float> &out_samples, int &sample_rate) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;

    // Read header
    char riff[4];
    f.read(riff, 4);
    if (std::strncmp(riff, "RIFF", 4) != 0) return false;

    f.seekg(8);
    char wave[4];
    f.read(wave,4);
    if (std::strncmp(wave, "WAVE", 4) != 0) return false;

    // Search chunks
    int16_t audio_format = 0;
    int16_t num_channels = 0;
    int32_t byte_rate = 0;
    int16_t block_align = 0;
    int16_t bits_per_sample = 0;
    uint32_t data_chunk_pos = 0;
    uint32_t data_chunk_size = 0;

    while (f.good()) {
        char chunk_id[4];
        uint32_t chunk_size = 0;
        f.read(chunk_id, 4);
        if (!f.read(reinterpret_cast<char*>(&chunk_size), 4)) break;

        if (std::strncmp(chunk_id, "fmt ", 4) == 0) {
            // fmt chunk
            f.read(reinterpret_cast<char*>(&audio_format), sizeof(audio_format));
            f.read(reinterpret_cast<char*>(&num_channels), sizeof(num_channels));
            f.read(reinterpret_cast<char*>(&sample_rate), sizeof(sample_rate));
            f.read(reinterpret_cast<char*>(&byte_rate), sizeof(byte_rate));
            f.read(reinterpret_cast<char*>(&block_align), sizeof(block_align));
            f.read(reinterpret_cast<char*>(&bits_per_sample), sizeof(bits_per_sample));
            // skip any extra fmt bytes
            if (chunk_size > 16) f.seekg(chunk_size - 16, std::ios::cur);
        } else if (std::strncmp(chunk_id, "data", 4) == 0) {
            data_chunk_pos = (uint32_t)f.tellg();
            data_chunk_size = chunk_size;
            f.seekg(chunk_size, std::ios::cur);
        } else {
            // skip other chunks
            f.seekg(chunk_size, std::ios::cur);
        }
    }

    if (data_chunk_pos == 0 || bits_per_sample != 16) return false; // only support PCM16

    // read samples
    f.clear();
    f.seekg(data_chunk_pos);
    const size_t n_samples = data_chunk_size / (bits_per_sample/8);
    std::vector<int16_t> buf(n_samples);
    f.read(reinterpret_cast<char*>(buf.data()), data_chunk_size);
    if (!f) return false;

    out_samples.resize(n_samples / std::max(1, (int)num_channels));
    // Convert to floats and downmix to mono if needed
    if (num_channels == 1) {
        for (size_t i = 0; i < out_samples.size(); ++i) {
            out_samples[i] = buf[i] / 32768.0f;
        }
    } else {
        // average channels
        for (size_t i = 0, j = 0; j < out_samples.size(); ++j) {
            int32_t sum = 0;
            for (int c = 0; c < num_channels; ++c, ++i) sum += buf[i];
            out_samples[j] = (sum / (float)num_channels) / 32768.0f;
        }
    }

    return true;
}

// JNI: boolean nativeInit(String modelPath, String language)
extern "C" JNIEXPORT jboolean JNICALL
Java_com_axo_transcribidor_NativeWhisper_nativeInit(JNIEnv *env, jobject /*thiz*/, jstring modelPath, jstring /*lang*/) {
    if (g_ctx) {
        // already initialized
        return JNI_TRUE;
    }

    const char *cpath = env->GetStringUTFChars(modelPath, nullptr);
    if (!cpath) return JNI_FALSE;

    // Use default params
    whisper_context_params params = whisper_context_default_params();
    // Use the file-based initializer with params if available
    g_ctx = whisper_init_from_file_with_params(cpath, params);
    env->ReleaseStringUTFChars(modelPath, cpath);

    if (!g_ctx) return JNI_FALSE;

    return JNI_TRUE;
}

// JNI: String nativeTranscribe(String audioPath, String language)
extern "C" JNIEXPORT jstring JNICALL
Java_com_axo_transcribidor_NativeWhisper_nativeTranscribe(JNIEnv *env, jobject /*thiz*/, jstring audioPath, jstring language) {
    if (!g_ctx) {
        const char *err = "Model not loaded";
        return env->NewStringUTF(err);
    }
    const char *cpath = env->GetStringUTFChars(audioPath, nullptr);
    const char *clang = env->GetStringUTFChars(language, nullptr);


    std::vector<float> samples;
    int sr = 0;
    bool ok = load_wav_pcm16_mono(cpath, samples, sr);

    if (!ok) {
        env->ReleaseStringUTFChars(audioPath, cpath);
        env->ReleaseStringUTFChars(language, clang);
        const char *err = "Failed to load WAV (expect PCM16 WAV)";
        return env->NewStringUTF(err);
    }

    // Setup whisper params
    struct whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    // set language if user selected a language code (e.g., "es" or "en")
    if (clang && std::strlen(clang) > 0) {
        if (clang && std::strlen(clang) > 0) {
            wparams.language = clang;   // assign the language code string
            wparams.translate = false;
        }

    }

    // run full transcription (blocking)
    int ret = whisper_full(g_ctx, wparams, samples.data(), (int)samples.size());
    std::string out_text;
    if (ret != 0) {
        out_text = "whisper_full() failed";
    } else {
        // concatenate segments
        const int n_segments = whisper_full_n_segments(g_ctx);
        for (int i = 0; i < n_segments; ++i) {
            const char *seg = whisper_full_get_segment_text(g_ctx, i);
            if (seg) {
                out_text += seg;
                if (i + 1 < n_segments) out_text += " ";
            }
        }
    }

    env->ReleaseStringUTFChars(audioPath, cpath);
    env->ReleaseStringUTFChars(language, clang);

    return env->NewStringUTF(out_text.c_str());
}

// JNI: void nativeShutdown()
extern "C" JNIEXPORT void JNICALL
Java_com_axo_transcribidor_NativeWhisper_nativeShutdown(JNIEnv *env, jobject /*thiz*/) {
    if (g_ctx) {
        whisper_free(g_ctx);
        g_ctx = nullptr;
    }
}
