#include "stdio-text-protocol.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "log.h"
#include <stdio.h>


static char line[STDIO_TEXT_PROTOCOL_LINE_SIZE]; // пока так, но не уверен, что это хорошая идея 
static uint line_length = 0; // пока так, но не уверен, что это хорошая идея 
static command_t cmd;
const command_t *stdio_text_protocol_handle(void)
{
    
    // // читаем символ без ожидания; нет символа — возвращаем NULL
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }
    
    // конец строки: пустую строку пропускаем, иначе переводим строку в терминале,
    // делим строку на слова и возвращаем команду
    if (symbol == '\r' || symbol == '\n')
    {
        putchar('\n');
        LOG_DBG("got %s\n", line);
        
        cmd.name = NULL;
        cmd.argc = 0;
        cmd.truncated = false;
        line[line_length] = '\0';
        if (line_length > 0)
        { 
            char *p = line;
            uint command_id = 0;
            cmd.name = p;

            while (*p != '\0')
            {
                if (*p == ' ' || *p == '\r' || *p == '\n')
                {
                    *p = '\0';
                    if (command_id <= STDIO_TEXT_PROTOCOL_MAX_ARGS - 1)
                    {

                        if (*(p+1) != ' ' && *(p+1) != '\0')
                        {
                            cmd.argv[command_id] = p + 1;
                            command_id += 1;
                            cmd.argc += 1;
                        }
                    }
                    else
                    {
                        cmd.truncated = true;
                        line_length = 0;
                        return &cmd;
                    }
                }
                p += 1;
            }
            // printf("name = %s\r\n", cmd.name);
            // for (int i = 0; i < command_id; i+=1)
            // {
            //     printf("argv[%u] = %s\r\n", i, cmd.argv[i]);
            // }
        }
        line_length = 0;
        return &cmd;
    }

    // Backspace (0x08 или 0x7f): убираем последний символ и стираем его на экране "\b \b"
    if (symbol == 0x08 || symbol == 0x7f){
        if (line_length != 0)
        {
            line_length -= 1;
            line[line_length] = '\0';
            putchar('\b');
            putchar(' ');
            putchar('\b');
        }
        return NULL;
    }

    // печатный символ: кладём в буфер и повторяем в терминал;
    if (line_length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)
    {
        line[line_length] = (char)symbol;
        line_length = line_length + 1;
        putchar(symbol);
        return NULL;
    }
    // буфер полон — помечаем строку как переполненную
    else
    {
        cmd.truncated = true;
        cmd.argc = 0;
        line_length = 0;
        return &cmd;
    }
}


