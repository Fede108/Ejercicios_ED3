/*
 * @file Ej13.c
 * @brief Timer0 
 *
 * The timer resolution determines how finely we can control the timing. 
 * The timer resolution is calculated as:
 *
 * Timer Resolution = (Prescaler + 1) / PCLK
 *
 * With a clock of 50 MHz, maximum peripheral frequency divider and a prescaler of maximum load value, the timer resolution is:
 *
 * Timer Resolution = ( 2^32 - 1 + 1) / (50/n MHz) = 687.195 segundos 
 *
 * The maximum time the timer can count before overflowing depends on the timer's resolution and the maximum 32-bit
 * count. Using the formula:
 *
 * Maximum Time = Timer Resolution * (2^32 - 1)
 *
 * The maximum time the timer can count before overflowing is:
 *
 * Maximum Time = 687.195 s * 2^32 = 
 *
 *
 */

