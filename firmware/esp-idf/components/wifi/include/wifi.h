#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_init(void);

bool wifi_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif