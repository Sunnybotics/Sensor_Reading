#ifndef SCR_MULTI_TASK_H_
#define SCR_MULTI_TASK_H_

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include "core/core.h"
#endif



namespace bfs{

    class Multi_task{
        private: 
            TaskHandle_t LecturaRF;
            TaskHandle_t LecturaSensores;
            TaskHandle_t Motores12;
            TaskHandle_t MotoresEscobillas;


        public:
            #if defined(ESP32)
            void Begin(void *args);
            #else
            void Begin();
            #endif
            void LecturaControlRF(void *arg)
            {

            };
            void LecturaSensoresDistancia(void *arg)
            {

            };
            void MovimientoMotores12(void *arg)
            {

            };
            void MovimientoMotoresEscobillas(void *arg)
            {

            };



    };
}  // namespace bfs
#endif