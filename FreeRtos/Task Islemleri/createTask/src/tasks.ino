#include <stdio.h>
#include <freertos/task.h>

#define P(a) Serial.print(a)
#define PN(a) Serial.println(a)

char pcWriteBuffer[1024];

StaticTask_t pxTaskBuffer;
StackType_t puxStackBuffer[1000];

StaticTask_t pxTaskBuffer2;
StackType_t puxStackBuffer2[1000];

void vMyTask(void * parameters)
{
    while(1){
        PN((char *) parameters);
        vTaskDelay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
    xTaskCreate(
        vMyTask,
        "vMyTask",
        1000,
        (void *) "Task Create",
        10,
        NULL);
    xTaskCreatePinnedToCore(
        vMyTask,
        "vMyTask",
        1000,
        (void *) "Task Pinned",
        9,
        NULL,
        0);
    xTaskCreateStatic(
        vMyTask,
        "vMyTask",
        1000,
        (void *) "Task Static",
        9,
        puxStackBuffer2,
        &pxTaskBuffer2);

    xTaskCreateStaticPinnedToCore(
        vMyTask,
        "vMyTask",
        1000,
        (void *) "Task Static Pinned",
        7,
        puxStackBuffer,
        &pxTaskBuffer,
        0);

}

void loop()
{

}
