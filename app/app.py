import os
from flask import Flask, render_template
from flask_mqtt import Mqtt
from flask_sqlalchemy import SQLAlchemy

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
    temperature = db.Column(db.Float, nullable=False)
    humidity = db.Column(db.Float, nullable=False)

    def __repr__(self):
        return f'<temphumi {self.firstname}>'
"""
temp = 0
humi = 2
@app.route('/', methods=["POST","GET"])
def index():
    if request.method =='POST':
        temp = temp + 1
        humi = humi + 3
	    return render_template('home.html', temperature=temp, humidity=humi)

"""
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
    if data['topic'] == 'Temp': 
    	print("Temp = ", data['payload'])
    elif data['topic'] == 'Humi': 
    	print("Humi = ", data['payload'])
"""

if __name__ == '__main__':
	app.run(debug=True)