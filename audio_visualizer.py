import struct
import sys
import wave

import pyqtgraph as pg
from PyQt6 import QtCore, QtWidgets

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, wf):
        super().__init__()

        # wave init
        self.wf = wf
        self.chunks = 1024
        self.channels = wf.getnchannels()
        
        # audio wave amplitude vs sample
        styles = {"color": "white", "font-size": "18px"}
        self.time_graph = pg.PlotWidget()
        self.setCentralWidget(self.time_graph)
        self.time_graph.setTitle("Audio Waveform", color="white", size="24pt")
        self.time_graph.setLabel("left", "Wave Amplitude", **styles)
        self.time_graph.setLabel("bottom", "Samples", **styles)
        self.time_graph.addLegend()
        self.time_graph.showGrid(x=True, y=True)
        self.time_graph.setYRange(-65536, 65536)
        self.time_graph.setXRange(0, self.channels*self.chunks)
        
        self.samples = list(range(self.channels*self.chunks)) # x-axis
        self.line = self.time_graph.plot(pen='c')
        
        # update timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.update_plot)
        self.timer.start()

    def update_plot(self):
        wf_data = self.wf.readframes(self.chunks)
        wf_data_short = struct.unpack(str(self.channels*self.chunks)+"h",
                                      wf_data)
        self.line.setData(self.samples, wf_data_short)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Plays a wave file.\n" \
        "Usage: %s filename.wav" % sys.argv[0])
        sys.exit(-1)
    wf = wave.open(sys.argv[1], 'rb')
    
    app = QtWidgets.QApplication([])
    main = MainWindow(wf)
    main.show()
    app.exec()
