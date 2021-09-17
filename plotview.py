# This Python file uses the following encoding: utf-8
from PyQt5 import QtCore
from PyQt5 import QtWidgets

import pandas as pd
from plotly.offline import download_plotlyjs, init_notebook_mode, plot, iplot

import cufflinks as cf
   
class plotview(QtWidgets.QWidget):
    
    graphTimeData = []
    graphActivityData = []
    timeUnits = ""
    
    def __init__(self,parent=None):
        super().__init__(parent)
        data = open("dots.txt")
        for entry in data.readlines():
            abcd = entry.split()
            self.graphTimeData.append(float(abcd[0]))
            self.graphActivityData.append(int(abcd[1]))
        #self.mainWidget = QtWidgets.QWidget()

    def show_graph(self):
        cf.set_config_file(sharing='public',theme='pearl',offline=False)
        cf.go_offline
        init_notebook_mode(connected = True)
        df = pd.DataFrame({'Activity':self.graphActivityData},index=self.graphTimeData)
        #df.head()
        print(df)
        print("bfr")
        df.plot()
        print("afr")


if __name__ == "__main__":
 
    # df.iplot(kind='scatter',xTitle='Time',yTitle='Activity',title='Graph')
    
    app = QtWidgets.QApplication([])
    pv = plotview()
    #pv.show()
    pv.show_graph()
    app.exec()
    