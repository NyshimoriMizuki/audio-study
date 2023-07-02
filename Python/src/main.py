import pyaudio
import wave
from os.path import exists

AUDIO = pyaudio.PyAudio()

stream = AUDIO.open(
    format=pyaudio.paInt16,  channels=1,
    rate=44100,
    input=True,
    frames_per_buffer=1024
)
frames = list()
to_save = input("save audio? [Y]/[N]\n ").upper()

try:
    while True:
        data = stream.read(1024)
        frames.append(data)
        print(f"data: {data}")

except KeyboardInterrupt:
    pass

stream.stop_stream()
stream.close()
AUDIO.terminate()

with wave.open('audio.wav', 'wb') as f:
    f.setnchannels(1)
    f.setsampwidth(AUDIO.get_sample_size(pyaudio.paInt16))
    f.setframerate(44100)
    f.writeframes(b''.join(frames))
