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
* You can use http://127.0.0.1:5000 to access from your pc/laptop

![localhost](/readme_folder/localhost.png)

* Or use http://192.168.0.128:5000 (this is my IP, replace with your) to access from your LAN device.

![LAN](/readme_folder/LAN.png)
* If you want your Flask app can access over Internet, you can use `ngrok`.
* You can go to the [ngrok home](https://ngrok.com/) or [Miguel Grinberg's blog guide](https://blog.miguelgrinberg.com/post/access-localhost-from-your-phone-or-from-anywhere-in-the-world) for more information.
* By the way opening a parallel terminal and activate your virtual environment and run `ngrok` corresponding Flask app port.
```
$ source tutorial-env/bin/activate
$ ngrok http 5000
```
* You can use the `Forwarding address` to access from any network.

![ngrok terminal](/readme_folder/ngrok_terminal.png)

* You need accept to visit site

![ngrok accept](/readme_folder/ngrok_accept.png)

* The result

![ngrok](/readme_folder/ngrok.png)