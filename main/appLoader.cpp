/*
*/
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "sdkconfig.h"

#include "lib/Blinker/Blinker.h"
#include "lib/mQttClient/mQttClient.h"
#include "lib/NVS/NVS.h"
#include "lib/UpdateFirmware/UpdateFirmware.h"
#include "lib/WiFi/WiFi.h"

#include "appConfig.h"

#define TAG "appLoader"

extern "C" {
    void app_main();
}

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

static esp_err_t mQttCommandCBfunction(esp_mqtt_event_handle_t event, void *cx)
{
	UpdateFirmware *UpFwr = UpdateFirmware::getInstance();

	char Command[256+1];
	char Parameter[256+1];
	char *ptr = event->data;
	int i = 0;

	while(i<event->data_len && i<256) {
		Command[i] = ptr[i];
		if(Command[i] == '?') break;
		i++;
	}
	Command[i++] = '\0';
	int j = 0;
	while(i<event->data_len && j<256) {
		Parameter[j++] = ptr[i++];
	}
	Parameter[j] = '\0';

	if(strcmp(Command, "DOWNLOAD") == 0) {
		esp_err_t err = UpFwr->Update(Parameter, (const char *)server_cert_pem_start);
		return(err);
	}
	if(strcmp(Command, "UPDATE") == 0) {
		if (UpFwr->SwitchToLoader() == ESP_OK) {
			UpFwr->Restart();
		}
		return(ESP_OK);
	}
	if(strcmp(Command, "ROLLBACK") == 0) {
		if (UpFwr->SwitchToApplication() == ESP_OK) {
			UpFwr->Restart();
		}
		return(ESP_OK);
	}
	return ESP_OK;
}

void app_main()
{
	esp_log_level_set("*", ESP_LOG_DEBUG);

    char ssid[WIFI_LEN_SSID+1];
    char password[WIFI_LEN_PASSWORD+1];
    char broker[MQTTCL_LEN_BROKER];
    char station[MQTTCL_LEN_STATION];

    Blinker	*blkLed = Blinker::getInstance();
    blkLed->SetPin(PIN_BOARD_LED);
    blkLed->SetPat(BKS_BOOT);
    blkLed->StartBlink();

    ESP_LOGI(TAG,"Application Loader!");

    NVS *nvs = NVS::getInstance();
    if (!nvs->Init(DATA_PARTITION_NAME, DEVICE_PARTITION_NAME)) {
    	ESP_LOGE(TAG, "Error to Init the NVS memory !");
    	vTaskDelay( 15000 / portTICK_PERIOD_MS ); // waits for one second
    	abort();
    }
    if( nvs->rStr_Dev("wifi_ssd",ssid,WIFI_LEN_SSID) != ESP_OK ) strcpy(ssid,WIFI_SSD);
    if( nvs->rStr_Dev("wifi_passwd",password,WIFI_LEN_PASSWORD) != ESP_OK ) strcpy(password,WIFI_PASSWORD);
    if( nvs->rStr_Dev("mqtt_broker_uri",broker,MQTTCL_LEN_BROKER) != ESP_OK ) strcpy(broker,MQTTCL_BROKER_URL);
    if( nvs->rStr_Dev("device_name",station,MQTTCL_LEN_STATION) != ESP_OK ) strcpy(station,MQTTCL_STATION_NAME);

    WiFi  *wifi = WiFi::getInstance();
    wifi->connectAP(ssid, password, WIFI_MODE_STA);

    UpdateFirmware *UpFwr = UpdateFirmware::getInstance();

    mQttClient 	  *mQtt = mQttClient::getInstance();
    mQtt->setBrokerUri("mqtt://172.20.42.42:1883");//broker);
    mQtt->setDeviceName("Test");//station);
    mQtt->init();

    while (mQtt->Publication("Application/Name", APPLICATIONNAME,0) == INVALID_PUBLICATION) {
    	vTaskDelay( 500 / portTICK_PERIOD_MS ); // waits for one second
    }
    mQtt->Publication("Application/Version",VERSION,0);
    mQtt->Subscribe("Command", mQttCommandCBfunction);


   	blkLed->SetPat(BKS_ACTIVE_OK);
    bool isFinishedTheJob = false;
	EventBits_t uxBits;
	const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS; // 10 sec timeout
    while(!isFinishedTheJob) {
    	uxBits = xEventGroupWaitBits(UpFwr->egUpdateFirmware,
    			UpdateFirmware::UPDFRW_DONE | UpdateFirmware::UPDFRW_ERROR | UpdateFirmware::UPDFRW_NOW,
				pdFALSE, pdFALSE, xTicksToWait);

    	if( (uxBits & (UpdateFirmware::UPDFRW_DONE)) == UpdateFirmware::UPDFRW_DONE) {
    		ESP_LOGI(TAG, "The firmware download succeeded !");
    		isFinishedTheJob = true;
    	} else if( (uxBits & (UpdateFirmware::UPDFRW_ERROR)) == UpdateFirmware::UPDFRW_ERROR) {
    		ESP_LOGE(TAG, "The firmware download returned an error !");
    		UpFwr->ResetAfterError();
    	} else if( (uxBits & (UpdateFirmware::UPDFRW_NOW)) == UpdateFirmware::UPDFRW_NOW) {
    		ESP_LOGD(TAG, "Download in progress...");
    		vTaskDelay( 1000 / portTICK_PERIOD_MS ); // waits for one second
    	} else {
    		ESP_LOGD(TAG, "Waiting for command...");
    	}
    }

    // --  Reboot the system in the application partition ---
    UpFwr->SwitchToApplication();
    UpFwr->Restart();
    while(true) {
		vTaskDelay( 1000 / portTICK_PERIOD_MS ); // waits for the reboot
    }

}
