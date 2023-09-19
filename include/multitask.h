#include  "Multi_task.h"

// multitask config
TaskHandle_t statusT;
TaskHandle_t SensoresT;
//TaskHandle_t ModbusT;
TaskHandle_t servidorT;

void indicaciones(void *arg);
void servidor(void *arg);
void sensores(void *arg);
//void ModbusTransmission(void *arg);




void MultitaskInit() {
  xTaskCreatePinnedToCore(
                    sensores,     /* Task function. */
                    "servidorT",          /* name of task. */
                    10000,                /* Stack size of task */
                    NULL,                 /* parameter of the task */
                    1,                    /* priority of the task */
                    &servidorT,           /* Task handle to keep track of created task */
                    0);                   /* pin task to core 0 */                  
  delay(10); 


  xTaskCreatePinnedToCore(
                    servidor,     /* Task function. */
                    "SensoresT",          /* name of task. */
                    10000,                /* Stack size of task */
                    NULL,                 /* parameter of the task */
                    1,                    /* priority of the task */
                    &SensoresT,           /* Task handle to keep track of created task */
                    1);                   /* pin task to core 0 */                  
  delay(10); 

  // xTaskCreatePinnedToCore(
  //                   ModbusTransmission,     /* Task function. */
  //                   "ModbusT",          /* name of task. */
  //                   10000,                /* Stack size of task */
  //                   NULL,                 /* parameter of the task */
  //                   1,                    /* priority of the task */
  //                   &ModbusT,           /* Task handle to keep track of created task */
  //                   0);                   /* pin task to core 0 */                  
  // delay(10); 

  xTaskCreatePinnedToCore(
                    indicaciones,   /* Task function. */
                    "statusT",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &statusT,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
  delay(10); 
}






