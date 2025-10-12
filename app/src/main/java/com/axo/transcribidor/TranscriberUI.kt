package com.axo.transcribidor

import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import java.io.File

@Composable
fun TranscriberApp(
    onStartRecording: ((String) -> Unit) -> Unit,
    onInitWhisper: () -> String,
    onInitNative: (String, String) -> Boolean,
    onToggleLang: (String) -> Unit
) {
    var text by remember { mutableStateOf(TextFieldValue("")) }
    var whisperInitialized by remember { mutableStateOf(false) }
    var recording by remember { mutableStateOf(false) }
    var language by remember { mutableStateOf("en") }

    Scaffold(
        bottomBar = {
            Row(
                Modifier.fillMaxWidth().padding(8.dp),
                horizontalArrangement = Arrangement.SpaceEvenly
            ) {
                Button(onClick = {
                    if (!whisperInitialized) {
                        val modelPath = onInitWhisper()
                        whisperInitialized = onInitNative(modelPath, language)
                    }
                    if (!recording) onStartRecording { result ->
                        text = TextFieldValue(text.text + result)
                    }
                    recording = true
                }) { Text("Record") }

                Button(onClick = { recording = false }) { Text("Stop") }

                Button(onClick = {
                    language = if (language == "en") "es" else "en"
                    onToggleLang(language)
                }) { Text(language.uppercase()) }
            }
        }
    ) { padding ->
        TextField(
            value = text,
            onValueChange = { text = it },
            modifier = Modifier.padding(padding).fillMaxSize()
        )
    }
}

