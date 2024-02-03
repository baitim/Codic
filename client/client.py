import PyQt5
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtCore import QObject, pyqtSignal, QTimer
from client_ui import Ui_MainWindow
import os

import sys
import socket

class Window(QMainWindow, Ui_MainWindow):
	def __init__(self):
		super().__init__()
		self.setupUi(self)
		self.send.clicked.connect(self.send_message)

	def send_message(self):
		message = self.enter.toPlainText()
		self.sock.send(message.encode())

class Chat(QObject):
	messageReceived = pyqtSignal(str)

	def __init__(self, sock):
		super().__init__()
		self.sock = sock
		QTimer.singleShot(100, self.check_message)

	def check_message(self):
		try:
			message = self.sock.recv(2048).decode()
		except Exception:
			QTimer.singleShot(100, self.check_message)
			return
		if len(message) > 0:
			self.messageReceived.emit(message)
			QTimer.singleShot(100, self.check_message)

def application():

	host = os.environ["HOST"]
	port = os.environ["PORT"]
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((host, int(port)))
	sock.setblocking(False)
	activate_chat = Chat(sock)

	app = QApplication(sys.argv)

	window = Window()
	window.show()

	sys.exit(app.exec_())

if __name__ == "__main__":
    application()