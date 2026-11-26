#pragma once
#include "pico/stdlib.h"
#include <stdio.h>

void button_task_init(void);
void button_task_handle(void);

static uint num_presses = 0;
uint button_task_get_press_count(void);
bool button_task_is_pressed(void); 


void button_task_handle(void);