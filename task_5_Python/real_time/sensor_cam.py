import os
import queue

import cv2


class Camera:
    def __init__(self, camera_sys_name):
        self.cap = cv2.VideoCapture(camera_sys_name)
        if not self.cap.isOpened():
            raise RuntimeError(f"{camera_sys_name} is not available")

    def release(self):
        if self.cap.isOpened():
            self.cap.release()


class SensorCam:
    def __init__(self, camera_sys_name, width, height):
        self.__camera = Camera(camera_sys_name)
        self.__camera_name = camera_sys_name
        self.__width = width
        self.__height = height
        self.__last_frame = None

    def get(self, frame_queue, stop_event):
        while not stop_event.is_set():
            try:
                ret, frame = self.__camera.cap.read()
                self.__last_frame = cv2.resize(frame, (self.__width, self.__height))
                frame_queue.put(self.__last_frame)

            except queue.Full:
                continue

    def __del__(self):
        self.__camera.release()
