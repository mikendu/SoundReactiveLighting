import sys
import pyaudio
import time
import wave
import struct

import numpy
import matplotlib.pyplot as plot
import matplotlib.animation as animation
from matplotlib.lines import Line2D
from numpy_ringbuffer import RingBuffer

FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 48000
CHUNK = 32
BUFFER_SIZE = 32768
AMPLITUDE_LIM = 32768

buffer = RingBuffer(capacity = BUFFER_SIZE, dtype = numpy.int16)

class AudioVisualizer(object):
    def __init__(self, axes):
        
        self.axes = axes
        self.waveform = Line2D([], [], color = 'b', lw = 0.5, label = "Waveform")

        self.axes.add_line(self.waveform)
        self.axes.set_ylim(-AMPLITUDE_LIM, AMPLITUDE_LIM)
        self.axes.set_xlim(0, BUFFER_SIZE)

        self.timeData = []

    def update(self, sampleChunks):
        
        # Draw a fresh set of audio data
        # self.axes.figure.canvas.draw()
        timeData = []
        waveformData = []
        
        for i in range(0, len(sampleChunks)):
            timeData.append(i)
            waveformData.append(sampleChunks[i])

        self.waveform.set_data(timeData, waveformData)
        return self.waveform,

    def terminate(self):
        print("TERMINATION")


def recordWaveFile(pyAudio, stream, fileName = "test.wav"):
    
    print("saving file....")
    frames = []
    
    for i in range(0, 10):
        
        bytesAvailable = stream.get_read_available()
        data = stream.read(bytesAvailable)
        frames.append(data)

    print("finished recording")
    waveFile = wave.open(fileName, 'wb')
    waveFile.setnchannels(CHANNELS)
    waveFile.setsampwidth(pyAudio.get_sample_size(FORMAT))
    waveFile.setframerate(RATE)
    waveFile.writeframes(b''.join(frames))
    waveFile.close()



# Program execution
pyAudio = pyaudio.PyAudio()
 
# start Recording
print("Opening audio stream...")
stream = pyAudio.open(format = FORMAT, channels = CHANNELS,
                rate = RATE, input = True, output = False,
                frames_per_buffer = CHUNK)

def visualizeAudio():

    while True:

        bytesAvailable = stream.get_read_available()
        data = stream.read(bytesAvailable)
        sample = numpy.frombuffer(data, dtype = numpy.int16)
        buffer.extend(sample)

        yield buffer

print("Success, audio is now streaming.")
print("Showing plot...")

figure, axes = plot.subplots()
audio = AudioVisualizer(axes)

# Plot info and legend
figure.set_size_inches(11, 9)
figure.suptitle("Audio Visualizer")
figure.legend(loc = "upper left")
plot.xlabel("samples")
plot.ylabel("amplitude")

# pass a generator in "emitter" to produce data for the update func
anim = animation.FuncAnimation(figure, audio.update, visualizeAudio, interval= 1, blit = True)
plot.show()


print("Closing stream...")
 
# stop Recording
stream.stop_stream()
stream.close()
audio.terminate()

print("Done. Exiting")