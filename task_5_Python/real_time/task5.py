import queue
import threading

import cv2

from inference import predict
from sensor_cam import SensorCam
from window_image import WindowImage


def execute(number_of_threads):

    stop_event = threading.Event()
    frame_queue = queue.Queue(maxsize=20)
    video_capture = SensorCam("/dev/video0", 1200, 800)

    video_capture_thread = threading.Thread(
        target=video_capture.get, args=(frame_queue, stop_event)
    )
    video_capture_thread.start()
    output_queue = queue.Queue(maxsize=20)

    yolo_threads = []

    for i in range(number_of_threads):
        yolo_threads += [
            threading.Thread(
                target=predict, args=(frame_queue, output_queue, stop_event)
            )
        ]
        yolo_threads[i].start()

    window = WindowImage()

    while True:
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

        try:
            frame = output_queue.get(timeout=0.1)
            window.show(frame)

        except queue.Empty:
            continue

    stop_event.set()

    if video_capture_thread.is_alive():
        video_capture_thread.join()

    for t in yolo_threads:
        if t.is_alive():
            t.join()
            if t.is_alive():
                print(f"Thread {t.name} did not terminate!")


execute(9)
