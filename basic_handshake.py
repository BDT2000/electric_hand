import serial
import time

# Use the COM port / device assigned after pairing
ser = serial.Serial('COM4', 115200, timeout=1)  # Windows example
# ser = serial.Serial('/dev/rfcomm0', 115200, timeout=1)  # Linux example

time.sleep(2)
ser.write(b"hello\n")
time.sleep(0.1)  # give ESP32 a moment to respond
response = ser.readline().decode().strip()
if response:
    print(response)
else:
    print("No response received")