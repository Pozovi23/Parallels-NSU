import logging
import os

import cv2

from sensor_x import Sensor

camera_log_path = "./log/camera.log"


def camera_logging(message):
    if not os.path.exists(camera_log_path):
        with open(camera_log_path, "w") as file:
            pass
    logging.basicConfig(level=logging.CRITICAL, filename=camera_log_path, filemode="a")
    logging.critical(message)


class Camera:
    def __init__(self, camera_sys_name):
        self.cap = cv2.VideoCapture(camera_sys_name)
        if not self.cap.isOpened():
            camera_logging(f"Camera with {camera_sys_name} name is not available")
            raise RuntimeError(f"{camera_sys_name} is not available")

    def release(self):
        if self.cap.isOpened():
            self.cap.release()


class SensorCam(Sensor):
    def __init__(self, camera_sys_name, width, height):
        self.__camera = Camera(camera_sys_name)
        self.__camera_name = camera_sys_name
        self.__width = width
        self.__height = height

    def get(self):
        try:
            ret, frame = self.__camera.cap.read()
            return cv2.resize(frame, (self.__width, self.__height))

        except:
            camera_logging(f"Connection with {self.__camera_name} camera was lost")
            raise RuntimeError(f"Connection with {self.__camera_name} camera was lost")

    def __del__(self):
        self.__camera.release()
