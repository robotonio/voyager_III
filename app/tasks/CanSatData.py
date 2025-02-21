import time
import random

class CanSatData:
    """
    Class representing a telemetry packet for the CanSat.
    It includes:
      - altitude (float) in meters
      - temperature (float) in °C
      - pressure (float) in hPa
      - gps_time (str) in "HH:MM:SS" format (from the GPS module)
      - latitude (float)
      - longitude (float)
      - pitch (float) in degrees
      - roll (float) in degrees
      - yaw (float) in degrees
      - is_vtx_on (int) (1 = True, 0 = False)
      - hotspots (list of tuples, e.g., [(lat, lng), ...])
      - timestamp (float) time the data was read (in seconds, e.g. Unix timestamp)
    """

    def __init__(self, altitude=0.0, temperature=0.0, pressure=0.0,
                 gps_time="00:00:00", latitude=0.0, longitude=0.0,
                 pitch=0.0, roll=0.0, yaw=0.0, is_vtx_on=0, hotspots=None,
                 timestamp=0.0):
        if hotspots is None:
            hotspots = []
        self.altitude = altitude
        self.temperature = temperature
        self.pressure = pressure
        self.gps_time = gps_time  # Time from GPS in "HH:MM:SS"
        self.latitude = latitude
        self.longitude = longitude
        self.pitch = pitch
        self.roll = roll
        self.yaw = yaw
        self.is_vtx_on = is_vtx_on
        self.hotspots = hotspots
        self.timestamp = timestamp  # Time (in seconds) when the data was read

    @staticmethod
    def parse_from_string(data_str):
        """
        Parses a telemetry string in the format:
        #altitude,temperature,pressure,gps_time,latitude,longitude,pitch,roll,yaw,is_vtx_on,
        hotspot1_lat,hotspot1_lng,...[,timestamp]#
        
        Example with timestamp:
        #150.5,25.3,1013.2,12:34:56,37.9838,23.7275,10.5,5.2,2.0,1,37.9839,23.7276,37.984,23.7277,1619191234.567#
        
        If no timestamp is provided, timestamp is set to 0.
        Returns a CanSatData object or None if parsing fails.
        """
        data_str = data_str.strip()
        if data_str.startswith('#') and data_str.endswith('#'):
            data_str = data_str[1:-1].strip()
        else:
            return None

        parts = data_str.split(',')
        # We expect at least 10 fields: altitude, temperature, pressure, gps_time, latitude,
        # longitude, pitch, roll, yaw, is_vtx_on
        if len(parts) < 10:
            return None

        try:
            altitude = float(parts[0])
            temperature = float(parts[1])
            pressure = float(parts[2])
            gps_time = parts[3].strip()
            latitude = float(parts[4])
            longitude = float(parts[5])
            pitch = float(parts[6])
            roll = float(parts[7])
            yaw = float(parts[8])
            is_vtx_on = int(parts[9])

            # Determine if a timestamp is provided:
            # Expected mandatory fields count = 10. Remaining fields correspond to hotspots.
            # If the number of remaining fields is odd, the last field is the timestamp.
            remaining = len(parts) - 10
            if remaining % 2 == 1:
                # Last field is timestamp
                timestamp = float(parts[-1])
                hotspot_parts = parts[10:-1]
            else:
                timestamp = 0.0
                hotspot_parts = parts[10:]

            hotspots = []
            for i in range(0, len(hotspot_parts), 2):
                lat_hot = float(hotspot_parts[i])
                lng_hot = float(hotspot_parts[i+1])
                hotspots.append((lat_hot, lng_hot))

            return CanSatData(
                altitude=altitude,
                temperature=temperature,
                pressure=pressure,
                gps_time=gps_time,
                latitude=latitude,
                longitude=longitude,
                pitch=pitch,
                roll=roll,
                yaw=yaw,
                is_vtx_on=is_vtx_on,
                hotspots=hotspots,
                timestamp=timestamp
            )
        except (ValueError, IndexError):
            return None

    def to_string(self):
        """
        Converts the object to a string in the format:
        #altitude,temperature,pressure,gps_time,latitude,longitude,pitch,roll,yaw,is_vtx_on,
        hotspot1_lat,hotspot1_lng,...,timestamp#
        """
        hotspots_str = []
        for (lat_hot, lng_hot) in self.hotspots:
            hotspots_str.append(f"{lat_hot},{lng_hot}")
        hotspots_joined = ",".join(hotspots_str)
        main_str = (
            f"{self.altitude},"
            f"{self.temperature},"
            f"{self.pressure},"
            f"{self.gps_time},"
            f"{self.latitude},"
            f"{self.longitude},"
            f"{self.pitch},"
            f"{self.roll},"
            f"{self.yaw},"
            f"{self.is_vtx_on}"
        )
        # Append timestamp as the last field
        if hotspots_joined:
            final_str = f"{main_str},{hotspots_joined},{self.timestamp}"
        else:
            final_str = f"{main_str},{self.timestamp}"
        return f"#{final_str}#"

    def update_from_string(self, data_str):
        """
        Updates the properties of the object using a telemetry string in the above format.
        """
        new_data = CanSatData.parse_from_string(data_str)
        if new_data is not None:
            self.altitude = new_data.altitude
            self.temperature = new_data.temperature
            self.pressure = new_data.pressure
            self.gps_time = new_data.gps_time
            self.latitude = new_data.latitude
            self.longitude = new_data.longitude
            self.pitch = new_data.pitch
            self.roll = new_data.roll
            self.yaw = new_data.yaw
            self.is_vtx_on = new_data.is_vtx_on
            self.hotspots = new_data.hotspots
            self.timestamp = new_data.timestamp

    # Getters
    def get_altitude(self):
        return self.altitude

    def get_temperature(self):
        return self.temperature

    def get_pressure(self):
        return self.pressure

    def get_gps_time(self):
        return self.gps_time

    def get_latitude(self):
        return self.latitude

    def get_longitude(self):
        return self.longitude

    def get_pitch(self):
        return self.pitch

    def get_roll(self):
        return self.roll

    def get_yaw(self):
        return self.yaw

    def get_is_vtx_on(self):
        return self.is_vtx_on

    def get_hotspots(self):
        return self.hotspots

    def get_timestamp(self):
        return self.timestamp

    def print_data(self):
        """
        Prints the object's data in a readable format.
        """
        print("=== CanSatData ===")
        print(f"Altitude:    {self.altitude}")
        print(f"Temperature: {self.temperature}")
        print(f"Pressure:    {self.pressure}")
        print(f"GPS Time:    {self.gps_time}")
        print(f"Latitude:    {self.latitude}")
        print(f"Longitude:   {self.longitude}")
        print(f"Pitch:       {self.pitch}")
        print(f"Roll:        {self.roll}")
        print(f"Yaw:         {self.yaw}")
        print(f"isVTXOn:     {self.is_vtx_on}")
        print(f"Hotspots:    {self.hotspots}")
        print(f"Timestamp:   {self.timestamp}")
        print("==================")

    @staticmethod
    def create_dump():
        """
        Creates a dummy CanSatData object with random values within logical ranges for a CanSat
        falling from around 1000 meters, but with GPS coordinates restricted to the municipality
        of Korydallos in Attica.
        
        Telemetry data format now:
        #altitude,temperature,pressure,gps_time,latitude,longitude,pitch,roll,yaw,is_vtx_on,
        hotspot...,timestamp#
        
        - altitude: between 100 and 1000 meters
        - temperature: between 20 and 35 °C
        - pressure: between 950 and 1020 hPa
        - gps_time: a dummy time in "HH:MM:SS" format
        - latitude: restricted approximately between 37.93 and 37.95 (Korydallos area)
        - longitude: restricted approximately between 23.68 and 23.72 (Korydallos area)
        - pitch: between 0 and 20°
        - roll: between 0 and 15°
        - yaw: between 0 and 10°
        - is_vtx_on: 1 with 90% probability
        - hotspots: 0 to 2 points, close to the main coordinates with very slight randomness.
        - timestamp: current time in seconds (using time.time())
        """
        altitude = round(random.uniform(100.0, 1000.0), 1)
        temperature = round(random.uniform(20.0, 35.0), 1)
        pressure = round(random.uniform(950.0, 1020.0), 1)
        # Generate a dummy GPS time in HH:MM:SS format
        hour = random.randint(0, 23)
        minute = random.randint(0, 59)
        second = random.randint(0, 59)
        gps_time = f"{hour:02d}:{minute:02d}:{second:02d}"
        
        # Restrict coordinates to the approximate area of Korydallos, Attica.
        latitude = round(random.uniform(37.93, 37.95), 4)
        longitude = round(random.uniform(23.68, 23.72), 4)
        
        pitch = round(random.uniform(0, 20), 1)
        roll = round(random.uniform(0, 15), 1)
        yaw = round(random.uniform(0, 10), 1)
        is_vtx_on = 1 if random.random() < 0.9 else 0

        num_hotspots = random.randint(1, 3)
        hotspots = []
        for _ in range(num_hotspots):
            h_lat = round(latitude + random.uniform(-0.001, 0.001), 4)
            h_lng = round(longitude + random.uniform(-0.001, 0.001), 4)
            hotspots.append((h_lat, h_lng))

        timestamp = time.time()  # Current time in seconds

        return CanSatData(
            altitude, temperature, pressure, gps_time, latitude, longitude,
            pitch, roll, yaw, is_vtx_on, hotspots, timestamp
        )


# Example usage of dump:
if __name__ == "__main__":
    dump_obj = CanSatData.create_dump()
    dump_obj.print_data()
    print("Dump as string:", dump_obj.to_string())
