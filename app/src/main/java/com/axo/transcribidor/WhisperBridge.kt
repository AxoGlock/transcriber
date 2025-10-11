object WhisperBridge {
    init {
        System.loadLibrary("native_whisper")
    }

    external fun transcribe(path: String, language: String): String
}
