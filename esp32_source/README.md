# ESP32 Node
## Collect temperature and humidity data from sensor
* Using HDC1080 sensor (datasheet: [HDC1080_datasheet](https://www.ti.com/lit/ds/symlink/hdc1080.pdf)).
* Using [nikwest's library](https://github.com/nikwest/hdc1080-esp-idf).
## Show data on LCD
* Using OLED SSD1306 (datasheet: [SSD1306 datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)).
## Send data to MQTT Broker every 10 seconds
* Using `flespi.io` as MQTT Broker.
* Using [MQTT TCP example](https://github.com/espressif/esp-idf/tree/master/examples/protocols/mqtt/tcp).
* You need config WiFi interface/Example Connection Configuration before build and flash code.