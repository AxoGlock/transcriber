#pragma once
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// Native API functions
bool nativeInit(const char* modelPath, const char* language);
void nativeSetLanguage(const char* language);
const char* nativeTranscribeChunk(const uint8_t* buffer, int len);

// JNI wrappers
#include <jni.h>

JNIEXPORT jboolean JNICALL
Java_com_axo_transcribidor_MainActivity_nativeInit(JNIEnv* env, jobject thiz,
                                                   jstring jModelPath, jstring jLanguage);

JNIEXPORT void JNICALL
Java_com_axo_transcribidor_MainActivity_nativeSetLanguage(JNIEnv* env, jobject thiz,
                                                          jstring jLanguage);

JNIEXPORT jstring JNICALL
Java_com_axo_transcribidor_MainActivity_nativeTranscribeChunk(JNIEnv* env, jobject thiz,
                                                              jbyteArray buffer);

#ifdef __cplusplus
}
#endif
