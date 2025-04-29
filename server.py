# server.py: Flask web server to receive IoT sensor data
# This server accepts HTTP POST requests from Arduino devices and logs the sensor readings.
# Ensure Flask is installed (`pip install flask`) before running. Run with: python server.py

from flask import Flask, request

app = Flask(__name__)

# Route to receive sensor data
@app.route('/sensor-data', methods=['POST'])
def receive_data():
    # Get JSON data from request (or form data if sent as form-encoded)
    data = request.get_json(force=True) or request.form.to_dict()
    if not data:
        return "No data received", 400
    # Log the received sensor data
    print(f"Received sensor data: {data}")
    return "OK", 200

if __name__ == '__main__':
    # Run the Flask app on all network interfaces so the IoT devices can reach it
    app.run(host='0.0.0.0', port=5000)
