
/**
 * @file lab1.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <stdio.h>


typedef union {
    int int_data;
    float float_data;
}MessangeData;


typedef struct {
    uint8_t type;
    int length;
    uint8_t flag;
    MessangeData data;
}mensaje;



int main(void)
{
    // Print the message "Hello, World!" to the console
    printf("Hello, World!\n");

    // Return 0 to the operating system indicating the program terminated successfully
    return 0;
}
