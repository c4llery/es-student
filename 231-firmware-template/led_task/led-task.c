#include "led-task.h"
#include "pico/stdlib.h"

uint led_task_get_period_ms(void)
{
    return half_period_ms * 2;
}

bool period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = time_us_64();

    if (now_us - *last_us < period_us) {
        return false;
    }

    // Следующее срабатывание отсчитывается от прошлого, а не от «сейчас»
    *last_us += period_us;

    // Отстали больше чем на период: не догонять, а начать отсчёт заново
    if (now_us - *last_us >= period_us) {
        *last_us = now_us;
    }

    return true;
}

void led_task_init()
{
    // настраиваем вывод светодиода
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    //период 1000 мс
    half_period_ms = 500;
    // состояние «мигает»
    led_task_set_state(LED_STATE_BLINK);

}

void led_task_handle(void)
{
    switch (led_state)
    {
    case LED_STATE_OFF:
    case LED_STATE_ON:
        // Светодиод уже установили, когда дали led_state_t state, делать нечего 
        break;
    case LED_STATE_BLINK:
        if (period_elapsed(&last_toggle_us, half_period_ms * 1000))
        {
            // меняем состояние на противоположное 
            gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
        }
    default:
        break;
    }
}

void led_task_set_state(led_state_t state)
{
    led_state = state;

    // Действие при входе: выполняется один раз, в момент перехода 
    switch (led_state)
    {
        case LED_STATE_OFF:
            // гасим светодиод
            gpio_put(PICO_DEFAULT_LED_PIN, false);
            break;
        case LED_STATE_ON:
            // зажигаем светодиод
            gpio_put(PICO_DEFAULT_LED_PIN, true);
            break;
        case LED_STATE_BLINK:
            // Первое включение - на ближайшей итерации 
            last_toggle_us = 0;
            break;
        default:
            break;
    }
}

led_state_t led_task_get_state(void)
{
    return led_state;
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    // С нулевым полупериодом светодиод не перключался бы, такое нельзя 
    if (period_ms == 0)
    {
        return false;
    }
    else
    {
        half_period_ms = (uint64_t)period_ms / 2;
        return true;
    }
}
