import random

import cv2


class WindowImage:
    def __init__(self):
        self.__window_name = str(random.random() * (10**10))

    def show(self, img):
        try:
            if img is not None:
                cv2.imshow(self.__window_name, img)

        except:
            raise RuntimeError("Could not show image")

    def __del__(self):
        try:
            cv2.destroyWindow(self.__window_name)
        except:
            pass
