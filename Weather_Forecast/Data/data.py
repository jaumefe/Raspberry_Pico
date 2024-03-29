import serial
import time

serial_port = '/dev/ttyACM0'
baud_rate = 115200

# Establish a connection to the serial port
ser = serial.Serial(serial_port, baud_rate, timeout=1)

try:
    while True:
        # Read data from the serial port
        data = ser.readline().decode('utf-8').strip()

        # If data received, print it
        if data:
            print("Received data from serial port: ", data)
            # Give the device time to send data again
            time.sleep(0.5)

# To close the serial port gracefully, use Ctrl+C to break the loop
except KeyboardInterrupt:
    print("Closing the serial port.")
    ser.close()
