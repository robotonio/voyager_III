import threading
import time
import csv
from CanSatData import CanSatData

class ESP32Task(threading.Thread):
    """
    Thread that reads telemetry data from the ESP32 (or generates dummy data),
    parses it into a CanSatData object, saves the data to a CSV file,
    and keeps the most recent data for quick access.
    """
    
    def __init__(self, csv_filename="esp32_data.csv", port=None, baudrate=115200):
        super().__init__(daemon=True)
        self.csv_filename = csv_filename
        self.port = port          # If using a real ESP32, set the COM port (e.g., "COM3")
        self.baudrate = baudrate  # Baud rate, e.g., 115200
        self.last_data = None     # Will store the most recent CanSatData object

        # For real serial reading, you would import and use pyserial:
        # import serial
        # self.ser = serial.Serial(port=self.port, baudrate=self.baudrate, timeout=1)

    def run(self):
        """
        Main loop: continuously reads and processes telemetry data.
        """
        while True:
            # For dummy data, generate a CanSatData object using its create_dump() method,
            # then convert it to a telemetry string.
            telemetry_line = self.read_telemetry_line()
            
            if telemetry_line:
                data_obj = CanSatData.parse_from_string(telemetry_line)
                if data_obj:
                    self.last_data = data_obj
                    self.save_to_csv(data_obj)
                    print("New data:", data_obj.to_string())
            time.sleep(1)  # Wait 1 second before reading again

    def read_telemetry_line(self):
        """
        Returns a dummy telemetry string by using the CanSatData.create_dump() method.
        In a real application, you would read from the serial port, for example:
            line = self.ser.readline().decode('utf-8').strip()
        """
        dummy_data = CanSatData.create_dump()
        return dummy_data.to_string()

    def save_to_csv(self, data_obj):
        """
        Saves the CanSatData object to a CSV file.
        If the file does not exist, headers are written first.
        Updated to include gps_time and timestamp.
        """
        file_exists = False
        try:
            with open(self.csv_filename, "r", newline="", encoding="utf-8") as f:
                file_exists = True
        except FileNotFoundError:
            pass

        with open(self.csv_filename, "a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            if not file_exists:
                # Write headers including gps_time and timestamp
                writer.writerow([
                    "altitude", "temperature", "pressure", "gps_time",
                    "latitude", "longitude", "pitch", "roll", "yaw",
                    "is_vtx_on", "hotspots", "timestamp"
                ])
            writer.writerow([
                data_obj.altitude,
                data_obj.temperature,
                data_obj.pressure,
                data_obj.gps_time,
                data_obj.latitude,
                data_obj.longitude,
                data_obj.pitch,
                data_obj.roll,
                data_obj.yaw,
                data_obj.is_vtx_on,
                data_obj.hotspots,  # You may convert this to a string if needed
                data_obj.timestamp
            ])

    def get_data(self):
        """
        Returns the most recent CanSatData object.
        """
        return self.last_data


# Example usage of the task (for testing):
if __name__ == "__main__":
    # Create and start the ESP32Task
    esp32_task = ESP32Task(csv_filename="esp32_data.csv")
    esp32_task.start()

    # Let the thread run for a few seconds
    time.sleep(10)

    # Print the latest data
    latest = esp32_task.get_data()
    if latest:
        print("Latest data:", latest.to_string())
