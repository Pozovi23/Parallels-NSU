import logging
import os
import random
import time

import cv2

window_log_path = "./log/window.log"


def window_logging(message):
    if not os.path.exists(window_log_path):
        with open(window_log_path, "w") as file:
            pass
    logging.basicConfig(level=logging.CRITICAL, filename=window_log_path, filemode="a")
    logging.critical(message)


class WindowImage:
    def __init__(self, frequency):
        self.__frequency = frequency
        self.__window_name = str(random.random() * (10**10))

    def show(self, img):
        try:
            if img is not None:
                cv2.imshow(self.__window_name, img)
                time.sleep(1 / self.__frequency)

        except:
            window_logging("Could not show image")
            raise RuntimeError("Could not show image")

    def __del__(self):
        try:
            cv2.destroyWindow(self.__window_name)
        except:
            pass
