import cv2
import time
import numpy as np
import random
import math
import csv
import ast

from tasks.CanSatData import CanSatData

MERGE_THRESHOLD = 20  # for merging nearby points (not used in these methods)

class VTXProcessor:
    """
    VTXProcessor handles image capture from a secondary monitor,
    processes the image to detect interest points, and provides
    utility functions such as converting an image pixel coordinate
    to estimated world GPS coordinates using CanSatData.
    """
    
    def __init__(self, camera_index=1, maxspots=20, debug=False):
        """
        Initializes the VTXProcessor.
        
        :param camera_index: The index of the camera (second monitor).
        :param maxspots: Maximum number of interest points to return.
        """
        if not debug:
            self.cap = cv2.VideoCapture(camera_index)
        self.maxspots = maxspots
        self.last_image = None
        self.last_timestamp = None
        self.debug = debug

    def read_image(self):
        """
        Captures an image from the secondary monitor.
        
        :return: A tuple (image, timestamp) where image is a BGR numpy array and
                 timestamp is the Unix timestamp (in seconds) when the image was captured.
        """
        if not self.debug:
            ret, frame = self.cap.read()
            if not ret:
                return None, None
            self.last_image = frame
        else:
            frame = cv2.imread("static/images/latest.jpg")
            self.last_image = frame
        self.last_timestamp = time.time()
        return frame, self.last_timestamp

    def image_point_to_world(self, cansat_data, image_point, image_resolution):
        """
        Converts a pixel (x, y) from an image to estimated world (GPS) coordinates,
        using a simplified pinhole camera model.
        
        :param cansat_data: A CanSatData object (with altitude, latitude, longitude, pitch, roll, yaw).
        :param image_point: A tuple (x, y) representing pixel coordinates.
        :param image_resolution: A tuple (width, height) of the image in pixels.
        :return: A tuple (new_lat, new_lng) representing estimated GPS coordinates.
        """
        W, H = image_resolution
        cx, cy = W / 2, H / 2

        horizontal_FOV = 150  # degrees (adjust as needed)
        f = (W / 2) / math.tan(math.radians(horizontal_FOV / 2))

        dx = image_point[0] - cx
        dy = image_point[1] - cy

        norm = math.sqrt(dx**2 + dy**2 + f**2)
        dir_cam = (dx / norm, dy / norm, f / norm)

        pitch = math.radians(cansat_data.pitch)
        roll = math.radians(cansat_data.roll)
        yaw = math.radians(cansat_data.yaw)

        Rx = [
            [1, 0, 0],
            [0, math.cos(roll), -math.sin(roll)],
            [0, math.sin(roll), math.cos(roll)]
        ]
        Ry = [
            [math.cos(pitch), 0, math.sin(pitch)],
            [0, 1, 0],
            [-math.sin(pitch), 0, math.cos(pitch)]
        ]
        Rz = [
            [math.cos(yaw), -math.sin(yaw), 0],
            [math.sin(yaw), math.cos(yaw), 0],
            [0, 0, 1]
        ]

        def mat_mult(A, B):
            result = [[0, 0, 0] for _ in range(3)]
            for i in range(3):
                for j in range(3):
                    for k in range(3):
                        result[i][j] += A[i][k] * B[k][j]
            return result

        Rzy = mat_mult(Rz, Ry)
        R = mat_mult(Rzy, Rx)

        world_dir = (
            R[0][0] * dir_cam[0] + R[0][1] * dir_cam[1] + R[0][2] * dir_cam[2],
            R[1][0] * dir_cam[0] + R[1][1] * dir_cam[1] + R[1][2] * dir_cam[2],
            R[2][0] * dir_cam[0] + R[2][1] * dir_cam[1] + R[2][2] * dir_cam[2]
        )

        Hc = cansat_data.altitude

        if world_dir[2] == 0:
            return (cansat_data.latitude, cansat_data.longitude)
        t = -Hc / world_dir[2]

        dx_m = world_dir[0] * t
        dy_m = world_dir[1] * t

        dlat = dx_m / 111111
        dlon = dy_m / (111111 * math.cos(math.radians(cansat_data.latitude)))

        new_lat = cansat_data.latitude + dlat
        new_lng = cansat_data.longitude + dlon

        return (new_lat, new_lng)

    def detect_cv_points(self, cansat_data: CanSatData) -> list[dict]:
        """
        Detects preliminary interest points using computer vision techniques,
        then converts their pixel coordinates to world coordinates.
        
        Dummy implementation: uses blob detection.
        
        :param cansat_data: A CanSatData object.
        :return: List of points as dictionaries with keys "lat", "lng", "score", "methods" (set containing "CV").
        """
        gray = cv2.cvtColor(self.last_image, cv2.COLOR_BGR2GRAY)
        detector = cv2.SimpleBlobDetector_create()
        keypoints = detector.detect(gray)

        gray = cv2.cvtColor(self.last_image, cv2.COLOR_BGR2GRAY)
        blurred = cv2.GaussianBlur(gray, (23, 23), 0)
        (T, threshInv) = cv2.threshold(blurred, 230, 255,
            cv2.THRESH_BINARY_INV)
        final_img = cv2.bitwise_not(threshInv)
        num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(final_img, connectivity=8)

        # Iterate over each blob (skip label 0, which is the background)
        for label in range(1, num_labels):
            # Create a mask for the current blob
            blob_mask = np.zeros_like(final_img)
            blob_mask[labels == label] = 255
            
            # Retrieve statistics for the blob
            area = stats[label, cv2.CC_STAT_AREA]
            x = stats[label, cv2.CC_STAT_LEFT]
            y = stats[label, cv2.CC_STAT_TOP]
            w = stats[label, cv2.CC_STAT_WIDTH]
            h = stats[label, cv2.CC_STAT_HEIGHT]
            
            print(f"Blob {label}: Area = {area}, Bounding box = ({x}, {y}, {w}, {h})")
            
            # Here you can add any additional processing for each blob
            # For example, you might want to extract the blob region from the original image:
            # blob_region = img[y:y+h, x:x+w]

        points = []
        h, w = self.last_image.shape[:2]
        image_resolution = (w, h)
        for kp in keypoints:
            # for typing purposes
            kp: cv2.KeyPoint = kp
            pixel = (int(kp.pt[0]), int(kp.pt[1]))
            world_coords = self.image_point_to_world(cansat_data, pixel, image_resolution)
            point = {"lat": world_coords[0],
                     "lng": world_coords[1],
                     "score": kp.response,
                     "methods": {"CV"}}
            points.append(point)
        return points

    def detect_ml_points(self, cansat_data: CanSatData):
        """
        Detects interest points using machine learning techniques,
        then converts their pixel coordinates to world coordinates.
        
        Dummy implementation: returns a few random points.
        
        :param cansat_data: A CanSatData object.
        :return: List of points as dictionaries with keys "lat", "lng", "score", "methods" (set containing "ML").
        """
        h, w, _ = self.last_image.shape
        image_resolution = (w, h)
        points = []
        num_points = random.randint(5, 10)
        for _ in range(num_points):
            x = random.randint(0, w - 1)
            y = random.randint(0, h - 1)
            score = random.uniform(0.5, 1.0)
            world_coords = self.image_point_to_world(cansat_data, (x, y), image_resolution)
            points.append({"lat": world_coords[0],
                           "lng": world_coords[1],
                           "score": score,
                           "methods": {"ML"}})
        return points

    def get_closest_cansat_data(self, image_timestamp, csv_filename):
        """
        Reads the CSV file containing logged CanSatData and returns the
        CanSatData object whose timestamp is closest to the given image_timestamp.
        
        :param image_timestamp: The Unix timestamp (in seconds) when the image was captured.
        :param csv_filename: The path to the CSV file containing telemetry data.
        :return: A CanSatData object with the closest timestamp, or None if not found.
        """
        closest_data = None
        smallest_diff = float('inf')
        try:
            with open(csv_filename, "r", newline="", encoding="utf-8") as f:
                reader = csv.reader(f)
                headers = next(reader)  # Skip header row
                # Expected headers: altitude,temperature,pressure,gps_time,latitude,longitude,
                # pitch,roll,yaw,is_vtx_on,hotspots,timestamp
                for row in reader:
                    try:
                        altitude = float(row[0])
                        temperature = float(row[1])
                        pressure = float(row[2])
                        gps_time = row[3]
                        latitude = float(row[4])
                        longitude = float(row[5])
                        pitch = float(row[6])
                        roll = float(row[7])
                        yaw = float(row[8])
                        is_vtx_on = int(row[9])
                        # hotspots are stored as a string; convert using literal_eval
                        hotspots = ast.literal_eval(row[10])
                        timestamp = float(row[11])
                    except (ValueError, IndexError):
                        continue
                    diff = abs(timestamp - image_timestamp)
                    if diff < smallest_diff:
                        smallest_diff = diff
                        # Create a new CanSatData object (assume its __init__ takes the fields in order)
                        closest_data = CanSatData(altitude, temperature, pressure, gps_time,
                                                   latitude, longitude, pitch, roll, yaw,
                                                   is_vtx_on, hotspots, timestamp)
        except FileNotFoundError:
            print("CSV file not found:", csv_filename)
            return None

        return closest_data

    def merge_points(self, points_list: list[dict]):
        """
        Merges nearby points from a list based on a dummy distance threshold.
        For simplicity, this dummy implementation uses a threshold in degrees.
        :param points_list: List of point dictionaries (each with "lat", "lng", "score", "methods").
        :return: A new list of merged points.
        """
        merged = []
        threshold = 0.0001  # Dummy threshold in degrees (adjust as needed)
        for pt in points_list:
            found = False
            for mpt in merged:
                dist = math.sqrt((pt["lat"] - mpt["lat"])**2 + (pt["lng"] - mpt["lng"])**2)
                if dist < threshold:
                    mpt["lat"] = (mpt["lat"] + pt["lat"]) / 2
                    mpt["lng"] = (mpt["lng"] + pt["lng"]) / 2
                    mpt["score"] = max(mpt["score"], pt["score"])
                    mpt["methods"] = mpt["methods"].union(pt["methods"])
                    found = True
                    break
            if not found:
                merged.append(pt)
        return merged

    def assign_rank(self, methods_set):
        """
        Assigns a rank based on the detection methods used.
        Lower rank indicates higher priority.
        
        Ranking:
          1. {"CV", "ML", "CH"} -> rank 1
          2. {"ML", "CH"}      -> rank 2
          3. {"CV", "CH"}      -> rank 3
          4. {"CV", "ML"}      -> rank 4
          5. {"ML"}            -> rank 5
          6. {"CV"}            -> rank 6
          7. {"CH"}            -> rank 7
        :param methods_set: A set of method strings.
        :return: An integer rank.
        """
        if methods_set == {"CV", "ML", "CH"}:
            return 1
        elif methods_set == {"ML", "CH"}:
            return 2
        elif methods_set == {"CV", "CH"}:
            return 3
        elif methods_set == {"CV", "ML"}:
            return 4
        elif methods_set == {"ML"}:
            return 5
        elif methods_set == {"CV"}:
            return 6
        elif methods_set == {"CH"}:
            return 7
        else:
            return 100  # Fallback rank for unexpected combinations

    def score_and_sort_points(self, cv_points, ml_points, ch_points, cansat_data) -> list[dict]:
        """
        Combines the interest points detected via CV, ML, and the hotspots (CH) from the closest
        CanSatData, then ranks and sorts them from most important to least important.
        
        The first point in the returned list will be the fixed point corresponding to the
        CanSat's location at capture time (i.e., cansat_data.latitude, cansat_data.longitude).
        The remaining points are ranked based on:
          1. Points detected by all three methods (CV, ML, CH) -> rank 1
          2. Points detected by ML and CH -> rank 2
          3. Points detected by CV and CH -> rank 3
          4. Points detected by CV and ML -> rank 4
          5. Points detected by ML only -> rank 5
          6. Points detected by CV only -> rank 6
          7. Points detected by CH only -> rank 7
          
        :param cv_points: List of points from CV.
        :param ml_points: List of points from ML.
        :param ch_points: List of points from CanSat hotspots (CH).
        :param cansat_data: The closest CanSatData object.
        :return: A list (length up to self.maxspots) of interest points, each as a dictionary
                 with keys "lat", "lng", "score", "methods", and "rating".
        """
        # Fixed point: the CanSat's location.
        fixed_point = {
            "lat": cansat_data.latitude,
            "lng": cansat_data.longitude,
            "score": 1.0,
            "methods": {"CANSAT"},
            "rating": 0
        }
        
        # Combine all detected points.
        all_points = cv_points + ml_points + ch_points
        
        # Merge points that are very close.
        merged_points = self.merge_points(all_points)
        
        # Assign a rating to each merged point based on the detection methods.
        for pt in merged_points:
            pt["rating"] = self.assign_rank(pt["methods"])
        
        # Sort the merged points by rating (ascending; lower is better) then by score (descending).
        sorted_points = sorted(merged_points, key=lambda p: (p["rating"], -p["score"]))
        
        # Limit to self.maxspots - 1 (since fixed point is added as the first element).
        sorted_points = sorted_points[:max(0, self.maxspots - 1)]
        
        # Prepend the fixed CanSat location as the first point.
        final_points = [fixed_point] + sorted_points
        return final_points

def hotspots_to_points(hotspots):
    return [{"lat": lat, "lng": lng, "score": 1.0, "methods": {"CH"}} for lat, lng in hotspots]

# Example usage for testing:
def main():
    # Create an instance of VTXProcessor.
    vtx_processor = VTXProcessor(camera_index=1, maxspots=50, debug=True)
    
    # For testing, we create a dummy image (1280x720) and a dummy CanSatData object.
    dummy_image = np.zeros((720, 1280, 3), dtype=np.uint8)
    
    class DummyCanSatData:
        def __init__(self):
            self.altitude = 500.0  # meters
            self.latitude = 37.9838
            self.longitude = 23.7275
            self.pitch = 5.0       # degrees
            self.roll = 3.0        # degrees
            self.yaw = 10.0        # degrees
    
    dummy_data = CanSatData.create_dump()
    
    vtx_processor.read_image()
    # Generate dummy points from CV, ML, and CH.
    cv_points = vtx_processor.detect_cv_points(dummy_data)
    ml_points = vtx_processor.detect_ml_points(dummy_data)
    # For CH, we simulate using the CanSat's hotspots.
    # Let's assume dummy_data has a hotspots attribute for testing.
    dummy_data.hotspots = [(37.9840, 23.7276), (37.9841, 23.7277)]
    # ch_points = []  # We'll use get_cansat_hotspots method.
    # ch_points = vtx_processor.get_cansat_hotspots(dummy_data)
    # ^ get_cansat_hotspots doesn't exist??
    ch_points = hotspots_to_points(dummy_data.hotspots)

    final_points = vtx_processor.score_and_sort_points(cv_points, ml_points, ch_points, dummy_data)
    
    print("Final sorted interest points (with ratings):")
    for pt in final_points:
        print(f"Coordinates: ({pt['lat']:.6f}, {pt['lng']:.6f}), Score: {pt['score']:.2f}, "
              f"Methods: {pt['methods']}, Rating: {pt['rating']}")
