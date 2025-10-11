package com.axo.transcribidor

object NativeWhisper {
    init {
        System.loadLibrary("native_whisper")
    }

    external fun initModel(modelPath: String): Boolean
    external fun nativeTranscribeChunk(audioData: ByteArray): String
    external fun releaseModel()
}
