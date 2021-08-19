# This Python file uses the following encoding: utf-8
from PyQt5 import QtCore
from PyQt5 import QtWidgets
import cufflinks as cf


class plotview(QtWidgets.QWidget):

    graphData = []

    def __init__(self):
        pass

if __name__ == "__main__":

    cf.datagen.lines().iplot(kind='scatter',xTitle='Dates',yTitle='Returns',title='Cufflinks - Line Chart')
