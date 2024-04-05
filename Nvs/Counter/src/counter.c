#include <stdio.h>
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <nvs.h>

esp_err_t err;
uint8_t counter;
nvs_handle_t handle;

void app_main(){
    vTaskDelay(500);
    
    err = nvs_flash_init();

    ESP_ERROR_CHECK( err );

    err = nvs_open("NVS area",NVS_READWRITE, &handle);

    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(handle, "restart_counter", &restart_counter);
    switch (err) {
        case ESP_OK:
            printf("Done\n");
            printf("Restart counter = %" PRIu32 "\n", restart_counter);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    nvs_set_i32(handle, "restart_counter", restart_counter + 1);
    nvs_commit(handle);
    nvs_close(handle);
    vTaskDelay(500);
    esp_restart();
 
}
