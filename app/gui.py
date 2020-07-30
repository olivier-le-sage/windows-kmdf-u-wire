# Simple Qt5 GUI interface with some basic demo functions
# Author: Olivier Lesage

from ctypes import *
import ctypes.util
import sys
import os
from PyQt5.QtWidgets import QApplication, QLabel, QWidget, QPushButton
from PyQt5.QtWidgets import QColorDialog, QVBoxLayout, QDesktopWidget, QMessageBox
from PyQt5.QtGui  import QIcon, QPainter, QColor, QPen
from PyQt5.QtCore import Qt

class UWireColorPicker(QWidget):

    def __init__(self, clib_handle):
        super().__init__()
        self.title = 'Color Control'
        self.current_color = QColor(0,0,0)
        self.left = 10
        self.top = 10
        self.width = 320
        self.height = 200
        self.clib_handle = clib_handle
        self.initUI()

    def initUI(self):
        # Adjust window position and size
        self.setGeometry(self.left, self.top, self.width, self.height)
        center = QDesktopWidget().availableGeometry().center()
        self.move(center.x() - int(self.width/2), center.y() - int(self.height/2))

        # Populate UI contents & layout
        #self.setWindowIcon(QtGui.QIcon("icon.png"))

        color_button = QPushButton("Color", self)
        color_button.setToolTip("Opens color dialog")
        color_button.clicked.connect(self.open_color_picker)

        blink_button = QPushButton("Blink", self)
        blink_button.setToolTip("Blinks the LED for a few seconds")
        blink_button.clicked.connect(self.blink)

        fade_button = QPushButton("Fade", self)
        fade_button.setToolTip("Fades the LED for a few seconds")
        fade_button.clicked.connect(self.fade)

        p = self.palette()
        p.setColor(self.backgroundRole(), Qt.white)

        layout = QVBoxLayout()
        layout.addWidget(color_button)
        layout.addWidget(blink_button)
        layout.addWidget(fade_button)
        layout.addStretch(1)

        self.setPalette(p)
        self.setLayout(layout)
        self.setWindowTitle(self.title)

        self.show()

    def paintEvent(self, event):
        qp = QPainter(self)
        qp.setPen(self.current_color)
        qp.setBrush(self.current_color)
        qp.drawEllipse(int(self.width/2)-12, int(self.height/1.3)-12, 25, 25)

    def check_driver(self, status):
        #print("DEBUG: returned " + str(status))
        if status == 0:
            self.alert = QMessageBox()
            self.alert.setIcon(QMessageBox.Critical)
            self.alert.setText("Device not found!")
            self.alert.setWindowTitle("Error")
            self.alert.setDetailedText("See console for debug messages.")
            self.alert.setStandardButtons(QMessageBox.Ok)
            self.alert.buttonClicked.connect(exit)
            self.alert.show()

    def open_color_picker(self):
        color = QColorDialog.getColor()
        if color.isValid():
            self.current_color = color
            #self.paint_rect(color) ### <-- CHANGE COLOR HERE ###
            print("DEBUG: Color selected is ("+str(color.red())+","+str(color.green())+
                  ","+str(color.blue())+") in RGB")
            status = self.clib_handle.uwire_set_led_color(c_ubyte(color.red()),
                                                          c_ubyte(color.green()),
                                                          c_ubyte(color.blue()))
            self.check_driver(status)

    def blink(self):
         # blink for 5 seconds
        status = self.clib_handle.uwire_blink_led(c_ubyte(5),
                                                  c_ubyte(self.current_color.red()),
                                                  c_ubyte(self.current_color.green()),
                                                  c_ubyte(self.current_color.blue()))
        self.check_driver(status)

    def fade(self):
        # fade for 5 seconds
        status = self.clib_handle.uwire_fade_led(c_ubyte(5),
                                                 c_ubyte(self.current_color.red()),
                                                 c_ubyte(self.current_color.green()),
                                                 c_ubyte(self.current_color.blue()))
        self.check_driver(status)

if __name__ == '__main__':

    # Load shared object library containing C object code
    led_control_lib = CDLL(os.path.abspath("led_control_app.dll"))
    if led_control_lib.uwire_set_led_color == None:
        print("ERROR: Failed to load led_control_app code. Exiting.")
        exit()

    app = QApplication(sys.argv)
    ex = UWireColorPicker(led_control_lib)
    sys.exit(app.exec_())

window = QWidget()
color_picker = QColorDialog.getColor()
layout = QVBoxLayout()
layout.addWidget(QPushButton('Blink'))
layout.addWidget(QPushButton('Change Color'))
#label = QLabel('Color Control')
#label.show()
window.show()

# Start the app
app.exec_()
