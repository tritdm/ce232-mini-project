import os
from flask import Flask, render_template
from flask_mqtt import Mqtt
from flask_sqlalchemy import SQLAlchemy
import sqlite3
import time
from sqlalchemy.sql import func

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
    conn = get_db_connection()
    temp_data = conn.execute('SELECT created, data FROM temp_data \
                WHERE id = (SELECT MAX(id) FROM temp_data)').fetchone()
    humi_data = conn.execute('SELECT created, data FROM humi_data \
                WHERE id = (SELECT MAX(id) FROM humi_data)').fetchone()
    conn.close()
    return render_template('home.html', temp_data=temp_data, humi_data=humi_data)

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
	mqtt.subscribe('Temp')
	mqtt.subscribe('Humi')

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(	
        topic=message.topic,
        payload=message.payload.decode()
    )
    conn = sqlite3.connect('sensors_data.db')
    cursor = conn.cursor()
    if data['topic'] == 'Temp':
        cursor.execute("INSERT INTO temp_data (data) VALUES (?)",
                (data['payload'],)
                )
    elif data['topic'] == 'Humi':
        cursor.execute("INSERT INTO humi_data (data) VALUES (?)",
                (data['payload'],)
                )
    conn.commit()
    cursor.close()
    conn.close()

if __name__ == '__main__':
	app.run(debug=True)