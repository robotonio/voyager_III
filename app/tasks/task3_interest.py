import random

def get_interest_points():
    point1 = {
        "lat": round(random.uniform(37.5, 38.2), 4),
        "lng": round(random.uniform(23.5, 24.1), 4)
    }
    point2 = {
        "lat": round(random.uniform(37.5, 38.2), 4),
        "lng": round(random.uniform(23.5, 24.1), 4)
    }
    point3 = {
        "lat": round(random.uniform(37.5, 38.2), 4),
        "lng": round(random.uniform(23.5, 24.1), 4)
    }
    return [point1, point2, point3]
