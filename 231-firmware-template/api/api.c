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
#include "led-task.h"
#include "pi-task.h"
#include "button-task.h"

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

void cmd_main_time_exec(const command_t *cmd)
{
    printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

void cmd_main_time_reset(const command_t *cmd)
{
    profiling_reset_max();
    printf("max reset\n");
}

void cmd_uptime(const command_t *cmd)
{
    printf("uptime: %.2f\n", time_us_64() / 1000.);
}

void cmd_info(const command_t *cmd)
{
    device_info();
}

void cmd_led_enable(const command_t *cmd)
{
    printf("led: on, period: %u ms\n", led_task_get_period_ms());
    led_task_set_state(LED_STATE_ON);
}

void cmd_led_disable(const command_t *cmd)
{
    printf("led: off, period: %u ms\n", led_task_get_period_ms());
    led_task_set_state(LED_STATE_OFF);
}

void cmd_led_blink(const command_t *cmd)
{
    printf("led: blink, period: %u ms\n", led_task_get_period_ms());
    led_task_set_state(LED_STATE_BLINK);
}

void cmd_led_period(const command_t *command)
{
    uint32_t period;
    if (command->argc != 1 || !parse_u32(command->argv[0], &period))
    {
        printf("usage led_period <period_ms>\n");
        return;
    }
    else if (command->argv[0] <= 0)
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }
    led_task_set_period_ms(period);
    printf("new period: %u ms\n", period);
}

void cmd_button(const command_t *command)
{
    printf("button: ");
    if (button_task_is_pressed())
    {
        printf("pressed\n");
    }
    else
    {
        printf("released\n");
    }
    printf("presses: %u\n", button_task_get_press_count());
}

void cmd_pi_start(const command_t *command)
{
    uint32_t pi_terms;
    if (pi_task_get_state() == PI_RUNNING)
    {
        printf("error: pi is busy\n");
        return;
    }
    if (command->argc == 0)
    {
        pi_terms = 1000000;
    }
    else if (command->argc > 1 || !parse_u32(command->argv[0], &pi_terms))
    {
        printf("error: usage pi_start [<terms>]\n");
        printf("%u\n", command->argc);
        return;
    }
    else if (command->argv[0] <= 0)
    {
        printf("error: terms must be greater than 0\n");
        return;
    }
    pi_task_start(pi_terms);
}

void cmd_pi(const command_t *command)
{
    uint64_t exec_time_us;
    double result;
    bool pi_is_available = pi_task_get_result(&result, &exec_time_us);
    if (pi_is_available)
    {
        printf("pi: %.8f in %llu ms\n", result, exec_time_us);
    }
    else
    {
        if (pi_task_get_state() == PI_RUNNING)
        {
            printf("pi: running\n");
        }
        else if (pi_task_get_state() == PI_NOT_LAUNCHED)
        {
            printf("pi: not running\n");
        }
    }
}
const api_command_t commands[] = {
    {"info", "device passport", cmd_info},
    {"main_time_exec", "superloop iteration: average and maximum", cmd_main_time_exec},
    {"main_time_reset", "reset maximum iteration", cmd_main_time_reset},
    {"uptime", "time since reset", cmd_uptime},
    {"led_enable", "turn led on", cmd_led_enable},
    {"led_disable", "turn led off", cmd_led_disable},
    {"led_blink", "led blinking", cmd_led_blink},
    {"led_period", "set led period: [period_ms]", cmd_led_period},
    {"button", "button_info", cmd_button},
    {"pi_start", "start pi computing: [<terms>]", cmd_pi_start},
    {"pi", "computation info", cmd_pi},
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
