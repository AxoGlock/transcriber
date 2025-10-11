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

// Global native and recording flags
private var whisperInitialized = false
private var recording = false

class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Ask microphone permission
        val permissionLauncher = registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { granted ->
            if (!granted) {
                // Optionally handle denial
            }
        }
        permissionLauncher.launch(Manifest.permission.RECORD_AUDIO)

        // Load native library
        System.loadLibrary("native_whisper")

        // Compose UI
        setContent {
            TranscriberApp(
                onStartRecording = { onResult ->
                    startRecording(onResult)
                }
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
                if (read > 0) {
                    val chunk = buffer.copyOf(read)
                    val result = NativeWhisper.nativeTranscribeChunk(chunk)
                    onResult(result + " ")
                }
            }
            recorder.stop()
            recorder.release()
        }
    }
}

@Composable
fun TranscriberApp(onStartRecording: ((String) -> Unit) -> Unit) {
    var text by remember { mutableStateOf(TextFieldValue("")) }
    var language by remember { mutableStateOf("en") }

    Scaffold(
        bottomBar = {
            Row(
                Modifier
                    .fillMaxWidth()
                    .padding(8.dp),
                horizontalArrangement = Arrangement.SpaceEvenly
            ) {
                Button(onClick = {
                    if (!whisperInitialized) {
                        val path = "/data/data/com.axo.transcribidor/files/models/ggml-base.bin"
                        whisperInitialized = NativeWhisper.nativeInit(path, language)
                    }
                    if (!recording) onStartRecording { result ->
                        text = TextFieldValue(text.text + result)
                    }
                    recording = true
                }) {
                    Text("Record")
                }

                Button(onClick = {
                    recording = false
                }) {
                    Text("Stop")
                }

                Button(onClick = {
                    language = if (language == "en") "es" else "en"
                    NativeWhisper.nativeSetLanguage(language)
                }) {
                    Text(language.uppercase())
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
