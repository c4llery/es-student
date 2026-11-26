#pragma once
#include "pico/stdlib.h"
#include <stdio.h>

typedef enum
{
    BUTTON_PRESSED,
    BUTTON_UNPRESSED, 
    BUTTON_PRESSED_UNSURE,
    BUTTON_UNPRESSED_UNSURE
} button_state_t;

static button_state_t button_state;



void button_task_init(void);
void button_task_handle(void);

void button_task_set_state(button_state_t state);
button_state_t button_task_get_state(void);
static uint num_presses = 0;
uint button_task_get_press_count(void);
bool button_task_is_pressed(void); // я пока не знаю, что с этим делать  


void button_task_handle(void);