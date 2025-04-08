import cv2


class VideoReader:
    def __init__(self, input_path, stop_event):
        self.frame_reading = True
        self.__cap = cv2.VideoCapture(input_path)
        if self.__cap.isOpened():
            self.fps = int(self.__cap.get(cv2.CAP_PROP_FPS))
            self.width = int(self.__cap.get(cv2.CAP_PROP_FRAME_WIDTH))
            self.height = int(self.__cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        else:
            print("Can not open video!")
            self.__cap = None
            self.fps = None
            self.width = None
            self.height = None
            stop_event.set()

    def __del__(self):
        if self.__cap is not None:
            self.__cap.release()

    def get_frames(self, frame_queue):
        frame_counter = 0
        while True:
            ret, frame = self.__cap.read()
            if not ret:
                break

            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            frame_queue.put((frame_counter, frame))
            frame_counter += 1
        self.frame_reading = False
