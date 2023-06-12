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

#include "driver/i2c.h"
#include "sdkconfig.h"
#include "hdc1080.h"
#include "i2c_init.h"
#include "oled_string_process.h"
#include "oled_task.h"

#define MQTT_BROKER_URI             "mqtt://mqtt.flespi.io"
#define MQTT_BROKER_PORT            1883
#define MQTT_BROKER_USERNAME        "M3ExGxt4y2DmCkvN8CAqK0tYyUD4GLEgD9D7uV0TNt3dCoRAOfPo58brRCkncOrF"
#define MQTT_BROKER_PASSWORD        ""
#define MQTT_BROKER_CLIENTID        "lop1nhom9"
#define TOPIC_1                     "Temp"
#define TOPIC_2                     "Humi"

static esp_mqtt_client_handle_t client;
hdc1080_registers_t *sensor;

const char *TAG = "MINI_PROJECT";

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

    i2c_master_init();
	ssd1306_init();
    sensor = hdc1080_init_sensor(I2C_MASTER_NUM, HDC1080_ADDR);

	if (sensor)
    {
        hdc1080_registers_t registers = hdc1080_get_registers(sensor);
        ESP_LOGI(TAG, "Initialized HDC1080 sensor");
        registers.acquisition_mode = 1;
        
        hdc1080_set_registers(sensor, registers);
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
	vTaskDelay(100 / portTICK_PERIOD_MS);

	while (1) 
	{
		float humi, temp;
		char *humi_char = malloc(5);
		char *temp_char = malloc(5);
		int msg_id;

		hdc1080_read(sensor, &temp, &humi);

		ftoa(temp, temp_char, 2);
		msg_id = esp_mqtt_client_publish(client, TOPIC_1, temp_char, 0, 0, 0);
        ESP_LOGI(TAG, "sent temp successful, msg_id=%d", msg_id);

		vTaskDelay(100 / portTICK_PERIOD_MS);

		ftoa(humi, humi_char, 2);
		msg_id = esp_mqtt_client_publish(client, TOPIC_2, humi_char, 0, 0, 0);
        ESP_LOGI(TAG, "sent humi successful, msg_id=%d", msg_id);

		char* oled_print = oled_string(temp_char, humi_char);

		xTaskCreate(task_ssd1306_display_clear, "display_clear", 1024 * 1, (void *)0, 10, NULL);
		vTaskDelay(100 / portTICK_PERIOD_MS);

		xTaskCreate(task_ssd1306_display_text, "display_text", 1024 * 1, (void *)oled_print, 10, NULL);
		vTaskDelay(10000 / portTICK_PERIOD_MS);

		free(temp_char);
		free(humi_char);
		free(oled_print);
	}
}