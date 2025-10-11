package com.axo.transcribidor


object NativeWhisper {
external fun nativeInit(modelPath: String, lang: String): Boolean
external fun nativeTranscribeChunk(pcm16: ByteArray): String
external fun nativeSetLanguage(lang: String)
external fun nativeShutdown()
}