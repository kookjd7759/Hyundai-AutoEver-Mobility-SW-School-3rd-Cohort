import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QWidget
from PyQt5.QtGui import QFont, QPainter, QColor, QPen, QPixmap
from PyQt5.QtCore import QTimer, Qt, QRectF
import os

BASE_PATH = os.path.dirname(os.path.abspath(__file__))
LOGO_IMAGE = os.path.join(BASE_PATH, "image", "volk.png")
BACK_IMAGE = os.path.join(BASE_PATH, "image", "back.png")
MAX_SPEED = 260
MIN_SPEED = 0
MAX_RPM = 8000
MIN_RPM = 1000
LIMITED_RPM = 3000

class SpeedProgress(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.value = 0
        self.setFixedSize(GAUGE_SIZE,GAUGE_SIZE)

    def setValue(self, value):
        self.value = value
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        rect = QRectF(5*WIDTH/800, 5*WIDTH/800, GAUGE_SIZE - 10*WIDTH/800, GAUGE_SIZE - 10*WIDTH/800)
        start_angle = 210 * 16
        span_angle = int(-self.value * 240/MAX_SPEED * 16)  # float을 int로 변환

        pen = QPen(QColor(int(255/MAX_SPEED*self.value), 255 - int(255/MAX_SPEED*self.value),0),int(10 * WIDTH/800))
        painter.setPen(pen)
        painter.drawArc(rect, start_angle, span_angle)

class RPMProgress(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.value = 0
        self.setFixedSize(GAUGE_SIZE,GAUGE_SIZE)

    def setValue(self, value):
        self.value = value
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        rect = QRectF(5*WIDTH/800, 5*WIDTH/800, GAUGE_SIZE - 10*WIDTH/800, GAUGE_SIZE - 10*WIDTH/800)
        start_angle = 210 * 16
        span_angle = int(-self.value * 240/MAX_RPM * 16)  # float을 int로 변환


        pen = QPen(QColor(int(255/MAX_RPM*self.value), 255 - int(255/MAX_RPM*self.value),0),int(10 * WIDTH/800))
        painter.setPen(pen)
        painter.drawArc(rect, start_angle, span_angle)

class ClusterWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Vehicle Cluster")
        self.setGeometry(0, 0, int(800*WIDTH/800), int(480 * HEIGHT/480))  # 창 크기를 800x480으로 설정

        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)

        self.background_label = QLabel(self.central_widget)
        self.background_pixmap = QPixmap(BACK_IMAGE)  # 배경 이미지 경로 설정
        self.background_label.setPixmap(self.background_pixmap.scaled(int(800 * WIDTH/800), int(480 * HEIGHT/480)))
        self.background_label.setGeometry(0, 0, int(800 * WIDTH/800), int(480 * HEIGHT/480))

        self.speed_gauge = SpeedProgress(self.central_widget)
        self.speed_gauge.move(POSITION, POSITION)
        self.speed_label = QLabel("0 km/h", self.central_widget)
        self.speed_label.setFont(QFont("Arial", int(25 * WIDTH/800)))
        self.speed_label.setAlignment(Qt.AlignCenter)
        self.speed_label.resize(GAUGE_SIZE, GAUGE_SIZE)
        self.speed_label.move(POSITION, POSITION)

        self.rpm_gauge = RPMProgress(self.central_widget)
        self.rpm_gauge.move(WIDTH - POSITION - GAUGE_SIZE, POSITION)
        self.rpm_label = QLabel("0 rpm", self.central_widget)
        self.rpm_multiple_label = QLabel("x1000",self.central_widget)
        self.rpm_label.setFont(QFont("Arial", int(25 * WIDTH/800)))
        self.rpm_label.setAlignment(Qt.AlignCenter)
        self.rpm_label.resize(GAUGE_SIZE, GAUGE_SIZE)
        self.rpm_label.move(WIDTH - POSITION - GAUGE_SIZE, POSITION)
        self.rpm_multiple_label.setFont(QFont("Arial", int(10 * WIDTH/800)))
        self.rpm_multiple_label.setAlignment(Qt.AlignCenter)
        self.rpm_multiple_label.resize(GAUGE_SIZE, GAUGE_SIZE)
        self.rpm_multiple_label.move(WIDTH - POSITION - GAUGE_SIZE, POSITION - int(15 * WIDTH/800))

        self.logo_label = QLabel(self.central_widget)
        self.logo_pixmap = QPixmap(LOGO_IMAGE)  # 로고 이미지 경로 설정
        self.logo_label.setPixmap(self.logo_pixmap.scaled(int(150 * WIDTH/800), int(150 * WIDTH/800), Qt.KeepAspectRatio))
        self.logo_label.setAlignment(Qt.AlignCenter)
        self.logo_label.move(int(WIDTH - 150 * WIDTH/800) // 2, POSITION)

        self.text_label = QLabel("P", self.central_widget) #주행 모드 표시
        self.text_label.setFont(QFont("Arial", int(100 * WIDTH/800)))
        self.text_label.setAlignment(Qt.AlignCenter)
        self.text_label.setStyleSheet('color:rgb(255,255,255)')
        self.text_label.resize(int(150 * WIDTH/800), int(150 * WIDTH/800))
        self.text_label.move(int(WIDTH - 150 * WIDTH/800) // 2, POSITION + int(200 * HEIGHT/480))

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_gauge)
        self.timer.start(200)  # 200ms 마다 업데이트

        self.current_speed = 0
        self.current_rpm = 0

    def update_gauge(self):
        # 여기서 실제 속도 값을 가져와서 업데이트합니다.
        self.current_speed = max(self.current_speed - 1, 0)
        self.speed_label.setText(f"{self.current_speed} km/h")
        self.speed_label.setStyleSheet(f'color:rgb({int(255/MAX_SPEED*self.current_speed)},{255 - int(255/MAX_SPEED*self.current_speed)},0)')
        self.speed_gauge.setValue(self.current_speed)  # 예시 값
        if self.current_rpm < MIN_RPM:
            self.current_rpm = MIN_RPM
        else:
            self.current_rpm = max(self.current_rpm - 100, MIN_RPM)
        self.rpm_label.setText(f"{self.current_rpm/1000} rpm")
        self.rpm_label.setStyleSheet(f'color:rgb({int(255/MAX_RPM*self.current_rpm)},{255 - int(255/MAX_RPM*self.current_rpm)},0)')
        self.rpm_multiple_label.setStyleSheet(f'color:rgb({int(255/MAX_RPM*self.current_rpm)},{255 - int(255/MAX_RPM*self.current_rpm)},0)')
        self.rpm_gauge.setValue(self.current_rpm)

    def change_gauge(self):
        # 여기서 실제 속도 값을 가져와서 업데이트합니다.
        self.speed_label.setText(f"{self.current_speed} km/h")
        self.speed_label.setStyleSheet(f'color:rgb({int(255/MAX_SPEED*self.current_speed)},{255 - int(255/MAX_SPEED*self.current_speed)},0)')
        self.speed_gauge.setValue(self.current_speed)  # 예시 값
        self.rpm_label.setText(f"{self.current_rpm/1000} rpm")
        self.rpm_label.setStyleSheet(f'color:rgb({int(255/MAX_RPM*self.current_rpm)},{255 - int(255/MAX_RPM*self.current_rpm)},0)')
        self.rpm_multiple_label.setStyleSheet(f'color:rgb({int(255/MAX_RPM*self.current_rpm)},{255 - int(255/MAX_RPM*self.current_rpm)},0)')
        self.rpm_gauge.setValue(self.current_rpm)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            self.close()
        elif event.key() == Qt.Key_P and self.current_speed == 0:
            self.text_label.setStyleSheet('color:rgb(255,255,255)')
            self.update_logo("P")
        elif event.key() == Qt.Key_D and self.current_speed == 0:
            self.text_label.setStyleSheet('color:rgb(255,255,255)')
            self.update_logo("D")
        elif event.key() == Qt.Key_R and self.current_speed == 0:
            self.text_label.setStyleSheet('color:rgb(255,0,0)')
            self.update_logo("R")
        elif event.key() == Qt.Key_N and self.current_speed == 0:
            self.text_label.setStyleSheet('color:rgb(255,255,255)')
            self.update_logo("N")
        elif event.key() == Qt.Key_Up and (self.text_label.text() == "D" or self.text_label.text() == "R"):
            self.current_speed = min(self.current_speed + 3, MAX_SPEED)  # 위 방향키로 엑셀
            if self.current_rpm < LIMITED_RPM:
                self.current_rpm = min(self.current_rpm + 100, LIMITED_RPM)
            else:
                self.current_rpm = min(self.current_rpm + 20, MAX_RPM)
            self.change_gauge()
        elif event.key() == Qt.Key_Down:
            self.current_speed = max(self.current_speed - 1, MIN_SPEED)  # 아래 방향키로 브레이크
            self.current_rpm = max(self.current_rpm - 100, MIN_RPM)
            self.change_gauge()

    def update_logo(self, text):
        self.text_label.setText(text)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    screen = app.primaryScreen()
    size = screen.availableGeometry().size()

    WIDTH = size.width()
    HEIGHT = size.height()
    GAUGE_SIZE = int(250 * WIDTH / 800)
    POSITION = int(40 * WIDTH / 800)

    window = ClusterWindow()
    window.showFullScreen()  # 전체 화면 대신 창 크기를 800x480으로 설정
    sys.exit(app.exec_())