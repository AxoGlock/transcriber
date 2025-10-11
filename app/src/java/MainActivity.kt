package com.axo.transcribidor
permissionLauncher.launch(Manifest.permission.RECORD_AUDIO)


System.loadLibrary("native_whisper")


setContent {
var text by remember { mutableStateOf(TextFieldValue("")) }
var language by remember { mutableStateOf("en") }


Scaffold(bottomBar = {
Row(
Modifier
.fillMaxWidth()
.padding(8.dp),
horizontalArrangement = Arrangement.SpaceEvenly
) {
Button(onClick = {
if (!whisperInitialized) {
val path = filesDir.absolutePath + "/models/ggml-base.bin"
whisperInitialized = NativeWhisper.nativeInit(path, language)
}
if (!recording) startRecording { result ->
text = TextFieldValue(text.text + result)
}
recording = true
}) { Text("Record") }


Button(onClick = {
recording = false
}) { Text("Stop") }


Button(onClick = {
language = if (language == "en") "es" else "en"
NativeWhisper.nativeSetLanguage(language)
}) { Text(language.uppercase()) }
}
}) { padding ->
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