#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------
  Durgapur Geographic Information
----------------------------------------------------------*/

#define CITY_LATITUDE      23.5204f
#define CITY_LONGITUDE     87.3119f

/*----------------------------------------------------------
  City Type
----------------------------------------------------------*/

#define CITY_IS_COASTAL          0.0f
#define CITY_IS_INDUSTRIAL       1.0f
#define CITY_IS_MILLION_PLUS     0.0f
#define CITY_IS_NON_ATTAINMENT   1.0f

/*----------------------------------------------------------
  City Tier (One-hot encoding)
----------------------------------------------------------*/

#define CITY_TIER_1      0.0f
#define CITY_TIER_2      1.0f
#define CITY_TIER_3      0.0f

/*----------------------------------------------------------
  Landmark Features

  These are fixed for the deployed location.

  Replace with actual percentages later if available.
----------------------------------------------------------*/

static const float DURGAPUR_LANDMARKS[42] =
{
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f
};

#ifdef __cplusplus
}
#endif