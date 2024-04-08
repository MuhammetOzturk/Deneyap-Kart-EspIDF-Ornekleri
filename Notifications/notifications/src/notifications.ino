#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define P(a) Serial.print(a)
#define PN(a) Serial.println(a)

TaskHandle_t taskHandle1;
TaskHandle_t taskHandle2;

void vMyTask1(void * parameters)
{
    while(1)
    {
        if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY) > 0)
        {
            PN("Isaret alindi");
        }
    }
}

 void vMyTask2(void * parameters)
 {
     while(1)
     {
        //xTaskNotifyGive(taskHandle1);
        xTaskNotify(taskHandle1,0,eIncrement);
        PN("Task1 icin bildirim gonderildi.");
        vTaskDelay(1000);
     }
 }
void setup()
{
    Serial.begin(115200);
    xTaskCreate(
        vMyTask1,
        "vMyTask1",
        1000,
        NULL,
        10,
        &taskHandle1);

    xTaskCreate(
        vMyTask2,
        "vMyTask2",
        1000,
        NULL,
        10,
        &taskHandle2);
}

void loop()
{

}
