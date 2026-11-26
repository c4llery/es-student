// led-task.h — задача светодиода: выключен, горит или мигает
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include <stdio.h>

typedef enum {
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINK,
} led_state_t;

void led_task_init(void);
void led_task_handle(void);

void led_task_set_state(led_state_t state);
led_state_t led_task_get_state(void);

// false — период не принят
bool led_task_set_period_ms(uint32_t period_ms);

// это внутренняя функция 
static bool period_elapsed(uint64_t *last_us, uint64_t period_us);

uint led_task_get_period_ms(void);

void led_task_next_state(void);
static led_state_t led_state;
static uint64_t last_toggle_us = 0;
static uint half_period_ms = 0;
