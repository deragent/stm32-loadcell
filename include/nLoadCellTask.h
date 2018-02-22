#ifndef N_LOAD_CELL_TASK_H
#define N_LOAD_CELL_TASK_H

#include "HX711Interface.h"

#include "FreeRTOS.h"
#include "queue.h"

QueueHandle_t cmdQueue;

void nLoadCellTaskInit();

#endif
