import cv2
import time
import random
import math
import csv
import ast
import enum
from tasks.CanSatData import CanSatData
from enum import Flag
from dataclasses import dataclass

MERGE_THRESHOLD = 20  # for merging nearby points (not used in these methods)


class DetectionMethod(Flag):
    COMPUTER_VISION = enum.auto()
    MACHINE_LEARNING = enum.auto()
    CANSAT_HOTSPOTS = enum.auto()
    THE_CANSAT = enum.auto()


@dataclass(slots=True)
class DetectionPoint:
    latitude: float
    longitude: float
    score: float
    methods: DetectionMethod


@dataclass(slots=True)
class RatedDetectionPoint:
    detection_point: DetectionPoint
    rating: int

    @property
    def latitude(self):
        return self.detection_point.latitude

    @property
    def longitude(self):
        return self.detection_point.longitude

    @property
    def score(self):
        return self.detection_point.score

    @property
    def methods(self):
        return self.detection_point.methods


class VTXProcessor:
    """
    VTXProcessor handles image capture from a secondary monitor,
    processes the image to detect interest points, and provides
    utility functions such as converting an image pixel coordinate
    to estimated world GPS coordinates using CanSatData.
    """

    cap: cv2.VideoCapture
    max_spots: int
    last_image: cv2.typing.MatLike | None
    last_timestamp: float | None
    debug: bool

    def __init__(self, camera_index: int = 1, max_spots: int = 20, debug: bool = False):
        """
        Initializes the VTXProcessor.

        :param camera_index: The index of the camera (second monitor).
        :param maxspots: Maximum number of interest points to return.
        """
        if not debug:
            self.cap = cv2.VideoCapture(camera_index)
        self.max_spots = max_spots
        self.last_image = None
        self.last_timestamp = None
        self.debug = debug

    def read_image(self):
        """
        Captures an image from the secondary monitor.
        """
        if not self.debug:
            ret, frame = self.cap.read()
            if not ret:
                return
            self.last_image = frame
        else:
            frame = cv2.imread("static/images/latest.jpg")
            self.last_image = frame
        self.last_timestamp = time.time()

    def image_point_to_world(
        self,
        cansat_data: CanSatData,
        image_point: tuple[int, int],
        image_resolution: tuple[int, int],
    ) -> tuple[float, float]:
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
            [1.0, 0.0, 0.0],
            [0.0, math.cos(roll), -math.sin(roll)],
            [0.0, math.sin(roll), math.cos(roll)],
        ]
        Ry = [
            [math.cos(pitch), 0.0, math.sin(pitch)],
            [0.0, 1.0, 0.0],
            [-math.sin(pitch), 0.0, math.cos(pitch)],
        ]
        Rz = [
            [math.cos(yaw), -math.sin(yaw), 0.0],
            [math.sin(yaw), math.cos(yaw), 0.0],
            [0.0, 0.0, 1.0],
        ]

        def mat_mult(A: list[list[float]], B: list[list[float]]):
            result = [[0.0, 0.0, 0.0] for _ in range(3)]
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
            R[2][0] * dir_cam[0] + R[2][1] * dir_cam[1] + R[2][2] * dir_cam[2],
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

    def detect_cv_points(self, cansat_data: CanSatData) -> list[DetectionPoint]:
        """
        Detects preliminary interest points using computer vision techniques,
        then converts their pixel coordinates to world coordinates.

        Dummy implementation: uses blob detection.

        :param cansat_data: A CanSatData object.
        :return: List of points as dictionaries with keys "lat", "lng", "score", "methods" (set containing "CV").
        """

        if self.last_image is None:
            return []

        points: list[DetectionPoint] = []

        gray = cv2.cvtColor(self.last_image, cv2.COLOR_BGR2GRAY)
        blurred = cv2.GaussianBlur(gray, (23, 23), 0)
        (_, threshholded) = cv2.threshold(blurred, 230, 255, cv2.THRESH_BINARY_INV)
        final_img = cv2.bitwise_not(threshholded)
        # num_labels, labels, stats, _ = cv2.connectedComponentsWithStats(
        #     final_img, connectivity=8
        # )
        contours, _ = cv2.findContours(
            final_img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
        )

        def score_from_area(area: float, image_size: tuple[int, int]):
            def clip(score: float):
                return max(min(score, 1), 0)

            w, h = image_size
            score = (area) / (w * h)
            score *= 8192
            if score > 0.5:
                return clip(score * 2)
            elif score > 0.2:
                return clip(score * 3)
            elif score > 0.1:
                return clip(score * 5)
            elif score > 0.01:
                return clip(score * 10)
            else:
                return clip(score * 30)

        for contour in contours:
            x, y, _, _ = cv2.boundingRect(contour)
            h: int = final_img.shape[0]  # pyright: ignore[reportAny]
            w: int = final_img.shape[1]  # pyright: ignore[reportAny]
            world_coords = self.image_point_to_world(cansat_data, (x, y), (w, h))
            area = cv2.contourArea(contour)
            points.append(
                DetectionPoint(
                    latitude=world_coords[0],
                    longitude=world_coords[1],
                    score=score_from_area(area, (w, h)),
                    methods=DetectionMethod.COMPUTER_VISION,
                )
            )

        return points

    def detect_ml_points(self, cansat_data: CanSatData) -> list[DetectionPoint]:
        """
        Detects interest points using machine learning techniques,
        then converts their pixel coordinates to world coordinates.

        Dummy implementation: returns a few random points.

        :param cansat_data: A CanSatData object.
        :return: List of points as dictionaries with keys "lat", "lng", "score", "methods" (set containing "ML").
        """

        if self.last_image is None:
            return []

        points: list[DetectionPoint] = []

        shape: tuple[int, ...] = self.last_image.shape  # pyright: ignore[reportAny]
        h, w, _ = shape
        image_resolution = (w, h)
        num_points = random.randint(5, 10)
        for _ in range(num_points):
            x = random.randint(0, w - 1)
            y = random.randint(0, h - 1)
            score = random.uniform(0.5, 1.0)
            world_coords = self.image_point_to_world(
                cansat_data, (x, y), image_resolution
            )
            points.append(
                DetectionPoint(
                    latitude=world_coords[0],
                    longitude=world_coords[1],
                    score=score,
                    methods=DetectionMethod.MACHINE_LEARNING,
                )
            )
        return points

    def get_closest_cansat_data(self, image_timestamp: float, csv_filename: str):
        """
        Reads the CSV file containing logged CanSatData and returns the
        CanSatData object whose timestamp is closest to the given image_timestamp.

        :param image_timestamp: The Unix timestamp (in seconds) when the image was captured.
        :param csv_filename: The path to the CSV file containing telemetry data.
        :return: A CanSatData object with the closest timestamp, or None if not found.
        """
        closest_data = None
        smallest_diff = float("inf")
        try:
            with open(csv_filename, "r", newline="", encoding="utf-8") as f:
                reader = csv.reader(f)
                _ = next(reader)  # Skip header row
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
                        closest_data = CanSatData(
                            altitude,
                            temperature,
                            pressure,
                            gps_time,
                            latitude,
                            longitude,
                            pitch,
                            roll,
                            yaw,
                            is_vtx_on,
                            hotspots,
                            timestamp,
                        )
        except FileNotFoundError:
            print("CSV file not found:", csv_filename)
            return None

        return closest_data

    def merge_points(self, points_list: list[DetectionPoint]):
        """
        Merges nearby points from a list based on a dummy distance threshold.
        For simplicity, this dummy implementation uses a threshold in degrees.
        :param points_list: List of point dictionaries (each with "lat", "lng", "score", "methods").
        :return: A new list of merged points.
        """
        merged: list[DetectionPoint] = []
        threshold = 0.0001  # Dummy threshold in degrees (adjust as needed)
        for pt in points_list:
            found = False
            for mpt in merged:
                dist = math.sqrt(
                    (pt.latitude - mpt.latitude) ** 2
                    + (pt.longitude - mpt.longitude) ** 2
                )
                if dist < threshold:
                    mpt.latitude = (mpt.latitude + pt.latitude) / 2
                    mpt.longitude = (mpt.longitude + pt.longitude) / 2
                    mpt.score = max(mpt.score, pt.score)
                    mpt.methods = pt.methods | mpt.methods
                    found = True
                    break
            if not found:
                merged.append(pt)
        return merged

    def assign_rank(self, methods: DetectionMethod):
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
        if (
            methods
            == DetectionMethod.COMPUTER_VISION
            | DetectionMethod.MACHINE_LEARNING
            | DetectionMethod.CANSAT_HOTSPOTS
        ):
            return 1
        elif (
            methods
            == DetectionMethod.MACHINE_LEARNING | DetectionMethod.CANSAT_HOTSPOTS
        ):
            return 2
        elif (
            methods == DetectionMethod.COMPUTER_VISION | DetectionMethod.CANSAT_HOTSPOTS
        ):
            return 3
        elif (
            methods
            == DetectionMethod.COMPUTER_VISION | DetectionMethod.MACHINE_LEARNING
        ):
            return 4
        elif methods == DetectionMethod.MACHINE_LEARNING:
            return 5
        elif methods == DetectionMethod.COMPUTER_VISION:
            return 6
        elif methods == DetectionMethod.CANSAT_HOTSPOTS:
            return 7
        else:
            return 100  # Fallback rank for unexpected combinations

    def score_and_sort_points(
        self,
        cv_points: list[DetectionPoint],
        ml_points: list[DetectionPoint],
        ch_points: list[DetectionPoint],
        cansat_data: CanSatData,
    ) -> list[RatedDetectionPoint]:
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
        fixed_point = RatedDetectionPoint(
            DetectionPoint(
                latitude=cansat_data.latitude,
                longitude=cansat_data.longitude,
                score=1.0,
                methods=DetectionMethod.THE_CANSAT,
            ),
            0,
        )

        all_points = cv_points + ml_points + ch_points
        merged_points = self.merge_points(all_points)

        rated_points: list[RatedDetectionPoint] = []
        for pt in merged_points:
            rated_points.append(RatedDetectionPoint(pt, self.assign_rank(pt.methods)))

        # Sort the merged points by rating (ascending; lower is better) then by score (descending).
        sorted_points = sorted(rated_points, key=lambda p: (p.rating, -p.score))
        sorted_points = sorted_points[: max(0, self.max_spots - 1)]

        final_points = [fixed_point] + sorted_points

        return final_points


# Example usage for testing:
def main():
    vtx_processor = VTXProcessor(camera_index=1, max_spots=50, debug=True)

    dummy_data = CanSatData.create_dump()

    vtx_processor.read_image()
    cv_points = vtx_processor.detect_cv_points(dummy_data)
    ml_points = vtx_processor.detect_ml_points(dummy_data)
    ch_points = [
        DetectionPoint(
            latitude, longitude, score=1, methods=DetectionMethod.CANSAT_HOTSPOTS
        )
        for latitude, longitude in dummy_data.hotspots
    ]

    final_points = vtx_processor.score_and_sort_points(
        cv_points, ml_points, ch_points, dummy_data
    )

    print("Final sorted interest points (with ratings):")
    for pt in final_points:
        print(
            f"Coordinates: ({pt.latitude:.6f}, {pt.longitude:.6f}), Score: {pt.score:.2f}, "
            + f"Methods: {pt.methods}, Rating: {pt.rating}"
        )
