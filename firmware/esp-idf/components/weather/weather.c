/******************************************************************************
 * File Name   : weather.c
 * Project     : PolluSense
 * Description :
 * Downloads weather data from Open-Meteo and stores the latest values.
 ******************************************************************************/

#include "weather.h"
#include "weather_parser.h"

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"

#include <string.h>

#define WEATHER_BUFFER_SIZE 2048

static char weather_buffer[WEATHER_BUFFER_SIZE];
static int weather_index = 0;

static weather_data_t latest_weather = {0};

static const char *TAG = "WEATHER";

/******************************************************************************
 * Open-Meteo URL
 ******************************************************************************/

#define WEATHER_URL \
"https://api.open-meteo.com/v1/forecast?" \
"latitude=23.5404352&" \
"longitude=87.2892225&" \
"current=temperature_2m,relative_humidity_2m,surface_pressure,wind_speed_10m,wind_direction_10m,rain"

/******************************************************************************
 * HTTP Event Handler
 ******************************************************************************/

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id)
    {
        case HTTP_EVENT_ON_CONNECTED:

            ESP_LOGI(TAG,"Connected");

            break;

        case HTTP_EVENT_HEADER_SENT:

            ESP_LOGI(TAG,"Header Sent");

            break;

        case HTTP_EVENT_ON_HEADER:

            ESP_LOGD(TAG,"%s : %s",
                     evt->header_key,
                     evt->header_value);

            break;

        case HTTP_EVENT_ON_DATA:

            if(weather_index + evt->data_len <
               WEATHER_BUFFER_SIZE - 1)
            {
                memcpy(

                    weather_buffer + weather_index,

                    evt->data,

                    evt->data_len

                );

                weather_index += evt->data_len;
            }

            break;

        case HTTP_EVENT_ON_FINISH:

            weather_buffer[weather_index] = '\0';

            ESP_LOGI(TAG,
                     "Received %d bytes",
                     weather_index);

            if(weather_parse_json(
                    weather_buffer,
                    &latest_weather))
            {
                ESP_LOGI(TAG,
                         "Weather parsed successfully");
            }
            else
            {
                ESP_LOGE(TAG,
                         "Weather parsing failed");
            }

            weather_index = 0;

            break;

        case HTTP_EVENT_DISCONNECTED:

            ESP_LOGI(TAG,
                     "Disconnected");

            break;

        default:

            break;
    }

    return ESP_OK;
}

/******************************************************************************
 * Initialize Weather Module
 ******************************************************************************/

esp_err_t weather_init(void)
{
    memset(&latest_weather,
           0,
           sizeof(weather_data_t));

    ESP_LOGI(TAG,
             "Weather module initialized");

    return ESP_OK;
}
/******************************************************************************
 * Download Latest Weather Data
 ******************************************************************************/

esp_err_t weather_update(void)
{
    ESP_LOGI(TAG,
             "Requesting weather data from Open-Meteo...");

    weather_index = 0;

    memset(weather_buffer,
           0,
           sizeof(weather_buffer));

    esp_http_client_config_t config =
    {
        .url = WEATHER_URL,

        .event_handler = http_event_handler,

        .transport_type = HTTP_TRANSPORT_OVER_SSL,

        .crt_bundle_attach = esp_crt_bundle_attach,

        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client =
        esp_http_client_init(&config);

    if(client == NULL)
    {
        ESP_LOGE(TAG,
                 "Failed to create HTTP client");

        return ESP_FAIL;
    }

    esp_err_t err =
        esp_http_client_perform(client);

    if(err == ESP_OK)
    {
        ESP_LOGI(TAG,
                 "HTTP Status = %d",
                 esp_http_client_get_status_code(client));

        ESP_LOGI(TAG,
                 "Content Length = %lld",
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG,
                 "Weather request failed : %s",
                 esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);

    if(latest_weather.valid)
    {
        ESP_LOGI(TAG, "=========== WEATHER ===========");

        ESP_LOGI(TAG,
                 "Temperature     : %.2f C",
                 latest_weather.temperature);

        ESP_LOGI(TAG,
                 "Humidity        : %.2f %%",
                 latest_weather.humidity);

        ESP_LOGI(TAG,
                 "Pressure        : %.2f hPa",
                 latest_weather.pressure);

        ESP_LOGI(TAG,
                 "Wind Speed      : %.2f m/s",
                 latest_weather.wind_speed);

        ESP_LOGI(TAG,
                 "Wind Direction  : %.2f deg",
                 latest_weather.wind_direction);

        ESP_LOGI(TAG,
                 "Rainfall        : %.2f mm",
                 latest_weather.rainfall);

        ESP_LOGI(TAG,
                 "Weather Valid   : %s",
                 latest_weather.valid ? "YES" : "NO");

        ESP_LOGI(TAG,
                 "===============================");
    }

    return err;
}

/******************************************************************************
 * Return Cached Weather Data
 ******************************************************************************/

weather_data_t weather_get_data(void)
{
    return latest_weather;
}