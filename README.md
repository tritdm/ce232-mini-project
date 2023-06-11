# The wireless embedded system design mini project
## Model of project
![alt](/readme_folder/model.png)
## ESP32 nodes task
* Collect temperature and humidity data from sensor.
* Show data on LCD.
* Send data to MQTT Broker every 10 seconds.
## Server task
* Receive data from MQTT Broker.
* Save data to SQLite.
* Access from LAN.
## End users (or web) task
* Show data in graph mode and web auto update without refresh.
# How to operate this project
## Activate or set up a virtual environment and install neccessary packet
* If you want to have an own virtual environment
```
$ python3 -m venv venv
$ source venv/bin/activate
$ python3 -m pip install -r requirements.txt
```
* Or you can use my virtual environment
```
$ source tutorial-env/bin/activate
```
## Flash code to ESP32 and keep 
* Change source code to fit your need.
* Go to [esp32 source code folder](/esp32_source/) and read [README.md](/esp32_source/README.md) for more infomation.
## Deploy Flask app
* Go to [app](/app/) folder
```
$ cd app
```
* Initialize database or you can change to fit your need in [sql](/app/sensors_data.sql) file.
```
$ python init_db.py
```
* Run `Flask` app
```
$ python app.py
```
![Flask](/readme_folder/flask.png)
* You can use http://127.0.0.1:5000 to access from your pc/laptop and http://172.17.22.73:5000 to access from your LAN device.
* If you want your Flask app can access over Internet, you cant use `ngrok`.
* You can go to the [ngrok main page](https://ngrok.com/) for more information.
* By the way opening a parallel terminal and activate your virtual environment and run `ngrok` corresponding Flask app port.
```
$ source tutorial-env/bin/activate
$ ngrok http 5000
```
* You can use the `Forwarding` address to access from any network.

![ngrok](/readme_folder/ngrok.png)