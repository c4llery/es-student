#include "pi-task.h"
#include "pico/stdlib.h"
#include <stdio.h>

bool pi_task_start(uint32_t n)
{
    if (pi_task_state != PI_RUNNING)
    {
        pi_n = n;
        pi_k = 0;
        pi_sum = 0.0;
        pi_sign = 1.0;
        pi_task_set_state(PI_RUNNING);
        return true;
    }
    return false;
}

void pi_task_handle(void)
{
    switch (pi_task_state)
    {
    case PI_NOT_LAUNCHED:
        break;
    case PI_RUNNING:
        count_portion();
        break;
    case PI_READY:
        break;
    }
}

static void count_portion(void)
{
    // Очередная порция членов ряда, но не дальше последнего
    uint64_t iter_start = time_us_64();
    int end = pi_k + PI_TERMS_PER_CALL;
    if (end > pi_n)
    {
        end = pi_n;
    }

    for (; pi_k < end; pi_k++)
    {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k == pi_n)
    {
        pi_result = 4.0 * pi_sum;
        pi_task_set_state(PI_READY);
    }
    compute_time_us += (time_us_64() - iter_start); // проверить!! что-то в курсе было про это
}

// Готов ли результат: если да, записать его по указателю
bool pi_task_get_result(double *result, uint64_t *exec_time)
{   
    if (pi_task_state != PI_READY)
    {
        return false;
    }
    *result = pi_result;
    *exec_time = compute_time_us;
    return true;
}

pi_task_state_t pi_task_get_state(void)
{
    return pi_task_state;
}

void pi_task_set_state(pi_task_state_t state)
{
    pi_task_state = state;
}