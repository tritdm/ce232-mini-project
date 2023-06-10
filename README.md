# The wireless embedded system design mini project
## Model of project
![alt](/readme_folder/model.png)
## ESP32 nodes task
* Collect temperature and humidity data from sensor.
* Show data on OLED SSD1306.
* Send data to MQTT Broker every 10 seconds.
## Server task
* Receive data from MQTT Broker.
* Save data to sqlite.
* Xây dựng web-app cho phép truy cập local (2đ)
## End users (or web) task
* Show data in graph mode and web auto update without refresh.