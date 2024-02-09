import PyQt5
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtCore import QObject, pyqtSignal, QTimer
from client_ui import Ui_MainWindow
import os

import sys
import socket

class Window(QMainWindow, Ui_MainWindow):
	def __init__(self, sock):
		super().__init__()
		self.sock = sock
		self.setupUi(self)
		self.send_connect_message()
		self.send.clicked.connect(self.send_message)

	def show_message(self, message):
		if message[:4] == "TIME":
			self.result.setText(message)

	def send_connect_message(self):
		message = "CONNECT"
		self.sock.send(message.encode())

	def send_message(self):
		message = "PROGRAM 0 " + self.message.toPlainText()
		self.sock.send(message.encode())


class Chat(QObject):
	messageReceived = pyqtSignal(str)

	def __init__(self, sock):
		super().__init__()
		self.sock = sock
		QTimer.singleShot(1000, self.check_message)

	def check_message(self):
		try:
			message = self.sock.recv(2048).decode()
			print("GET ", message)
		except Exception:
			QTimer.singleShot(1000, self.check_message)
			return
		if len(message) > 0:
			self.messageReceived.emit(message)
			QTimer.singleShot(1000, self.check_message)

def application():

	app = QApplication(sys.argv)
	host = os.environ["HOST"]
	port = os.environ["PORT"]
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.connect((host, int(port)))
	sock.setblocking(False)
	window = Window(sock)
	chat = Chat(sock)
	chat.messageReceived.connect(window.show_message)

	window.show()

	sys.exit(app.exec_())

if __name__ == "__main__":
    application()
