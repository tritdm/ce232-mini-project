import os
from flask import Flask, render_template, jsonify, make_response, Response, stream_with_context
from flask_mqtt import Mqtt
import sqlite3
import json
import time

app = Flask(__name__)
app.config['MQTT_BROKER_URL'] = 'mqtt.flespi.io'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'M3ExGxt4y2DmCkvN8CAqK0tYyUD4GLEgD9D7uV0TNt3dCoRAOfPo58brRCkncOrF'
app.config['MQTT_PASSWORD'] = ''
app.config['MQTT_REFRESH_TIME'] = 1.0 # refresh time in seconds
mqtt = Mqtt(app)

"""
basedir = os.path.abspath(os.path.dirname(__file__))

app.config['SQLALCHEMY_DATABASE_URI'] = \
		'sqlite:///' + os.path.join(basedir, 'database.db')
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)

class temphumi(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    time = db.Column(db.)
    topic = db.Column(db.Float, nullable=False)
    data = db.Column(db.Float, nullable=False)

    def __repr__(self):
        return f'<temphumi {self.firstname}>'
"""

def get_db_connection():
    conn = sqlite3.connect('sensors_data.db')
    conn.row_factory = sqlite3.Row
    return conn

@app.route('/')
def index():
    return render_template('home.html')
    
@app.route('/chart-data')
def chart_data():
    def get_database_data():
        while True:
            conn = get_db_connection()
            temp_data = conn.execute('SELECT created, data FROM temp_data \
                WHERE id = (SELECT MAX(id) FROM temp_data)').fetchone()
            humi_data = conn.execute('SELECT created, data FROM humi_data \
                WHERE id = (SELECT MAX(id) FROM humi_data)').fetchone()
            json_data = json.dumps(
                {'temp_time': temp_data['created'], 'temp_value': float(temp_data['data']), \
                'humi_time': humi_data['created'], 'humi_value': float(humi_data['data'])})
            yield f"data:{json_data}\n\n"
            time.sleep(10)

    response = Response(stream_with_context(get_database_data	()), mimetype="text/event-stream")
    response.headers["Cache-Control"] = "no-cache"
    response.headers["X-Accel-Buffering"] = "no"
    return response

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
	mqtt.subscribe('Temp')
	mqtt.subscribe('Humi')

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    conn = sqlite3.connect('sensors_data.db')
    cursor = conn.cursor()
    if message.topic == 'Temp':
        cursor.execute("INSERT INTO temp_data (data) VALUES (?)",
                (message.payload.decode(),)
                )
    elif message.topic == 'Humi':
        cursor.execute("INSERT INTO humi_data (data) VALUES (?)",
                (message.payload.decode(),)
                )
    conn.commit()
    cursor.close()
    conn.close()

if __name__ == '__main__':
	app.run(debug=False, port=5000, host='0.0.0.0')