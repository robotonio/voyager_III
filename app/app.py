import threading
import time
import random
from tasks.task3_interest import get_interest_points
from flask import Flask, jsonify, render_template
from dotenv import load_dotenv
import os

load_dotenv()  # load variables from .env

google_api_key = os.getenv('GOOGLE_API_KEY')
if google_api_key is None:
    raise ValueError("Δεν βρέθηκε μεταβλητή GOOGLE_API_KEY στο αρχείο .env")

app = Flask(__name__)

# Global μεταβλητή για τα σημεία ενδιαφέροντος (dump data)
interest_points = []

def background_task():
    """Background task που ενημερώνει τα σημεία ενδιαφέροντος κάθε 5 δευτερόλεπτα."""
    global interest_points
    while True:
        # Ενημερώνουμε τα interest points με νέα dump data
        interest_points = get_interest_points()
        print("Ενημερώθηκαν τα interest points:", interest_points)
        time.sleep(5)

# Endpoint για την απόδοση των interest points ως JSON (για χρήση από το Google Maps view)
@app.route('/points')
def points_endpoint():
    return jsonify(interest_points)

# Endpoint για την προβολή του Google Maps view
@app.route('/map')
def map_view():
    return render_template('map.html')

# Endpoint για την προβολή της εικόνας VTX με overlay (εδώ απλά δείχνουμε dump data ως λίστα)
@app.route('/')
def index():
    return render_template('index.html', google_api_key=os.getenv('GOOGLE_API_KEY'))

if __name__ == '__main__':
    # Ξεκινάμε το background task σε ξεχωριστό thread
    thread = threading.Thread(target=background_task, daemon=True)
    thread.start()
    
    # Ξεκινάμε τον Flask server (προσβάσιμος από localhost)
    app.run(debug=True, host='0.0.0.0', port=3000)
