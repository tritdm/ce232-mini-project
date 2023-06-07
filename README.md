# The wireless embedded system design mini project
## Model of project
![alt](/readme_folder/model.png)
## ESP32 nodes task
* Collect temperature and humidity data from sensor
* Show data on OLED SSD1306
* Send data to MQTT Broker every 10 seconds
## Server task
* Nhận dữ liệu (0.5đ)
* Lưu dữ liệu vào database
    * Lưu vào file TXT (1đ)
    * Lưu vào file Excel/CSV/Json (1.5đ)
    * Sử dụng cơ sở dữ liệu như SQL/MySQL/MongoDB/Redis/...(2đ)
* Xây dựng web-app cho phép truy cập local (2đ)
## End users (or web) task
* Hiển thị dữ liệu để người dùng xem
    * Hiển thị dạng số đơn giản
        * Người dùng tự bấm refresh để xem dữ liệu mới (1đ)
        * Web tự refresh khi có dữ liệu mới (1.5đ)
        * Web tự cập nhật dữ liệu mà không cần refresh (2đ)
    * Hiển thị dạng đồ thị
        * Người dụng tự bấm refresh để xem dữ liệu mới (1.5đ)
        * Web tự refresh khi có dữ liệu mới (2đ)
        * Web tự cập nhật dữ liệu mà không cần refresh (2.5đ)
* Thiết kế website đẹp (Cộng 1đ Bonus)