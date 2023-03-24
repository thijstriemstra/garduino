/*  Copyright (c) 2023, Collab
 *  All rights reserved
*/

#ifndef images_h
#define images_h

// https://fontawesome.com/icons/temperature-three-quarters
#define temperature_three_quarters_width 15
#define temperature_three_quarters_height 26
static unsigned char temperature_three_quarters_bits[] = {
   0xe0, 0x01, 0xf0, 0x07, 0xf8, 0x0f, 0x7c, 0x0f, 0x1c, 0x1e, 0x1c, 0x1c,
   0x1c, 0x1c, 0xdc, 0x1c, 0xdc, 0x1c, 0xdc, 0x1c, 0xdc, 0x1c, 0xdc, 0x1c,
   0xdc, 0x1c, 0xdc, 0x1c, 0xde, 0x1c, 0xce, 0x3c, 0xcf, 0x38, 0xe7, 0x39,
   0xe7, 0x3b, 0xe7, 0x39, 0xcf, 0x39, 0x1e, 0x3c, 0x3e, 0x1f, 0xfc, 0x0f,
   0xf8, 0x07, 0xe0, 0x01 };

// https://fontawesome.com/icons/clock
#define clock_width 24
#define clock_height 24
static unsigned char clock_bits[] = {
   0x00, 0x7e, 0x00, 0xc0, 0xff, 0x03, 0xe0, 0xc3, 0x07, 0x70, 0x00, 0x0e,
   0x38, 0x00, 0x1c, 0x1c, 0x18, 0x38, 0x0e, 0x18, 0x70, 0x06, 0x18, 0x60,
   0x06, 0x18, 0x60, 0x07, 0x18, 0xe0, 0x03, 0x18, 0xc0, 0x03, 0x18, 0xc0,
   0x03, 0x78, 0xc0, 0x03, 0xf0, 0xc0, 0x07, 0xc0, 0xe1, 0x06, 0x80, 0x61,
   0x06, 0x00, 0x60, 0x0e, 0x00, 0x70, 0x1c, 0x00, 0x38, 0x38, 0x00, 0x1c,
   0x70, 0x00, 0x0e, 0xe0, 0xc3, 0x07, 0xc0, 0xff, 0x03, 0x00, 0x7e, 0x00 };


// https://fontawesome.com/icons/droplet
#define droplet_width 19
#define droplet_height 26
static unsigned char droplet_bits[] = {
   0x00, 0x02, 0x00, 0x00, 0x07, 0x00, 0x80, 0x0f, 0x00, 0xc0, 0x1f, 0x00,
   0xc0, 0x1f, 0x00, 0xe0, 0x3f, 0x00, 0xf0, 0x7f, 0x00, 0xf0, 0x7f, 0x00,
   0xf8, 0xff, 0x00, 0xfc, 0xff, 0x01, 0xfc, 0xff, 0x01, 0xfe, 0xff, 0x03,
   0xfe, 0xff, 0x03, 0xfe, 0xff, 0x03, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07,
   0xf7, 0xff, 0x07, 0xe7, 0xff, 0x07, 0xe7, 0xff, 0x07, 0xce, 0xff, 0x03,
   0x8e, 0xff, 0x03, 0x3c, 0xfe, 0x01, 0xfc, 0xff, 0x01, 0xf0, 0xff, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0x07, 0x00 };

// https://fontawesome.com/icons/shower
#define shower_width 24
#define shower_height 22
static unsigned char shower_bits[] = {
   0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0xfe, 0x1f, 0x00, 0xff, 0xff, 0x00,
   0xc7, 0xff, 0x00, 0xc7, 0x7f, 0x00, 0xc7, 0x3f, 0x00, 0xc7, 0x1f, 0x06,
   0xc7, 0x0f, 0xe6, 0x87, 0x07, 0xe0, 0x87, 0xc3, 0xe0, 0x87, 0xc1, 0x1c,
   0x07, 0x00, 0x1c, 0x07, 0x18, 0x1c, 0x07, 0x98, 0x03, 0x07, 0x80, 0x03,
   0x07, 0x80, 0x03, 0x07, 0x70, 0x00, 0x07, 0x70, 0x00, 0x07, 0x70, 0x00,
   0x07, 0x00, 0x00, 0x00, 0x00, 0x00 };

// https://fontawesome.com/icons/wifi
#define wifi_width 25
#define wifi_height 20
static unsigned char wifi_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0xc0, 0xff, 0x07, 0x00,
   0xf0, 0xff, 0x1f, 0x00, 0xfc, 0x00, 0x7e, 0x00, 0x3e, 0x00, 0xf8, 0x00,
   0x0f, 0x00, 0xe0, 0x01, 0x07, 0x00, 0xc0, 0x01, 0x00, 0xff, 0x01, 0x00,
   0xc0, 0xff, 0x07, 0x00, 0xe0, 0x83, 0x0f, 0x00, 0xe0, 0x00, 0x0e, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00,
   0x00, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00,
   0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// https://fontawesome.com/icons/sun
#define sun_width 26
#define sun_height 26
static unsigned char sun_bits[] = {
   0x80, 0x01, 0x06, 0x00, 0x80, 0x03, 0x07, 0x00, 0x80, 0xcf, 0x07, 0x00,
   0x80, 0xff, 0x07, 0x00, 0xc0, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0x0f, 0x00,
   0xf0, 0xff, 0x3f, 0x00, 0xff, 0x03, 0xff, 0x03, 0xff, 0x78, 0xfc, 0x03,
   0xfe, 0xfc, 0xfc, 0x01, 0x7c, 0xfe, 0xf9, 0x00, 0x7c, 0xff, 0xfb, 0x00,
   0x78, 0xff, 0x7b, 0x00, 0x78, 0xff, 0x7b, 0x00, 0x7c, 0xff, 0xfb, 0x00,
   0x7c, 0xfe, 0xf9, 0x00, 0xfe, 0xfc, 0xfc, 0x01, 0xff, 0x78, 0xfc, 0x03,
   0xff, 0x03, 0xff, 0x03, 0xf0, 0xff, 0x3f, 0x00, 0xc0, 0xff, 0x0f, 0x00,
   0xc0, 0xff, 0x0f, 0x00, 0x80, 0xff, 0x07, 0x00, 0x80, 0xcf, 0x07, 0x00,
   0x80, 0x03, 0x07, 0x00, 0x80, 0x01, 0x06, 0x00 };

// https://fontawesome.com/icons/cloud
#define cloud_width 28
#define cloud_height 22
static unsigned char cloud_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00,
   0xc0, 0xff, 0x00, 0x00, 0xc0, 0xff, 0x1d, 0x00, 0xe0, 0xff, 0x7f, 0x00,
   0xe0, 0xff, 0x7f, 0x00, 0xf0, 0xff, 0xff, 0x00, 0xf0, 0xff, 0xff, 0x00,
   0xf8, 0xff, 0xff, 0x00, 0xfc, 0xff, 0xff, 0x01, 0xfe, 0xff, 0xff, 0x03,
   0xfe, 0xff, 0xff, 0x07, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff, 0xff, 0x0f,
   0xff, 0xff, 0xff, 0x0f, 0xfe, 0xff, 0xff, 0x0f, 0xfe, 0xff, 0xff, 0x07,
   0xfc, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0x03, 0xe0, 0xff, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0x00 };

// https://fontawesome.com/icons/moon
#define moon_width 19
#define moon_height 24
static unsigned char moon_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0xe0, 0x0f, 0x00, 0xf0, 0x07, 0x00,
   0xf8, 0x03, 0x00, 0xfc, 0x01, 0x00, 0xfc, 0x01, 0x00, 0xfe, 0x00, 0x00,
   0xfe, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00,
   0xff, 0x00, 0x00, 0xff, 0x01, 0x00, 0xff, 0x01, 0x00, 0xfe, 0x03, 0x00,
   0xfe, 0x07, 0x00, 0xfc, 0x0f, 0x00, 0xfc, 0x7f, 0x00, 0xf8, 0xff, 0x07,
   0xf0, 0xff, 0x03, 0xe0, 0xff, 0x01, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00 };

// https://www.flaticon.com/free-icons/plant
#define soil_width 31
#define soil_height 26
static unsigned char soil_bits[] = {
   0x07, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x00,
   0xdc, 0x03, 0xe0, 0x03, 0xbc, 0x07, 0xf0, 0x3f, 0x78, 0x07, 0xf8, 0x3f,
   0xf0, 0x04, 0x1c, 0x1f, 0xe0, 0x05, 0xc4, 0x0f, 0xc0, 0x03, 0xf8, 0x07,
   0x00, 0x04, 0xfe, 0x01, 0x00, 0x08, 0x01, 0x00, 0x00, 0x88, 0x00, 0x00,
   0x00, 0x90, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00,
   0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
   0x00, 0xfc, 0x01, 0x00, 0x00, 0xff, 0x01, 0x00, 0xc0, 0xff, 0x0f, 0x00,
   0xc0, 0xff, 0x1f, 0x00, 0xf0, 0xff, 0x3f, 0x00, 0xf8, 0xff, 0x7f, 0x00,
   0xf8, 0xff, 0x7f, 0x00, 0xf0, 0xff, 0x3f, 0x00 };

// Font: Antihero
#define logo_width 128
#define logo_height 32
static unsigned char logo_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
   0x00, 0xe0, 0x03, 0x00, 0x3e, 0x0c, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
   0x0f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x3f, 0x1c, 0x00, 0xde,
   0x07, 0x00, 0x00, 0x00, 0x1f, 0x00, 0xc0, 0x01, 0x00, 0xfc, 0x1f, 0x80,
   0x3f, 0xfe, 0x3f, 0xfe, 0x1f, 0x1e, 0x00, 0x06, 0x1f, 0x38, 0xf0, 0x01,
   0x00, 0xfe, 0x1f, 0xc0, 0x1f, 0xfe, 0x7f, 0xfe, 0x3f, 0x1e, 0x1e, 0x1e,
   0x3f, 0x78, 0xf0, 0x07, 0x80, 0x7f, 0x00, 0xe0, 0x1f, 0xff, 0xff, 0xff,
   0x38, 0x1f, 0x1f, 0x1e, 0x3f, 0x78, 0xfc, 0x1f, 0xc0, 0x1f, 0x00, 0xe0,
   0x8f, 0xff, 0xfd, 0x3f, 0x78, 0x1f, 0x3f, 0x9f, 0x3f, 0x78, 0xfe, 0x3f,
   0xe0, 0x0f, 0x1e, 0xf0, 0xcf, 0x3f, 0xfe, 0x1f, 0xb8, 0x1f, 0x1f, 0x9f,
   0x3f, 0x7c, 0x3e, 0x78, 0xf0, 0xc7, 0xff, 0xf8, 0xc7, 0xcf, 0xff, 0x0f,
   0xfc, 0x8f, 0x1f, 0x9f, 0x1f, 0x3c, 0x1f, 0x70, 0xf8, 0xf1, 0xff, 0xf9,
   0xc7, 0xff, 0xff, 0x0f, 0xfc, 0x8f, 0x9f, 0xdf, 0x1f, 0x3e, 0x0f, 0x78,
   0xf8, 0xf9, 0xff, 0xfd, 0xc7, 0xff, 0xdf, 0x07, 0xbe, 0x8f, 0x8f, 0xcf,
   0x1f, 0xbe, 0x0f, 0x78, 0xfc, 0xf0, 0x0f, 0xfe, 0xc3, 0xff, 0xc3, 0x07,
   0xdf, 0xc7, 0x8f, 0xcf, 0x3f, 0x9e, 0x07, 0x78, 0x7e, 0xe0, 0x0f, 0xdf,
   0xe3, 0xff, 0xe0, 0x83, 0xdf, 0xe7, 0xc7, 0xef, 0x3f, 0xdf, 0x07, 0x7c,
   0x3e, 0xe0, 0x07, 0xef, 0xe3, 0x7f, 0xe0, 0xc3, 0xcf, 0xe7, 0xc7, 0xe7,
   0x3b, 0xdf, 0x07, 0x3e, 0x1f, 0xf0, 0x83, 0xff, 0xe3, 0x7d, 0xe0, 0xf1,
   0xc7, 0xf7, 0xc3, 0xe7, 0x39, 0xcf, 0x03, 0x3f, 0x1f, 0xfc, 0xc3, 0xff,
   0xf7, 0xf9, 0xe0, 0xf9, 0xc3, 0xff, 0xe1, 0xe3, 0x31, 0x8f, 0x83, 0x1f,
   0x0e, 0xff, 0xe1, 0xff, 0xf1, 0xf8, 0xe0, 0xff, 0x80, 0xff, 0xe0, 0xf3,
   0xf1, 0x8f, 0xc3, 0x1f, 0xce, 0xff, 0xe0, 0xe7, 0xf1, 0xf0, 0xe1, 0x7f,
   0x80, 0x7f, 0xe0, 0xf1, 0xe0, 0x07, 0xf7, 0x0f, 0xfe, 0xff, 0xe0, 0xe1,
   0xf1, 0xe0, 0xf1, 0x3f, 0x00, 0x7f, 0xe0, 0xf1, 0xc0, 0x07, 0xff, 0x07,
   0xfc, 0x7f, 0x00, 0xc0, 0x71, 0xc0, 0xe3, 0x0f, 0x00, 0x1e, 0xe0, 0xe1,
   0x00, 0x07, 0xfe, 0x03, 0xf8, 0x39, 0x00, 0xc0, 0x70, 0x80, 0x07, 0x00,
   0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x70, 0x38, 0x00, 0x00,
   0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#endif