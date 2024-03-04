# audio visualizer + playback
# only tested with mono audio

import struct
import sys
import wave
import numpy as np
import pyaudio
import pyqtgraph as pg
import timeit

from PyQt6 import QtCore, QtWidgets
from scipy.fft import fft, fftfreq, rfft, rfftfreq

class MainWindow(QtWidgets.QWidget):
    def __init__(self, wf):
        super().__init__()
        
        self.setWindowTitle('Audio Time Frequency Graph')

        self.y_range = 64
        
        # waveform
        self.wf = wave.open(sys.argv[1], 'rb')
        self.chunks = 512
        self.samp_width = self.wf.getsampwidth()
        self.channels = self.wf.getnchannels()
        self.sample_rate = self.wf.getframerate()

        # stream to play wav file
        p = pyaudio.PyAudio()
        self.stream = p.open(format = p.get_format_from_width(self.samp_width),
                             channels = self.channels,
                             rate = self.sample_rate,
                             output = True)
        
        # create graphs
        styles = {"color": "white", "font-size": "12px"}
        
        self.time_graph = pg.PlotWidget()
        self.time_graph.setTitle("Audio Time Domain", color="white", size="12pt")
        self.time_graph.setLabel("left", "Wave Amplitude", **styles)
        self.time_graph.setLabel("bottom", "Samples", **styles)
        self.time_graph.showGrid(x=True, y=True)
        self.time_graph.setXRange(0, self.channels*self.chunks)
        self.time_graph.setYRange(-65536, 65536)
        self.time_samples = list(range(self.channels*self.chunks)) # x-axis (time)
        self.time_line = self.time_graph.plot(pen='c')
        
        self.freq_graph = pg.PlotWidget()
        self.freq_graph.setTitle("Audio Frequency Domain", color="white", size="12pt")
        self.freq_graph.setLabel("left", "Amplitude", **styles)
        self.freq_graph.setLabel("bottom", "Frequency", **styles)
        self.freq_graph.showGrid(x=True, y=True)
        self.freq_graph.setXRange(0, self.sample_rate/2)
        self.freq_graph.setYRange(0, self.y_range)
        # self.freq_graph.enableAutoRange(y=True)
        # self.freq_graph.setAutoVisible(y=True)
        self.freq_samples = np.linspace(0, self.sample_rate, self.channels*self.chunks)[0:int(((self.channels*self.chunks)/2)+1)] # x-axis (freq)
        self.freq_line = self.freq_graph.plot(pen='c')
        
        # create a layout
        layout = QtWidgets.QVBoxLayout()
        self.setLayout(layout)
        
        layout.addWidget(self.time_graph)
        layout.addWidget(self.freq_graph)

        # create timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(0) # don't wait to update, keep playing
        self.timer.timeout.connect(self.update_plot)
        self.timer.start() 
        
        # show the window
        self.show()

    def update_plot(self):
        wf_data = self.wf.readframes(self.chunks) # underrun may occur. can solve w/ threading or larger chunk size
        
        self.stream.write(wf_data)
        try:
            wf_data_short = struct.unpack(str(self.channels*self.chunks)+"h", wf_data)
        except:
            self.wf.rewind()
            #self.timer.stop()
            return

        fft_wf_data_short = np.abs(rfft(wf_data_short)) # calc magnitude
        fft_wf_data_short = fft_wf_data_short*2/(32767*self.chunks) # rescaling between 0-1
        fft_wf_data_short*= self.y_range

        self.time_line.setData(self.time_samples, wf_data_short) # y-axis (time)
        self.freq_line.setData(self.freq_samples, fft_wf_data_short) # y-axis (freq)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Plays a wave file.\n" \
        "Usage: %s filename.wav" % sys.argv[0])
        sys.exit(-1)
        
    
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow(sys.argv[1])
    #window.update_plot()
    sys.exit(app.exec())
