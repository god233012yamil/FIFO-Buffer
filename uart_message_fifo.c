/*
 * uart_message_fifo.c
 *
 * Created: 1/21/2025 11:15:12 AM
 *  Author: yamil
 */ 

#include "uart_message_fifo.h"

/**
 * @brief Adds a complete UART message to the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param message Pointer to the message data to add.
 * @param length Length of the message, including the checksum.
 * @return true if the message was successfully added, false if the buffer lacks space.
 */
bool Add_UART_Message(FIFO_Buffer *fifo, const uint8_t *message, uint8_t length) {
	if (length < 3 || BUFFER_SIZE - fifo->count < length) {
		return false; // Message too short or not enough space
	}
	
	for (uint8_t i = 0; i < length; i++) {
		if (!FIFO_Push(fifo, message[i])) {
			return false;
		}
	}
	return true;
}

/**
 * @brief Retrieves a complete UART message from the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param message Pointer to an array to store the retrieved message.
 * @param length Pointer to store the length of the retrieved message.
 * @return true if a complete message was retrieved, false if the buffer is empty or corrupted data.
 */
bool Get_UART_Message(FIFO_Buffer *fifo, uint8_t *message, uint8_t *length) {
	if (FIFO_IsEmpty(fifo)) {
		return false; // Buffer is empty
	}
	
	uint8_t start_byte;
	if (!FIFO_Pop(fifo, &start_byte) || start_byte != MESSAGE_START_BYTE) {
		return false; // Invalid start byte
	}
	
	uint8_t message_length;
	if (!FIFO_Pop(fifo, &message_length) || message_length < 3) {
		return false; // Invalid length
	}
	
	*length = message_length;
	message[0] = MESSAGE_START_BYTE;
	message[1] = message_length;
	
	uint8_t checksum = 0;
	for (uint8_t i = 2; i < message_length; i++) {
		if (!FIFO_Pop(fifo, &message[i])) {
			return false; // Incomplete message
		}
		checksum ^= message[i];
	}
	
	if (checksum != 0) {
		return false; // Invalid checksum
	}
	
	return true;
}

/*
Example Usage.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "fifo_buffer.h"
#include "uart_message_fifo.h"

#define UART_BAUD_RATE 9600
#define F_CPU 16000000UL
#define BAUD_PRESCALE ((F_CPU / (UART_BAUD_RATE * 16UL)) - 1)

FIFO_Buffer uart_fifo;  // Define the UART FIFO buffer

// Initializes UART for AVR128DA64.
void UART_Init(void) {
    // Set baud rate
    UBRR0H = (uint8_t)(BAUD_PRESCALE >> 8);
    UBRR0L = (uint8_t)(BAUD_PRESCALE);

    // Enable receiver and receiver interrupt
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0);

    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Sends a byte of data over UART.
void UART_SendByte(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))) {
        // Wait until the buffer is empty
    }
    UDR0 = data;  // Send the data
}

// Sends a complete message over UART.
void UART_SendMessage(const uint8_t *message, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        UART_SendByte(message[i]);
    }
}

// Processes a complete UART message.
void ProcessMessage(const uint8_t *message, uint8_t length) {
    // Example: Echo the received message back
    UART_SendMessage(message, length);
}

// Main program loop.
int main(void) {
    // Initialize the FIFO with a statically allocated buffer
    FIFO_Init(&fifo, uart_fifo, BUFFER_SIZE);
	// Initialize UART
    UART_Init(); 
	// Enable global interrupts          
    sei();                 

    uint8_t message[64];   // Buffer to hold a single UART message
    uint8_t length;        // Length of the retrieved message

    while (1) {
        // Check if a complete message can be retrieved
        if (Get_UART_Message(&uart_fifo, message, &length)) {
            ProcessMessage(message, length);  // Process the message
        }
    }
}

// UART Receive Interrupt Service Routine.
ISR(USART_RX_vect) {
    uint8_t received_byte = UDR0;  // Read the received byte
    FIFO_Push(&uart_fifo, received_byte);  // Add the byte to the FIFO buffer
}


*/