#ifndef WEATHER_PARSER_H
#define WEATHER_PARSER_H

#include <stdbool.h>
#include "weather.h"

bool weather_parse_json(const char *json,
                        weather_data_t *out);

#endif