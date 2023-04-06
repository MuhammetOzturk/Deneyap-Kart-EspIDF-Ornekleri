# Bu notu kendim için oluşturdum ama umarım başkalarına da faydası olur.
# Paylaştığım örnek platformio'da kodlanmış ve çalıştırılmıştır. 
---
## Deneyap Kart'ta FreeRTOS Fonksiyonları
Deneyap kartı bir esp32 kartıdır.Çift çekirdeklidir.UART, I2C, SPI, I2S, Ethernet, SDIO gibi
bağlantı aryüzlerine sahiptir.Ben yazılım mühendisi olduğum için cihazın yazılımsal yetenekleri ile daha çok ilgileniyor olacağım.Buna yönelik
kendi yazdığım örnekleri paylaşacağım. 
#### Cihazın Teknik Özellikleri
(benim elimdeki cihazın özellikleri)
- **240 MHz Çift Çekirdek Tensilica LX6 Mikroişlemci**
- **520 KB SRAM**
- **448 KB ROM**
- **8 MB PSRAM**
- **4 MB SPI Flash**
- **2.4 GHz Wi-Fi – 802.11b/g/n**
- **Bluetooth EDR ve BLE**

---
### Görev Oluşturma
Arduino framework'ünde setup içersinde değişkenler ve fonksiyonlar ilklendirilirken 
loop içerisinde üstten aşağı sırasıyla gerçekleştirilir.Fakat gerçek dünya 
uygulamalrında görevlere her zaman doğrusal şekilde ihtiyaç duymayız.
Örneğin ekrana yazı yazdırırken diğer tarafta kritik veri gönderimi ile uğraşabiliriz.
Freertos bu işlemleri genelde önem sırasına göre kendisi zaman ve kaynak ayırarak bizim yerimize yapar.

#### Görev oluşturma fonksiyonları
Görev oluşturmak için **&lt;freertos/task.h&gt;** başlık dosyası içe aktarılmalıdır.   
Taskların kendilerine atanmış sonsuz döngüde devam eden bir görevleri vardır.  

---
 
**xTaskCreate :** Yeni bir görev oluşturur.
```c
static inline BaseType_t xTaskCreate(  
   TaskFunction_t pxTaskCode,  
   const char *const pcName,  
   const configSTACK_DEPTH_TYPE usStackDepth,  
   void *const pvParameters,  
   UBaseType_t uxPriority,  
   TaskHandle_t *const pxCreatedTask)
```
**pxTaskCode:** Bu parametre çalıştırılacak fonksiyon tanıtıcısıdır.  
**pcName:** "char" tipindeki bu değer log kayıtlarından hata ayıklama işlemleri
için kullanılır.  
**usStackDepth:** Task için başlangıçta ayrılacak bellek.  
**pvParameters:** Fonksiyona  göderilecek argümanlar.  
**uxPriority:** Gorevin önceliği.Değer büyüklüğüne göre öncelik artar.  
**pxCreatedTask:** Gorevin durumunu yönetmek için TaskHandle_t tipinde veri belirtilir.
Görev çalışıyor, hazır , engellendi ve durduruldu gibi durumlara sahip olabilir.
<center><img src="tskstate.gif" /></center>  

**Örnek**  

```c
#include <freertos/task.h>

#define P(a) Serial.print(a)
#define PN(a) Serial.println(a)

TaskHandle_t task_handle = NULL;

void EkranaMesaj(void * parameters)
{
    for(;;){
        PN("Task fonksiyonundan mesaj...");
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
    xTaskCreate(
        EkranaMesaj,
        "EkranaMesaj",
        1000,
        NULL,
        10,
        &task_handle);
    delay(5000);
    PN("Task durduruluyor...");
    vTaskSuspend(task_handle);
    delay(3000);
    PN("Task devam ettiriliyor...");
    vTaskResume(task_handle);
}

void loop()
{
    PN("Selam ben loop");
    delay(3000);
}
```  

---  

**xTaskCreatePinnedToCore:** Görevi çekirdeklerden birine sabitler.(Core0,Core1)
Görev sadece belirtilen çekirdekte çalıştırılır.  

```c
BaseType_t xTaskCreatePinnedToCore(
    TaskFunction_t pxTaskCode,
    const char *const pcName, 
    const configSTACK_DEPTH_TYPE usStackDepth, 
    void *const pvParameters, UBaseType_t uxPriority,
    TaskHandle_t *const pvCreatedTask,
    const BaseType_t xCoreID)
```  

**xCoreID:** Önceki fonksiyondan farklı olarak xCoreID değeri atanan çekirdeği belitir.
Bu bize kolay hata ayıklama ve yönetim imkanı sağlar.Yukarıda verilen örnekte 
görevi Core1'e atamak için aşağıdaki tanımlama kullanılabilir.
```c
    xTaskCreatePinnedToCore(
        EkranaMesaj,
        "EkranaMesaj",
        1000,
        NULL,
        10,
        &task_handle,
        1);
```  

---  

**xTaskCreateStatic:** Sabit bellek alanı üzerinde gorevi çalıştırır.
Bellek alanı yazar tarafından oluşturulur ve yönetilir.  

```c
static inline TaskHandle_t xTaskCreateStatic(
    TaskFunction_t pxTaskCode,
    const char *const pcName,
    const uint32_t ulStackDepth,
    void *const pvParameters,
    UBaseType_t uxPriority,
    StackType_t *const puxStackBuffer,
    StaticTask_t *const pxTaskBuffer)
```
**puxStackBuffer:** Yeni görev için tampon bellek.Bu alan sabittir.Bellek alanın
yönetimi yazara aittir.  
**pxTaskBuffer:** Görevin veri yapılarını saklar.Önceki örnek üzerinden devam edersekaşağıdaki gibi tanımlanabilir.  

```c
    StackType_t stackBuffer[1024];
    StaticTask_t staticTask;
    task_handler = xTaskCreateStatic(
                            EkranaMesaj,
                            "EkranaMesaj",
                            1000,
                            NULL,
                            10,
                            stackBuffer,
                            &staticTask);

```
---
**xTaskCreateStaticPinnedToCore:** Oluşturulacak göreve sabit bellek alanı ayırıp
çekirdeklerden birine sabitler.
```c
TaskHandle_t xTaskCreateStaticPinnedToCore(
    TaskFunction_t pxTaskCode,
    const char *const pcName,
    const uint32_t ulStackDepth,
    void *const pvParameters,
    UBaseType_t uxPriority,
    StackType_t *const pxStackBuffer,
    StaticTask_t *const pxTaskBuffer,
    const BaseType_t xCoreID)
```  

Yukarıdaki örenekten devam edilirse aşağıdaki gibi görev oluşturulur.  

```c
    StackType_t stackBuffer[1024];
    StaticTask_t staticTask;
    task_handler = xTaskCreateStatic(
                            EkranaMesaj,
                            "EkranaMesaj",
                            1000,
                            NULL,
                            10,
                            stackBuffer,
                            &staticTask,
                            1);

```  

---  
**vTaskDelete( TaskHandle_t task_handle ):** Görev tamamalandıktan sonra vTaskDelete(handle) fonksiyonu ile 
gorev silinir.Görev silinirken dikkatli olunmalı esp-idf dokumantasyonunda tavsiye 
edilen görev silme işlemlerini görevin içerisinden yapılmasıdır.Çünkü silme işlemi
görevin dışından çağrıldığında görevin yarıda kesilmesine neden olup veri kaybına 
neden olabilir.
Görevin içersinde vTaskDelete(NULL) parametresi verilerek silinebilir.
```c
    vTaskDelete(task_handle)
```  

Gorevin içiresinden görevi silme:
```c
void MyTask(void * arg){
    printf("Gorev Tamamlandı.");
    vTaskDelete(NULL);
}
```  

---  
**vTaskDelay:** İstenilen süre kadar bekletir.
```c  
vTaskDelay(1000)
```

---  
**vTaskDelayUntil:** İstenilen süre kadar bekletir.  

```c
BaseType_t xTaskDelayUntil(
    TickType_t *const pxPreviousWakeTime,
    const TickType_t xTimeIncrement)
```  

**pxPreviousWakeTime:** Başlangıç zamanı.  
**xTimeIncrement:** Başlangıç zamanında sonra geçmesi gereken süre.Esp-idf 
dokumantasyonunda bekletme için bu fonksiyon tavsiye edilir.
Bazı fonksiyonlar saat darbelerinin sayımını durdurduğu için vTaskDelay sayım 
yapamazken ,vTaskDelayUntil arada geçen zaman farkını kontrol eder.
Kullanım örneği  

```c  
TickType_t pxPreviousWakeTime = xTaskGetTickCount();
vTaskDelayUntil(&pxPreviousWakeTime, (TickType_t)1000);  
```  
---  
Şimdiye kadar anlatılanların toplam öreneği.  

```c

#include <stdio.h>
#include <freertos/task.h>

#define P(a) Serial.print(a)
#define PN(a) Serial.println(a)


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
```  

---  
