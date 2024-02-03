import PyQt5
from PyQt5.QtWidgets import QApplication, QMainWindow
from client_ui import Ui_MainWindow

import sys


class Window(QMainWindow, Ui_MainWindow):
	def __init__(self):
		super().__init__()
		self.setupUi(self)
		self.send.clicked.connect(self.send_message)

	def send_message(self):
		message = self.message.toPlainText()
		print("message: ", message)


def application():
	
    app = QApplication(sys.argv)
	
    window = Window()
    window.show()

    sys.exit(app.exec_())


if __name__ == "__main__":
    application()