/******************************************************************************
 * File Name   : weather_parser.c
 * Project     : PolluSense
 * Description :
 * Implements the JSON parser for the PolluSense weather module.
 *
 * This module is responsible for:
 *   - Parsing the JSON response received from the Open-Meteo API.
 *   - Extracting required weather parameters.
 *   - Converting JSON string values into floating-point numbers.
 *   - Validating that all required weather fields are present.
 ******************************************************************************/

#include "weather_parser.h"
#include "jsmn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Maximum number of JSON tokens supported by the parser */
#define TOKEN_COUNT 64

/******************************************************************************
 * Function    : jsoneq
 *
 * Description :
 * Compares a JSON token with a specified string.
 *
 * Parameters
 *      json : Complete JSON document.
 *      tok  : JSON token.
 *      s    : Target string.
 *
 * Returns
 *      1 if equal, otherwise 0.
 ******************************************************************************/
static int jsoneq(const char *json,
                  jsmntok_t *tok,
                  const char *s)
{
    /* Ensure the token represents a JSON string */
    if (tok->type != JSMN_STRING)
        return 0;

    /* Compare string lengths */
    if ((int)strlen(s) != tok->end - tok->start)
        return 0;

    /* Compare token contents */
    return (strncmp(json + tok->start,
                    s,
                    tok->end - tok->start) == 0);
}

/******************************************************************************
 * Function    : weather_parse_json
 *
 * Description :
 * Parses the weather JSON returned by the Open-Meteo server and extracts
 * the required environmental parameters.
 *
 * Extracted Parameters:
 *   - Temperature
 *   - Relative Humidity
 *   - Surface Pressure
 *   - Wind Speed
 *   - Wind Direction
 *   - Rainfall
 *
 * Parameters
 *      json : JSON response string.
 *      out  : Output weather structure.
 *
 * Returns
 *      true  -> Parsing successful.
 *      false -> Parsing failed.
 ******************************************************************************/
bool weather_parse_json(const char *json,
                        weather_data_t *out)
{
    /* JSON parser instance */
    jsmn_parser parser;

    /* Token array used by the JSMN parser */
    jsmntok_t tokens[TOKEN_COUNT];

    /* Initialize the JSON parser */
    jsmn_init(&parser);

    /* Parse the JSON document */
    int count = jsmn_parse(&parser,
                           json,
                           strlen(json),
                           tokens,
                           TOKEN_COUNT);

    /* Verify successful parsing */
    if (count < 0)
    {
        printf("JSON parse failed\n");
        return false;
    }

    /* Track whether every required field has been found */
    bool temp_found = false;
    bool hum_found = false;
    bool press_found = false;
    bool wind_found = false;
    bool direction_found = false;
    bool rain_found = false;

    /* Search every JSON token */
    for (int i = 1; i < count; i++)
    {
        /* ---------------- Temperature ---------------- */

        if (jsoneq(json,
                   &tokens[i],
                   "temperature_2m"))
        {
            char value[32];

            int len =
                tokens[i + 1].end - tokens[i + 1].start;

            /* Copy the JSON value */
            memcpy(value,
                   json + tokens[i + 1].start,
                   len);

            value[len] = 0;

            /* Convert string to floating-point value */
            out->temperature = atof(value);

            temp_found = true;

            i++;
        }

        /* ---------------- Humidity ---------------- */

        else if (jsoneq(json,
                        &tokens[i],
                        "relative_humidity_2m"))
        {
            char value[32];

            int len =
                tokens[i + 1].end - tokens[i + 1].start;

            memcpy(value,
                   json + tokens[i + 1].start,
                   len);

            value[len] = 0;

            out->humidity = atof(value);

            hum_found = true;

            i++;
        }

        /* ---------------- Pressure ---------------- */

        else if (jsoneq(json,
                        &tokens[i],
                        "surface_pressure"))
        {
            char value[32];

            int len =
                tokens[i + 1].end - tokens[i + 1].start;

            memcpy(value,
                   json + tokens[i + 1].start,
                   len);

            value[len] = 0;

            out->pressure = atof(value);

            press_found = true;

            i++;
        }

        /* ---------------- Wind Speed ---------------- */

        else if (jsoneq(json,
                        &tokens[i],
                        "wind_speed_10m"))
        {
            char value[32];

            int len =
                tokens[i + 1].end - tokens[i + 1].start;

            memcpy(value,
                json + tokens[i + 1].start,
                len);

            value[len] = 0;

            out->wind_speed = atof(value);

            wind_found = true;

            i++;
        }

        /* ---------------- Wind Direction ---------------- */

        else if (jsoneq(json,
                        &tokens[i],
                        "wind_direction_10m"))
        {
            char value[32];

            int len =
                tokens[i + 1].end - tokens[i + 1].start;

            memcpy(value,
                json + tokens[i + 1].start,
                len);

            value[len] = 0;

            out->wind_direction = atof(value);

            direction_found = true;

            i++;
        }

        /* ---------------- Rainfall ---------------- */

        else if (jsoneq(json,
                        &tokens[i],
                        "rain"))
        {
            char value[32];

            int len =
                tokens[i + 1].end - tokens[i + 1].start;

            memcpy(value,
                json + tokens[i + 1].start,
                len);

            value[len] = 0;

            out->rainfall = atof(value);

            rain_found = true;

            i++;
        }
    }

    /* Ensure every required weather field was successfully extracted */
    if (!(temp_found &&
            hum_found &&
            press_found &&
            wind_found &&
            direction_found &&
            rain_found))
    {
        printf("Weather fields missing\n");
        return false;
    }

    /* Display the parsed weather information */
    printf("\n");

    printf("Temperature   : %.2f C\n",
        out->temperature);

    printf("Humidity      : %.2f %%\n",
        out->humidity);

    printf("Pressure      : %.2f hPa\n",
        out->pressure);

    printf("Wind Speed    : %.2f km/h\n",
        out->wind_speed);

    printf("Wind Direction: %.2f deg\n",
        out->wind_direction);

    printf("Rain          : %.2f mm\n",
        out->rainfall);

    /* Mark the weather data as valid */
    out->valid = true;

    return true;
}