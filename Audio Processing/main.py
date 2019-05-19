import sys
import pyaudio
import time
import wave
import struct

import numpy
from vispy import plot, gloo, app, visuals, scene, color
from vispy.visuals.transforms import STTransform, NullTransform
from numpy_ringbuffer import RingBuffer

FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 48000
CHUNK = 32
#BUFFER_SIZE = 32768
BUFFER_SIZE = 4096
AMPLITUDE_LIM = 32768


PLOT_SIZE = 700
MARGIN_X = 125
MARGIN_Y = 125

stream = None
audioInterface = None

class AudioVisualizer():

    def __init__(self):
        self.timeData = []
        self.waveform = []


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
audioInterface = pyaudio.PyAudio()
 
# start Recording
print("Opening audio stream...")
stream = audioInterface.open(format = FORMAT, channels = CHANNELS,
                rate = RATE, input = True, output = False,
                frames_per_buffer = CHUNK)

print("Success, audio is now streaming.")
print("Showing plot...")


class Graph():


    def __init__(self, canvas, transform, axisBounds, offset = None, origin = False):
        
        offsetX = 0 if offset is None else offset[0]
        offsetY = 0 if offset is None else offset[1]
        self.xAxis = scene.visuals.Axis(pos = [(0 + offsetX, 0 + offsetY), (1 + offsetX, 0 + offsetY)], 
                                        domain = (axisBounds['xmin'], axisBounds['xmax']), anchors = ('center', 'middle'),
                                        parent = canvas, tick_color = 'k', text_color = 'k', axis_color = 'k',
                                        tick_direction = (0.0, -1.0), tick_label_margin = 20, major_tick_length = 15, 
                                        minor_tick_length = 5.0, axis_label = axisBounds['labelx'], axis_label_margin = 60)

        
        self.yAxis = scene.visuals.Axis(pos = [(0 + offsetX, 0 + offsetY), (0 + offsetX, 1 + offsetY)], 
                                        domain = (axisBounds['ymin'], axisBounds['ymax']), anchors = ('center', 'bottom'),
                                        parent = canvas, tick_color = 'k', text_color = 'k', axis_color = 'k',
                                        tick_direction = (-1.0, 0.0), tick_label_margin = 35, major_tick_length = 15, 
                                        minor_tick_length = 5.0, axis_label = axisBounds['labely'], axis_label_margin = 80)
        
        defaultPos = numpy.array([(0.0, 0.0), (1.0, 0.0)])
        deltaX = 1.0 / float(BUFFER_SIZE)
        self.line = scene.visuals.ScrollingLines(n_lines = 1, line_size = BUFFER_SIZE, dx = deltaX,
                                                columns = 1, cell_size = (1, 1), color = [0, 0, 0, 1], parent = canvas)
        
        subvisuals = [self.xAxis, self.yAxis, self.line]

        if(origin):            
            #self.grid = scene.visuals.GridLines(parent = canvas, color = 'w')
            linePositions = numpy.array([(0.0, 0.0), (1.0, 0.0)])
            self.origin = scene.visuals.Line(pos = linePositions, color = (0.6, 0, 0, 0.5), connect = 'strip', 
                                            method = 'gl', antialias = False, parent = canvas)
            subvisuals.append(self.origin)
        
        self.container = scene.visuals.Compound(subvisuals = subvisuals, parent = canvas)        
        self.container.transform = transform


    def rollData(self, data):
        self.line.roll_data(data)



windowSizeX = (2 * PLOT_SIZE) + (3 * MARGIN_X)
windowSizeY = (2 * PLOT_SIZE) + (3 * MARGIN_Y)
window = scene.canvas.SceneCanvas(  title = "Audio Visualizer", 
                                    keys = 'interactive',
                                    size = (windowSizeX, windowSizeY), 
                                    position = (3840 + 1920 - windowSizeX / 2.0, 1080 - windowSizeY / 2.0), 
                                    resizable = False, bgcolor = "#eaebed")

waveform = Graph(   canvas = window.scene, 
                    origin = True,
                    offset = (0, -0.5),
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE, 1), 
                                                                translate = (MARGIN_X, MARGIN_Y + (PLOT_SIZE / 2.0), 0)),
                    axisBounds = {  'xmin' : BUFFER_SIZE, 
                                    'xmax' : 0,
                                    'ymin' : -1.0,
                                    'ymax' : 1.0,
                                    'labelx' : 'samples',
                                    'labely' : 'amplitude'})


amplitude = Graph(  canvas = window.scene, 
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE, 1), 
                                                                translate = ((2.0 * MARGIN_X) + PLOT_SIZE, MARGIN_Y + PLOT_SIZE, 0)),
                    axisBounds = {  'xmin' : BUFFER_SIZE, 
                                    'xmax' : 0,
                                    'ymin' : 0,
                                    'ymax' : 1.0,
                                    'labelx' : 'samples',
                                    'labely' : 'amplitude - rms (dB)'})

frequency = Graph(  canvas = window.scene,
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE, 1), 
                                                                translate = (MARGIN_X, (2.0 * MARGIN_Y) + (2.0 * PLOT_SIZE), 0)),
                    axisBounds = {  'xmin' : 20, 
                                    'xmax' : 20000,
                                    'ymin' : 0,
                                    'ymax' : 1.0,
                                    'labelx' : 'frequency (Hz)',
                                    'labely' : 'amplitude (dB)'})

lightColor = scene.visuals.Rectangle(   center = (PLOT_SIZE * 1.5 + MARGIN_X * 2, PLOT_SIZE * 1.5 + MARGIN_Y * 2), 
                                        color = '#a7b8d3', height = PLOT_SIZE, width = PLOT_SIZE, parent = window.scene)


#buffer = RingBuffer(capacity = BUFFER_SIZE, dtype = numpy.int16)

def update(ev):
    bytesAvailable = stream.get_read_available()
    if(bytesAvailable <= 0):
        return

    data = stream.read(bytesAvailable)
    sample = numpy.frombuffer(data, dtype = numpy.int16)

    waveData = numpy.multiply(sample, 1 / (2.0 * AMPLITUDE_LIM))
    waveform.rollData(numpy.array([waveData]))
    #buffer.extend(sample)"""


timer = app.Timer(interval = 0.001, connect = update)
timer.start()





window.show(run = True)






print("Closing stream...")
 
# stop Recording
stream.stop_stream()
stream.close()
audioInterface.terminate()

print("Done. Exiting")