import argparse
import os
import queue
import threading

import cv2

from sensor_cam import SensorCam
from sensor_x import SensorX
from window_image import WindowImage

stop_event = threading.Event()


def sensor_get_data(sensor, curr_queue):
    while not stop_event.is_set():
        try:
            res = sensor.get()
            curr_queue.put(res)

        except:
            break


def execute():
    try:
        parser = argparse.ArgumentParser()
        parser.add_argument("--camera_sys_name", type=str, required=True)
        parser.add_argument("--resolution", type=str, required=True)
        parser.add_argument("--frequency", type=float, required=True)
        args = parser.parse_args()

        camera_sys_name = args.camera_sys_name
        resolution_split = args.resolution.split(sep="x")
        width = int(resolution_split[0])
        height = int(resolution_split[1])
        frequency = args.frequency

        if not os.path.exists("./log"):
            os.makedirs("./log")

        sensor0 = SensorX(0.01)
        sensor1 = SensorX(0.1)
        sensor2 = SensorX(1)
        sensor_camera = SensorCam(camera_sys_name, width, height)
        window = WindowImage(frequency)
        sensors = [sensor0, sensor1, sensor2, sensor_camera]

        queues = [queue.Queue() for _ in range(4)]

        last_values = {0: 0, 1: 0, 2: 0, 3: None}

        threads = []
        for i in range(4):
            threads += [
                threading.Thread(target=sensor_get_data, args=(sensors[i], queues[i]))
            ]
            threads[i].start()

        while True:
            for i in range(4):
                while not queues[i].empty():
                    last_values[i] = queues[i].get()

            img = last_values[3]

            if img is not None:
                x_start_draw = width - width // 5
                y_start_draw = height - height // 5
                cv2.rectangle(
                    img,
                    (x_start_draw, y_start_draw),
                    (width, height),
                    (255, 255, 255),
                    -1,
                )

                text_y = y_start_draw + 20
                for i in range(3):
                    if last_values[i] is not None:
                        cv2.putText(
                            img,
                            f"Sensor{i}: {last_values[i]}",
                            (x_start_draw + 10, text_y),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.7,
                            (0, 0, 0),
                            2,
                        )
                        text_y += 20

                window.show(img)

            if cv2.waitKey(1) & 0xFF == ord("q"):
                stop_event.set()
                for t in threads:
                    t.join()
                break

    except Exception as e:
        print(f"Error: {str(e)}")


def main():
    try:
        execute()
    except KeyboardInterrupt:
        print("\nProgram terminated by user")
    except Exception as e:
        print(f"Error: {str(e)}")


main()
