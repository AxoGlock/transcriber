package com.axo.transcribidor


object NativeWhisper {
    init {
        System.loadLibrary("native_whisper")
    }

    external fun nativeTranscribeChunk(data: ByteArray): String
}
