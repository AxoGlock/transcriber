// native_whisper_chunk.cpp
#include "native_whisper.h"
#include <string>
#include <mutex>
#include <vector>

static std::mutex g_mutex;
static std::string g_language = "en";

// Here you would store your Whisper context if you want streaming
static void* g_ctx = nullptr; // placeholder

bool nativeInit(const char* modelPath, const char* language) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_language = language ? language : "en";
    // TODO: load your Whisper model into g_ctx here
    return true;
}

void nativeSetLanguage(const char* language) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if(language) g_language = language;
}

const char* nativeTranscribeChunk(const uint8_t* audioChunk, int length) {
    // TODO: feed audioChunk into Whisper streaming API
    static std::string result = "[dummy transcription]";
    return result.c_str();
}
