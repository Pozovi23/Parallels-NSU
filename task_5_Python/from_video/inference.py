import queue

from ultralytics import YOLO


def predict(img_queue, output_queue, stop_event):
    local_model = YOLO("yolov8n-pose.pt").to("cpu")

    while not stop_event.is_set():
        try:
            frame_number, frame = img_queue.get(timeout=0.1)
            results = local_model.predict(frame, verbose=False)
            output_queue.put((frame_number, results[0].plot(labels=False, boxes=False)))
        except queue.Empty:
            continue
        except Exception as e:
            print(f"Error: {e}")
            break
