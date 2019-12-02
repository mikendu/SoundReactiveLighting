import sys
import pyaudio
import time
import wave
import struct

import numpy
from vispy import gloo, app, visuals

FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 48000
CHUNK = 256
BUFFER_SIZE = 4096
AMPLITUDE_LIM = 32768

stream = None
audioInterface = None
buffer = []


class AudioVisualizer():

    def __init__(self):
        self.timeData = []
        self.waveform = []




def recordWaveFile(audioInterface, stream, fileName = "test.wav"):
    
    print("saving file....")
    frames = []
    
    for i in range(0, int(RATE / CHUNK * 2)):
        data = stream.read(CHUNK, exception_on_overflow = False)
        frames.append(data)

    print("finished recording")
    waveFile = wave.open(fileName, 'wb')
    waveFile.setnchannels(CHANNELS)
    waveFile.setsampwidth(audioInterface.get_sample_size(FORMAT))
    waveFile.setframerate(RATE)
    waveFile.writeframes(b''.join(frames))
    waveFile.close()



# Program execution
audioInterface = pyaudio.PyAudio()
 
# start Recording
print("Opening audio stream...")
stream = audioInterface.open(format=FORMAT, channels=CHANNELS,
                        rate=RATE, input=True,
                        frames_per_buffer=CHUNK)


print("Success, audio is now streaming.")
print("Showing plot...")


figure = plot.Fig(bgcolor = 'w', size = (1920, 1080), show = True)
plotWidget = figure[0, 0]

# Plot the target square wave shape
xValues = []
yValues = []

for i in range(0, 1000):
    xVal = float(i) / 1000.0
    yVal = xVal * xVal
    xValues.append(xVal)
    yValues.append(yVal)


lineColor = (0.0, 0.3, 1.0, 1)
line = plotWidget.plot((xValues, yValues), width = 15.0, color = lineColor,
                      title = 'Audio Visualizer', xlabel = 'samples',
                      ylabel = 'amplitude')


mainLayout = plotWidget.view.add_grid(margin = 1)
legendWidget = plot.Widget( bgcolor = (1, 1, 1, 0.6), border_color = 'k')
legend = mainLayout.add_widget(legendWidget, row = 0, col = 0)
legend.width_max = 200
legend.height_max = 120


label = plot.Label('Waveform', color = lineColor, anchor_x = 'center')
legendLayout = legendWidget.add_grid()
placedLabel = legendLayout.add_widget(label, row = 0, col = 0)


grid = plot.visuals.GridLines(color=(0, 0, 0, 0.5))
grid.set_gl_state('translucent')
plotWidget.view.add(grid)



def update(ev):
    pass
    #line.roll_data(1)
    #for i in range(0, len(yValues)):
        #yValues[i] = 0
        #line.set_data((xValues, yValues))
        

timer = app.Timer(interval = 'auto', connect = update)
timer.start()

#figure.show(run = True)

print("Closing stream...")
 
# stop Recording
stream.stop_stream()
stream.close()
audioInterface.terminate()

print("Done. Exiting")