#include "button-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "led-task.h"

const uint debounce_time_us = 20000;

typedef enum
{
    BUTTON_PRESSED,
    BUTTON_UNPRESSED, 
    BUTTON_PRESSED_UNSURE,
    BUTTON_UNPRESSED_UNSURE
} button_state_t;

static button_state_t button_state;


const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;
static uint32_t previous_toggle_us = 0;
uint32_t now_us = 0;

static void button_task_set_state(button_state_t state)

{
    button_state = state;
}

void button_task_init(void)
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    button_task_set_state(BUTTON_UNPRESSED);
}


uint button_task_get_press_count(void)
{
    return num_presses;
}

bool button_task_is_pressed(void)
{
    return !gpio_get(BUTTON_PIN);
}

void button_task_handle(void)
{
    // читаем уровень: нажатая кнопка замыкает вывод на землю и даёт ноль
    bool state = !gpio_get(BUTTON_PIN);
    // читаем часы

    switch (button_state)
    {
    case BUTTON_UNPRESSED:
        if (state)
        {
            previous_toggle_us = time_us_32();
            button_task_set_state(BUTTON_PRESSED_UNSURE);
        }
        break;
    case BUTTON_PRESSED_UNSURE:
        now_us = time_us_32();
        if ((now_us - previous_toggle_us) >= debounce_time_us && state)
        {
            button_task_set_state(BUTTON_PRESSED);
            led_task_next_state();
            num_presses += 1;
        }
        else
        {
            button_task_set_state(BUTTON_UNPRESSED);
        }
        break;
    case BUTTON_PRESSED:
        if (!state)
        {
            previous_toggle_us = time_us_32();
            button_task_set_state(BUTTON_UNPRESSED_UNSURE);
        }
        break;

    case BUTTON_UNPRESSED_UNSURE:
        now_us = time_us_32();
        if ((now_us - previous_toggle_us) >= debounce_time_us && !state)
        {
            button_task_set_state(BUTTON_UNPRESSED);
        }
        else
        {
            button_task_set_state(BUTTON_PRESSED);
        }
        break;
    default:
        break;
    }
    
    // переходы по таблице; при переходе в «нажата»
    // увеличиваем счётчик и вызываем led_task_next_state()
}