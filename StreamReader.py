import argparse
import cv2

def main(ip):
    url = "http://" + ip + ":81/stream"
    cap = cv2.VideoCapture(url)

    while True:
        ret, frame = cap.read()

        if not ret:
            break

        if frame[0][0][2] > 100 and (frame[0][0][0] < 100 and frame[0][0][1] < 100): # B G R
            print("Red pixel detected")

        cv2.imshow("Rover Stream", frame)
        key = cv2.waitKey(1)

        if key == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Displays a video file from an ESP32 camera")
    parser.add_argument("url", help="The URL for the stream (This can be found in the serial monitor output)")
    args = parser.parse_args()
    url = args.url
    main(url)
    