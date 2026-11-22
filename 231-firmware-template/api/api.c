#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include <stdio.h>
#include "log.h"
#include "device.h"
#include <string.h>
#include "profiling.h"
#include "stdio-text-protocol.h"
#include "api.h"
#include <errno.h>
#include <ctype.h>
#include "led_task.h"

// Число без знака из десятичной записи. false — в записи не только цифры
// или число не помещается в uint32_t.
static bool parse_u32(const char *text, uint32_t *value)
{
    // strtoul приняла бы и знак: из "-1" получилось бы 4294967295
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }
    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    // после числа остались символы или число не поместилось
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
}

double calc_pi(uint terms)
{
    double sum = 0.0;
    double sign = 1.0;

    for (int k = 0; k < terms; k++) {
        // Прибавить очередной член ряда: ±1 / (2k + 1)
        sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }

    // Сумма ряда равна π/4
    return 4.0 * sum;
}

volatile double pi_result;
void cmd_calc_pi(const command_t * command)
{
    uint32_t pi_terms;
    if (command->argc != 1 || !parse_u32(command->argv[0], &pi_terms))
    {
        printf("error: usage calc_pi <pi_terms>\n");
        return;
    }
    uint64_t start_us = time_us_64();
    pi_result = calc_pi(pi_terms);
    uint64_t spent_us = time_us_64() - start_us;

    printf("pi: %.8f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}

void cmd_main_time_exec(const command_t * cmd)
{
    printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

void cmd_main_time_reset(const command_t * cmd)
{
    profiling_reset_max();
    printf("max reset\n");
}

void cmd_uptime(const command_t * cmd)
{
    printf("uptime: %.2f\n", time_us_64() / 1000.);
}

void cmd_info(const command_t * cmd)
{
    device_info();
}

void cmd_led_enable(const command_t *cmd)
{
    printf("led: on, period: %u ms\n", 2 * get_half_period());
    led_task_set_state(LED_STATE_ON);
}

void cmd_led_disable(const command_t *cmd)
{
    printf("led: off, period: %u ms\n", 2 * get_half_period());
    led_task_set_state(LED_STATE_OFF);
}

void cmd_led_blink(const command_t *cmd)
{
    printf("led: blink, period: %u ms\n", 2 * get_half_period());
    led_task_set_state(LED_STATE_BLINK);
}

void cmd_led_period(const command_t *command)
{
    uint32_t period;
    if (command->argc != 1 || !parse_u32(command->argv[0], &period))
    {
        printf("usage led_period <period_ms>")
        return;
    }
    else if (command->argv[0] <= 0)
    {
        printf("error: period_ms must be greater than 0");
        return;
    }
    led_task_set_period_ms(period);
    printf("new period: %u ms\n", period);
}

const api_command_t commands[] = {
    { "info", "device passport", cmd_info },
    { "calc_pi", "calculate pi: calc_pi [terms]", cmd_calc_pi},
    { "main_time_exec", "superloop iteration: average and maximum", cmd_main_time_exec }, 
    { "main_time_reset", "reset maximum iteration", cmd_main_time_reset },
    { "uptime", "time since reset", cmd_uptime },
    { "led_enable", "turn led on", cmd_led_enable},
    { "led_disable", "turn led off", cmd_led_disable},
    { "led_blink", "led blinking", cmd_led_blink},
    { "led_period", "set led period: [period_ms]", cmd_led_period},
};

const uint command_count = sizeof(commands) / sizeof(commands[0]);

void api_handle(const command_t *command)
{
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
    }
    else if (strcmp(command->name, "help") == 0)
    {
        printf("%-20s %s\n", "help", "list of commands");
        for (int i = 0; i < command_count; i++)
        {
            printf("%-20s %s\n", commands[i].name, commands[i].help);
        }
    }       
    else
    {
        bool found = 0;
        for (int i = 0; i < command_count; i++)
        {
            if (strcmp(command->name, commands[i].name) == 0)
            {
                found = 1;
                commands[i].callback(command);
            }
        }
        if (!found)
        {
            printf("error: unknown command '%s', try help\n", command->name);
        }
    }   
}
