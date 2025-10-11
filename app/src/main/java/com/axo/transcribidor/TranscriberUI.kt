package com.axo.transcribidor

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
fun TranscriberUI(
    tempAudioFile: File,
    transcribedText: MutableState<String>,
    onRecordStart: () -> Unit,
    onRecordStop: (language: String) -> Unit
) {
    var selectedLanguage by remember { mutableStateOf("English") }

    Column(modifier = Modifier.fillMaxSize().padding(16.dp)) {
        // Language display and button
        Row(verticalAlignment = androidx.compose.ui.Alignment.CenterVertically) {
            Text("Language: $selectedLanguage", style = MaterialTheme.typography.titleMedium)
            Spacer(modifier = Modifier.width(16.dp))
            Button(onClick = {
                selectedLanguage = if (selectedLanguage == "English") "Spanish" else "English"
            }) {
                Text("Switch Language")
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Editable transcription box
        BasicTextField(
            value = transcribedText.value,
            onValueChange = { transcribedText.value = it },
            modifier = Modifier
                .fillMaxWidth()
                .height(300.dp)
                .padding(8.dp)
                .border(1.dp, Color.Gray)
                .padding(8.dp)
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Record buttons
        Row {
            Button(onClick = { onRecordStart() }, modifier = Modifier.weight(1f)) {
                Text("Start Recording")
            }
            Spacer(modifier = Modifier.width(16.dp))
            Button(onClick = { onRecordStop(selectedLanguage) }, modifier = Modifier.weight(1f)) {
                Text("Stop & Transcribe")
            }
        }
    }
}
