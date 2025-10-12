package com.axo.transcribidor

import android.Manifest
import android.media.*
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.ui.unit.dp
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.*
import java.io.File
import java.io.FileOutputStream

class MainActivity : ComponentActivity() {

    companion object {
        init {
            System.loadLibrary("native_whisper")
        }
    }

    private var whisperInitialized = false
    private var recording = false
    private var language = "en"

    // JNI methods
    external fun nativeInit(modelPath: String, language: String): Boolean
    external fun nativeSetLanguage(language: String)
    external fun nativeTranscribeChunk(audioChunk: ByteArray): String

private fun prepareModel(): String {
    val modelDir = File(filesDir, "models")
    if (!modelDir.exists()) modelDir.mkdirs()

    val modelFile = File(modelDir, "ggml-tiny.bin")  // <- change here

    if (!modelFile.exists()) {
        assets.open("models/ggml-tiny.bin").use { input ->  // <- change here
            FileOutputStream(modelFile).use { output ->
                input.copyTo(output)
            }
        }
    }

    return modelFile.absolutePath
}


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Request microphone permission
        val permissionLauncher = registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { granted ->
            if (!granted) {
                // Optionally show warning dialog
            }
        }
        permissionLauncher.launch(Manifest.permission.RECORD_AUDIO)

        setContent {
            TranscriberApp(
                        onStartRecording = { onResult -> startRecording(onResult) },
                        onInitWhisper = { prepareModel() },
                        onInitNative = { path, lang -> nativeInit(path, lang) },
                        onToggleLang = { lang -> nativeSetLanguage(lang) }
            )
        }
    }

    private fun startRecording(onResult: (String) -> Unit) {
        lifecycleScope.launch(Dispatchers.IO) {
            val bufferSize = AudioRecord.getMinBufferSize(
                16000,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT
            )

            val recorder = AudioRecord(
                MediaRecorder.AudioSource.MIC,
                16000,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT,
                bufferSize
            )

            val buffer = ByteArray(bufferSize)
            recorder.startRecording()
            recording = true

            while (recording) {
                val read = recorder.read(buffer, 0, buffer.size)
                if (read > 0 && whisperInitialized) {
                    val chunk = buffer.copyOf(read)
                    val result = nativeTranscribeChunk(chunk)
                    if (result.isNotBlank()) {
                        onResult(result + " ")
                    }
                }
            }

            recorder.stop()
            recorder.release()
        }
    }

    @Composable
    fun TranscriberApp() {
        var text by remember { mutableStateOf(TextFieldValue("")) }
        var currentLanguage by remember { mutableStateOf(language) }

        Scaffold(
            bottomBar = {
                Row(
                    Modifier
                        .fillMaxWidth()
                        .padding(8.dp),
                    horizontalArrangement = Arrangement.SpaceEvenly
                ) {
                    // 🎙 Record button
                    Button(onClick = {
                                 if (!whisperInitialized) {
                                        val modelPath = "/data/data/com.axo.transcribidor/files/models/ggml-tiny.bin"
                                        whisperInitialized = nativeInit(modelPath, currentLanguage)
                                 }
                                 if (!recording) {
                                         startRecording { result ->
                                         text = TextFieldValue(text.text + result)
                                 }
                             }
                                recording = true
                             }) {
                                Text("Record")
                        }

                    // ⏹ Stop button
                    Button(onClick = { recording = false }) {
                        Text("Stop")
                    }

                    // 🌍 Language toggle
                    Button(onClick = {
                        currentLanguage = if (currentLanguage == "en") "es" else "en"
                        language = currentLanguage
                        if (whisperInitialized) {
                            nativeSetLanguage(currentLanguage)
                        }
                    }) {
                        Text(currentLanguage.uppercase())
                    }
                }
            }
        ) { padding ->
            TextField(
                value = text,
                onValueChange = { text = it },
                modifier = Modifier
                    .padding(padding)
                    .fillMaxSize()
            )
        }
    }
}
