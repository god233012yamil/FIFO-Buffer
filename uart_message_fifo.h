/*
 * uart_message_fifo.h
 *
 * Created: 1/21/2025 11:14:51 AM
 *  Author: yamil
 */ 


#ifndef UART_MESSAGE_FIFO_H_
#define UART_MESSAGE_FIFO_H_

#include "fifo_buffer.h"

#define MESSAGE_START_BYTE 0xAA  // Example start byte
#define BUFFER_SIZE			128


bool Add_UART_Message(FIFO_Buffer *fifo, const uint8_t *message, uint8_t length);
bool Get_UART_Message(FIFO_Buffer *fifo, uint8_t *message, uint8_t *length);

#endif /* UART_MESSAGE_FIFO_H_ */