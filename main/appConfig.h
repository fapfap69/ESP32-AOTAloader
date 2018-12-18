/*
 * appConfig.h
 *
 *  Created on: Nov 26, 2018
 *      Author: fap
 */

#ifndef MAIN_APPCONFIG_H_
#define MAIN_APPCONFIG_H_

#include "driver/gpio.h"

#define VERSION "0.1"
#define APPLICATIONNAME "appLoader"

#define DATA_PARTITION_NAME "nvs"
#define DEVICE_PARTITION_NAME "appldata"

#define WIFI_SSD	"Apple-fap"
#define WIFI_PASSWORD	"PaperinO"
#define WIFI_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#define PIN_BTN_BOOT	GPIO_NUM_0
#define PIN_BOARD_LED	GPIO_NUM_2

#define MQTTCL_BROKER_URL		"mqtt://192.168.178.26:1883"
#define MQTTCL_STATION_NAME	"Test"




#endif /* MAIN_APPCONFIG_H_ */
