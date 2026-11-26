#pragma once
#include "pico/stdlib.h"
#include <stdio.h>

// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 +110 = 585 тактов;
// 100 членов по 585 тактов при 125 МГц — около 4,7 мс
#define PI_TERMS_PER_CALL 100



typedef enum
{
PI_NOT_LAUNCHED,
PI_RUNNING,
PI_READY
} pi_task_state_t;

static uint32_t pi_k; // использование uint32_t, т.к граничные значения подходят и описаны в ТЗ
static uint32_t pi_n;
static double pi_sum;
static double pi_sign;
static double pi_result;
static uint64_t compute_time_us;
static pi_task_state_t pi_task_state;

bool pi_task_start(uint32_t n);
void pi_task_handle(void);
bool pi_task_get_result(double *result, uint64_t *exec_time_us);
void pi_task_set_state(pi_task_state_t state);
pi_task_state_t pi_task_get_state(void);
static void count_portion();
