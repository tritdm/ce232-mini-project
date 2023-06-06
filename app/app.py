from flask import Flask, render_template
from flask_mqtt import Mqtt

app = Flask(__name__)
app.config['MQTT_BROKER_URL'] = 'mqtt.flespi.io'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'M3ExGxt4y2DmCkvN8CAqK0tYyUD4GLEgD9D7uV0TNt3dCoRAOfPo58brRCkncOrF'
app.config['MQTT_PASSWORD'] = ''
app.config['MQTT_REFRESH_TIME'] = 1.0 # refresh time in seconds
mqtt = Mqtt(app)

@app.route('/')
def index():
	return render_template('index.html')

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
    	return render_template('index.html', temperature=data['payload'])
    else: 
    	print("Humi = ", data['payload'])
    	return render_template('index.html', humidity=data['payload'])

if __name__ == '__main__':
	app.run(debug=True)
