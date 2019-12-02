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
BUFFER_SIZE = 32768
#BUFFER_SIZE = 4096
AMPLITUDE_LIM = 32768
WINDOW_SIZE = 128


PLOT_SIZE = 400
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


    def __init__(self, canvas, transform, axisBounds, numPoints = BUFFER_SIZE, axisOffset = None, axisScale = (1, 1), origin = False):
        
        offsetX = 0 if axisOffset is None else axisOffset[0]
        offsetY = 0 if axisOffset is None else axisOffset[1]
        scaleX = axisScale[0]
        scaleY = axisScale[1]
        self.xAxis = scene.visuals.Axis(pos = [(0 + offsetX, 0 + offsetY), (scaleX + offsetX, 0 + offsetY)], 
                                        domain = (axisBounds['xmin'], axisBounds['xmax']), anchors = ('center', 'middle'),
                                        parent = canvas, tick_color = 'k', text_color = 'k', axis_color = 'k',
                                        tick_direction = (0.0, -1.0), tick_label_margin = 20, major_tick_length = 15, 
                                        minor_tick_length = 5.0, axis_label = axisBounds['labelx'], axis_label_margin = 60)

        
        self.yAxis = scene.visuals.Axis(pos = [(0 + offsetX, 0 + offsetY), (0 + offsetX, scaleY + offsetY)], 
                                        domain = (axisBounds['ymin'], axisBounds['ymax']), anchors = ('center', 'bottom'),
                                        parent = canvas, tick_color = 'k', text_color = 'k', axis_color = 'k',
                                        tick_direction = (-1.0, 0.0), tick_label_margin = 35, major_tick_length = 15, 
                                        minor_tick_length = 5.0, axis_label = axisBounds['labely'], axis_label_margin = 80)
        
        defaultPos = numpy.array([(0.0, 0.0), (1.0, 0.0)])
        deltaX = 1.0 / float(numPoints)
        self.line = scene.visuals.ScrollingLines(n_lines = 1, line_size = numPoints, dx = deltaX,
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



windowSizeX = (3 * PLOT_SIZE) + (4 * MARGIN_X)
windowSizeY = (2 * PLOT_SIZE) + (3 * MARGIN_Y)
window = scene.canvas.SceneCanvas(  title = "Audio Visualizer", 
                                    keys = 'interactive',
                                    size = (windowSizeX, windowSizeY), 
                                    position = (0 + 1920 - windowSizeX / 2.0, 1080 - windowSizeY / 2.0), 
                                    resizable = False, bgcolor = "#eaebed")

waveform = Graph(   canvas = window.scene, 
                    origin = True,
                    axisOffset = (0, -1),
                    axisScale = (1, 2),
                    numPoints = BUFFER_SIZE,
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE * 0.5, 1), 
                                                                translate = (MARGIN_X, MARGIN_Y + (PLOT_SIZE / 2.0), 0)),
                    axisBounds = {  'xmin' : BUFFER_SIZE, 
                                    'xmax' : 0,
                                    'ymin' : -1.0,
                                    'ymax' : 1.0,
                                    'labelx' : 'samples',
                                    'labely' : 'amplitude'})


peak = Graph(  canvas = window.scene, 
                    numPoints = BUFFER_SIZE,
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE, 1), 
                                                                translate = ((2.0 * MARGIN_X) + PLOT_SIZE, MARGIN_Y + PLOT_SIZE, 0)),
                    axisBounds = {  'xmin' : BUFFER_SIZE, 
                                    'xmax' : 0,
                                    'ymin' : 0,
                                    'ymax' : 1.0,
                                    'labelx' : 'samples',
                                    'labely' : 'amplitude - peak (dB)'})

rms = Graph(  canvas = window.scene, 
                    numPoints = BUFFER_SIZE,
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE, 1), 
                                                                translate = ((3.0 * MARGIN_X) + (2.0 * PLOT_SIZE), MARGIN_Y + PLOT_SIZE, 0)),
                    axisBounds = {  'xmin' : BUFFER_SIZE, 
                                    'xmax' : 0,
                                    'ymin' : 0,
                                    'ymax' : 1.0,
                                    'labelx' : 'samples',
                                    'labely' : 'amplitude - rms (dB)'})

beats = Graph(      canvas = window.scene, 
                    numPoints = BUFFER_SIZE,
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE, 1), 
                                                                translate = (MARGIN_X, (2.0 * MARGIN_Y) + (2.0 * PLOT_SIZE), 0)),
                    axisBounds = {  'xmin' : BUFFER_SIZE, 
                                    'xmax' : 0,
                                    'ymin' : 0,
                                    'ymax' : 1.0,
                                    'labelx' : 'samples',
                                    'labely' : 'beat (binary)'})

frequency = Graph(  canvas = window.scene,
                    numPoints = 1024,
                    transform = visuals.transforms.STTransform( scale = (PLOT_SIZE, -PLOT_SIZE, 1), 
                                                                translate = ((2.0 * MARGIN_X) + PLOT_SIZE, (2.0 * MARGIN_Y) + (2.0 * PLOT_SIZE), 0)),
                    axisBounds = {  'xmin' : 20, 
                                    'xmax' : 20000,
                                    'ymin' : 0,
                                    'ymax' : 1.0,
                                    'labelx' : 'frequency (Hz)',
                                    'labely' : 'amplitude (dB)'})

lightColor = scene.visuals.Rectangle(   center = (PLOT_SIZE * 2.5 + MARGIN_X * 3, PLOT_SIZE * 1.5 + MARGIN_Y * 2), 
                                        color = '#a7b8d3', height = PLOT_SIZE, width = PLOT_SIZE, parent = window.scene)


#buffer = RingBuffer(capacity = BUFFER_SIZE, dtype = numpy.int16)
ledColor = color.Color()
ledColor.hsv = (0.0, 1.0, 1.0)
hue = 0.0
value = 1.0
saturation = 1.0
globalSpeed = 1.0


class SoundProcessor:

    def __init__(self):
        pass

    def calculateRMS(self, samples, start, length):
        averageSquared = 0
        for i in range(0, length):
            sampleSquared = pow(samples[start + i], 2.0)
            averageSquared += ((1.0 / length) * sampleSquared)
        
        rms = pow(averageSquared, 0.5)
        return numpy.full((1, length), rms)

    def calculatePeaks(self, samples, start, length):
        peak = 0
        for i in range(0, length):
            sample = abs(samples[start + i])
            peak = sample if sample > peak else peak

        return numpy.full((1, length), peak)

    def getRMS(self, samples, windowSize, bufferLen):
        return self.windowAction(samples, windowSize, bufferLen, self.calculateRMS)

    def getPeaks(self, samples, windowSize, bufferLen):
        return self.windowAction(samples, windowSize, bufferLen, self.calculatePeaks)

    def getAverage(self, samples, startIndex, length):
        averge = 0.0
        for i in range(0, length):
            averge += ((1.0 / length) * samples[startIndex + i])

        return averge

    def windowAction(self, samples, windowSize, bufferLen, function):
        totalResult = None
        remaining = bufferLen
        startIndex = 0

        while remaining > 0:
            window = min(windowSize, remaining)
            currentResult = function(samples, startIndex, window)

            if totalResult is None:
                totalResult = currentResult
            else:
                totalResult = numpy.concatenate([totalResult, currentResult], axis = 1)

            remaining -= window
            startIndex += window

        return totalResult

processor = SoundProcessor()

def update(ev):
    global globalSpeed

    # Read Input
    bytesAvailable = stream.get_read_available()
    if(bytesAvailable <= 0):
        return

    data = stream.read(bytesAvailable)
    samples = numpy.frombuffer(data, dtype = numpy.int16)
    normalized = numpy.multiply(samples, 1 / (1.0 * AMPLITUDE_LIM))

    # Process audio
    processingWindow = 256
    peaks = processor.getPeaks(normalized, processingWindow, bytesAvailable)
    rmsData = processor.getRMS(normalized, processingWindow, bytesAvailable)

    # Update Graphs
    waveform.rollData( numpy.array([normalized]) )
    peak.rollData( peaks )
    rms.rollData( rmsData )

    # Update lights
    averagePower = processor.getAverage(peaks[0], bytesAvailable - processingWindow - 1, processingWindow)
    
    h, s, v = ledColor.hsv
    #globalSpeed = 1.0 + (100.0 * averagePower)
    #saturation = 1.0 - pow(averagePower, 0.1)
    h = (h + (0.7 * globalSpeed)) % 360
    value = 1.0 - averagePower
    ledColor.hsv = (h, saturation, value)
    lightColor.color = ledColor

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