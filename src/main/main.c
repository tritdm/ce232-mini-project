/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include <string.h>
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "font8x8_basic.h"
#include "ssd1306.h"
#include "HDC1080.h"
#include <math.h>

#define MQTT_BROKER_URI             "mqtt://mqtt.flespi.io"
#define MQTT_BROKER_PORT            1883
#define MQTT_BROKER_USERNAME        "M3ExGxt4y2DmCkvN8CAqK0tYyUD4GLEgD9D7uV0TNt3dCoRAOfPo58brRCkncOrF"
#define MQTT_BROKER_PASSWORD        ""
#define MQTT_BROKER_CLIENTID        "lop1nhom9"
#define TOPIC_1                     "Humi"
#define TOPIC_2                     "Temp"

#define _I2C_NUMBER(num)            I2C_NUM_##num
#define I2C_NUMBER(num)             _I2C_NUMBER(num)

#define I2C_MASTER_SCL_IO           4                                       /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           5                                       /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUMBER(0)                           /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          100000                                  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                                       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                                       /*!< I2C master doesn't need buffer */

static esp_mqtt_client_handle_t client;

static const char *TAG = "MINI_PROJECT";

//dht_sensor_type_t DHT11;

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode               = I2C_MODE_MASTER,
        .sda_io_num         = I2C_MASTER_SDA_IO,
        .sda_pullup_en      = GPIO_PULLUP_ENABLE,
        .scl_io_num         = I2C_MASTER_SCL_IO,
        .scl_pullup_en      = GPIO_PULLUP_ENABLE,
        .master.clk_speed   = I2C_MASTER_FREQ_HZ,
        .clk_flags          = I2C_SCLK_SRC_FLAG_FOR_NOMAL,          
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) 
    {
        return err;
    }
	ESP_LOGI(TAG, "I2C configured successfully");
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static esp_err_t ssd1306_init(void)
{
    esp_err_t espRc;

	i2c_cmd_handle_t cmd;
	cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

	i2c_master_write_byte(cmd, OLED_CMD_SET_CHARGE_PUMP, true);
	i2c_master_write_byte(cmd, 0x14, true);

	i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP, true); // reverse left-right mapping
	i2c_master_write_byte(cmd, OLED_CMD_SET_COM_SCAN_MODE, true); // reverse up-bottom mapping

	i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_NORMAL, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_OFF, true);
	i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_ON, true);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
	if (espRc == ESP_OK) 
	{
		ESP_LOGI(TAG, "OLED configured successfully");
	} 
	else 
	{
		ESP_LOGE(TAG, "OLED configuration failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);

    return espRc;
}

static esp_err_t hdc1080_init()
{
    esp_err_t espRc;

	i2c_cmd_handle_t cmd;
	cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (HDC1080_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, REGISTER_ADDRESS, true);

	i2c_master_write_byte(cmd, CONFIG_MSB, true);
	i2c_master_write_byte(cmd, CONFIG_LSB, true);

	i2c_master_stop(cmd);

    espRc = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
	if (espRc == ESP_OK) 
	{
		ESP_LOGI(TAG, "HDC1080 configured successfully");
	} 
	else 
	{
		ESP_LOGE(TAG, "HDC1080 configuration failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);

    return espRc;
}

void hdc1080_read()
{
    uint8_t *humi = malloc(2 * sizeof(uint8_t));
    uint8_t *temp = malloc(2 * sizeof(uint8_t));
    esp_err_t espRc;
    i2c_cmd_handle_t cmd;
	cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (HDC1080_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, REGISTER_TRIGGER, true);

    i2c_master_stop(cmd);

    espRc = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
	if (espRc == ESP_OK) 
	{
		ESP_LOGI(TAG, "HDC1080 trigger successfully");
	} 
	else 
	{
		ESP_LOGE(TAG, "HDC1080 trigger failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_read(cmd, *humi, 2, true);
    i2c_master_read(cmd, *temp, 2, true);

    i2c_master_stop(cmd);

    espRc = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
	if (espRc == ESP_OK) 
	{
		ESP_LOGI(TAG, "HDC1080 read successfully");
	} 
	else 
	{
		ESP_LOGE(TAG, "HDC1080 read failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);

    float humi_infloat, temp_infloat;
    humi_infloat = (float)*humi * 100 / 65536;
    temp_infloat = (float)(*temp * 165 - 40) / 65536;
    printf("Humadity: %f\nTemperature: %f", humi_infloat, temp_infloat);
}

void task_ssd1306_display_text(const char *text) 
{
	//char *text = (char*)arg_text;
	uint8_t text_len = strlen(text);

	i2c_cmd_handle_t cmd;

	uint8_t cur_page = 0;

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
	i2c_master_write_byte(cmd, 0x00, true); 
	i2c_master_write_byte(cmd, 0x10, true);
	i2c_master_write_byte(cmd, 0xB0 | cur_page, true);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	for (uint8_t i = 0; i < text_len; i++) {
		if (text[i] == '\n') 
		{
			cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

			i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
			i2c_master_write_byte(cmd, 0x00, true); 
			i2c_master_write_byte(cmd, 0x10, true);
			i2c_master_write_byte(cmd, 0xB0 | ++cur_page, true); 

			i2c_master_stop(cmd);
			i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
			i2c_cmd_link_delete(cmd);
		} 
		else 
		{
			cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

			i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
			i2c_master_write(cmd, font8x8_basic_tr[(uint8_t)text[i]], 8, true);

			i2c_master_stop(cmd);
			i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
			i2c_cmd_link_delete(cmd);
		}
	}
	vTaskDelete(NULL);
}

void task_ssd1306_display_clear(void *ignore) 
{
	i2c_cmd_handle_t cmd;

	uint8_t clear[128];
	for (uint8_t i = 0; i < 128; i++) 
	{
		clear[i] = 0;
	}
	for (uint8_t i = 0; i < 8; i++) 
	{
		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
		i2c_master_write_byte(cmd, 0xB0 | i, true);

		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
		i2c_master_write(cmd, clear, 128, true);
		i2c_master_stop(cmd);
		i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);
	}

	vTaskDelete(NULL);
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_1, 1);
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_2, 1);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .broker.address.port = MQTT_BROKER_PORT,
        .credentials.username = MQTT_BROKER_USERNAME,
        .credentials.authentication.password = MQTT_BROKER_PASSWORD,
        .credentials.client_id = MQTT_BROKER_CLIENTID
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/*void read_DHT11_data()
{
    float *humi;
    float *temp;
    ESP_ERROR_CHECK(dht_read_float_data(DHT11, 6, humi, temp));
    printf("Humadity: %f\nTemperature: %f", *humi, *temp);
}*/

void sensor_read(float *a, float *b)
{
	*a = (*a) + 1;
	*b = (*b) + 1;
}

void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}

void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;
 
    // Extract floating part
    float fpart = n - (float)ipart;
 
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
 
    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot
 
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
 
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

char* oled_string(const char *humidity, const char *temperature)
{
	char humi[12] = "Humi: ";
	char temp[12] = "Temp: ";
	char* res = malloc(25);

	strcat(humi, humidity);
	strcat(temp, temperature);
	strcpy(res, humi);
	strcat(res, "\n");
	strcat(res, temp);
	
	return res;
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(i2c_master_init());
	ssd1306_init();
    //hdc1080_init();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	float humi = 10, temp = 12;
	while (1) 
	{
		//float humi, temp;
		char *humi_char = malloc(5);
		char *temp_char = malloc(5);
		int msg_id;

		sensor_read(&humi, &temp);

		ftoa(humi, humi_char, 2);
		msg_id = esp_mqtt_client_publish(client, TOPIC_1, humi_char, 0, 0, 0);
        ESP_LOGI(TAG, "sent humi successful, msg_id=%d", msg_id);

		vTaskDelay(100 / portTICK_PERIOD_MS);

		ftoa(temp, temp_char, 2);
		msg_id = esp_mqtt_client_publish(client, TOPIC_2, temp_char, 0, 0, 0);
        ESP_LOGI(TAG, "sent temp successful, msg_id=%d", msg_id);

		char* oled_print = oled_string(humi_char, temp_char);

		xTaskCreate(task_ssd1306_display_clear, "display_clear", 1024 * 1, (void *)0, 10, NULL);
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		xTaskCreate(task_ssd1306_display_text, "display_text", 1024 * 1, oled_print, 10, NULL);
		vTaskDelay(5000 / portTICK_PERIOD_MS);

		free(oled_print);

		//vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
