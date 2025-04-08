import argparse
import queue
import threading
import time

import cv2

from inference import predict
from video_reader import VideoReader


def execute(number_of_threads):
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_path", type=str, required=True)
    parser.add_argument("--multithreading", type=bool, required=True)
    parser.add_argument("--output_path", type=str, required=True)
    args = parser.parse_args()

    input_path = args.input_path
    if not args.multithreading:
        number_of_threads = 1

    output_path = args.output_path

    stop_event = threading.Event()
    frame_queue = queue.Queue()
    video_reader = VideoReader(input_path, stop_event)

    if video_reader.width is None:
        return

    fps, width, height = video_reader.fps, video_reader.width, video_reader.height

    start_time = time.time()

    get_frames_thread = threading.Thread(
        target=video_reader.get_frames, args=(frame_queue,)
    )
    get_frames_thread.start()

    output_queue = queue.Queue()

    yolo_threads = []

    for i in range(number_of_threads):
        yolo_threads += [
            threading.Thread(
                target=predict, args=(frame_queue, output_queue, stop_event)
            )
        ]
        yolo_threads[i].start()

    fourcc = cv2.VideoWriter_fourcc(*"mp4v")
    out = cv2.VideoWriter(output_path, fourcc, fps, (width, height))

    buffer = {}
    frame_counter = 0
    while (
        not frame_queue.empty()
        or video_reader.frame_reading
        or not output_queue.empty()
    ):
        try:
            frame_num, frame = output_queue.get()
            buffer[frame_num] = frame

            while frame_counter in buffer:
                out.write(cv2.cvtColor(buffer.pop(frame_counter), cv2.COLOR_RGB2BGR))
                frame_counter += 1

        except queue.Empty:
            continue

    out.release()
    stop_event.set()

    work_time = time.time() - start_time

    if get_frames_thread.is_alive():
        get_frames_thread.join()

    for t in yolo_threads:
        if t.is_alive():
            t.join()
            if t.is_alive():
                print(f"Thread {t.name} did not terminate!")

    return work_time


def test_time():
    for h in range(1, 13):
        sum_time = 123456789
        for l in range(3):
            curr_res = execute(h)
            sum_time = min(curr_res, sum_time)

        print(f"work time: {sum_time}")


test_time()
