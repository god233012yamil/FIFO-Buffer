/*
 * fifo_buffer.c
 *
 * Created: 1/21/2025 10:35:15 AM
 *  Author: yamil
 */ 

#include "fifo_buffer.h"
#include <stdio.h>

/**
 * @brief Initializes a statically allocated FIFO buffer.
 * 
 * This function sets up a statically allocated FIFO buffer by initializing the head, tail, and count
 * to zero. The size of the buffer is defined statically, and no dynamic memory allocation is performed.
 * 
 * @param fifo Pointer to the FIFO buffer structure to initialize.
 * @param buffer Pointer to a statically allocated array to be used as the buffer.
 * @param size Size of the statically allocated buffer.
 */
void FIFO_Init(FIFO_Buffer *fifo, uint8_t *buffer, uint16_t size) {
    fifo->buffer = buffer;						// Assign the statically allocated array
    fifo->size = size;							// Set the buffer size
    fifo->head = 0;								// Initialize head pointer
    fifo->tail = 0;								// Initialize tail pointer
    fifo->count = 0;							// Initialize the count of elements
    fifo->high_watermark = size - (size / 4);	// Default high watermark (75% full)
    fifo->low_watermark = size / 4;				// Default low watermark (25% full)
    fifo->overwrite_enabled = false;			// Default: no overwrite
}

/**
 * @brief Initializes a dynamically allocated FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param size Size of the buffer.
 * @return true if initialization was successful, false otherwise.
 */
bool FIFO_Init_Dynamic(FIFO_Buffer *fifo, uint16_t size) {
	fifo->buffer = (uint8_t *)malloc(size * sizeof(uint8_t));
	if (fifo->buffer == NULL) {
		return false; // Memory allocation failed
	}
	fifo->size = size;
	fifo->head = 0;
	fifo->tail = 0;
	fifo->count = 0;
	fifo->high_watermark = size - 1;	// Default to near full
	fifo->low_watermark = 1;			// Default to near empty
	fifo->overwrite_enabled = false;    // Default: no overwrite
	return true;
}

/**
 * @brief Frees the dynamically allocated buffer memory.
 * 
 * @param fifo Pointer to the FIFO buffer.
 */
void FIFO_Free(FIFO_Buffer *fifo) {
	free(fifo->buffer);
	fifo->buffer = NULL;
}

/**
 * @brief Resets the FIFO buffer to an empty state.
 * 
 * @param fifo Pointer to the FIFO buffer.
 */
void FIFO_Reset(FIFO_Buffer *fifo) {
	fifo->head = 0;
	fifo->tail = 0;
	fifo->count = 0;
}

/**
 * @brief Pushes a byte into the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param data The byte to push into the buffer.
 * @return true if successful, false if the buffer is full.
 */
bool FIFO_Push(FIFO_Buffer *fifo, uint8_t data) {
	if (fifo->count == fifo->size) {
		if (fifo->overwrite_enabled) {
			// Overwrite: Advance the tail pointer to discard the oldest byte
			fifo->tail = (fifo->tail + 1) % fifo->size;
		} else {
			return false; // Buffer is full, and overwriting is disabled
		}
	} else {
		fifo->count++;
	}

	fifo->buffer[fifo->head] = data;			// Insert the new data
	fifo->head = (fifo->head + 1) % fifo->size; // Advance the head pointer
	return true;
}

/**
 * @brief Pushes a byte into the FIFO buffer, overwriting the oldest data if full.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param data The byte to push into the buffer.
 */
void FIFO_PushOverwrite(FIFO_Buffer *fifo, uint8_t data) {
	if (fifo->count == fifo->size) {
		fifo->tail = (fifo->tail + 1) % fifo->size; // Overwrite oldest data
	} else {
		fifo->count++;
	}
	fifo->buffer[fifo->head] = data;
	fifo->head = (fifo->head + 1) % fifo->size;
}

/**
 * @brief Pops a byte from the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param data Pointer to store the popped byte.
 * @return true if successful, false if the buffer is empty.
 */
bool FIFO_Pop(FIFO_Buffer *fifo, uint8_t *data) {
	if (fifo->count == 0) {
		return false; // Buffer is empty
	}
	*data = fifo->buffer[fifo->tail];
	fifo->tail = (fifo->tail + 1) % fifo->size;
	fifo->count--;
	return true;
}

/**
 * @brief Peeks at a byte in the FIFO buffer without removing it.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param index Index of the byte to peek at (0 for the oldest byte).
 * @param data Pointer to store the peeked byte.
 * @return true if successful, false if the index is out of bounds.
 */
bool FIFO_Peek(FIFO_Buffer *fifo, uint16_t index, uint8_t *data) {
	if (index >= fifo->count) {
		return false; // Index out of bounds
	}
	uint16_t position = (fifo->tail + index) % fifo->size;
	*data = fifo->buffer[position];
	return true;
}

/**
 * @brief Checks if the FIFO buffer is empty.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @return true if empty, false otherwise.
 */
bool FIFO_IsEmpty(FIFO_Buffer *fifo) {
	return fifo->count == 0;
}

/**
 * @brief Checks if the FIFO buffer is full.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @return true if full, false otherwise.
 */
bool FIFO_IsFull(FIFO_Buffer *fifo) {
	return fifo->count == fifo->size;
}

/**
 * @brief Prints the current state of the FIFO buffer for debugging.
 * 
 * @param fifo Pointer to the FIFO buffer.
 */
void FIFO_DebugPrint(FIFO_Buffer *fifo) {
	printf("FIFO Debug Info:\n");
	printf("Size: %u, Count: %u, Head: %u, Tail: %u\n", fifo->size, fifo->count, fifo->head, fifo->tail);
	for (uint16_t i = 0; i < fifo->count; i++) {
		uint8_t data;
		FIFO_Peek(fifo, i, &data);
		printf("Index %u: %02X\n", i, data);
	}
}

/**
 * @brief Safely pushes a byte into the FIFO buffer by disabling interrupts during the operation.
 * 
 * This function ensures that no interrupt can interfere while pushing data into the FIFO buffer,
 * making it safe to use in an interrupt-driven or multi-threaded environment.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param data The byte to push into the buffer.
 * @return true if the operation is successful, false if the buffer is full.
 */
bool FIFO_PushSafe(FIFO_Buffer *fifo, uint8_t data) {
	uint8_t sreg = SREG; // Save the global interrupt flag
	cli(); // Disable interrupts
	bool result = FIFO_Push(fifo, data);
	SREG = sreg; // Restore the interrupt flag
	return result;
}

/**
 * @brief Safely pops a byte from the FIFO buffer by disabling interrupts during the operation.
 * 
 * This function ensures that no interrupt can interfere while popping data from the FIFO buffer,
 * making it safe to use in an interrupt-driven or multi-threaded environment.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param data Pointer to a variable where the popped byte will be stored.
 * @return true if the operation is successful, false if the buffer is empty.
 */
bool FIFO_PopSafe(FIFO_Buffer *fifo, uint8_t *data) {
	uint8_t sreg = SREG; // Save the global interrupt flag
	cli(); // Disable interrupts
	bool result = FIFO_Pop(fifo, data);
	SREG = sreg; // Restore the interrupt flag
	return result;
}

/**
 * @brief Enables or disables the overwrite mode for the FIFO buffer.
 * 
 * When overwrite mode is enabled, the FIFO buffer will discard the oldest data 
 * (by advancing the tail pointer) to make room for new data if the buffer is full. 
 * When overwrite mode is disabled, the buffer will reject new data if it is full.
 * 
 * This setting allows flexibility in handling data loss in scenarios where 
 * the latest data may be prioritized over the oldest data.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param enable Pass `true` to enable overwrite mode, or `false` to disable it.
 */
void FIFO_SetOverwrite(FIFO_Buffer *fifo, bool enable) {
	fifo->overwrite_enabled = enable;
}

/**
 * @brief Checks the current fill level of the FIFO buffer against its watermarks.
 * 
 * This function compares the current number of bytes in the FIFO buffer (`count`)
 * with the configured high and low watermark thresholds. It can be used to monitor 
 * buffer usage and trigger events or notifications based on these thresholds.
 * 
 * - If the count exceeds or equals the high watermark, a high watermark event can be triggered.
 * - If the count falls below or equals the low watermark, a low watermark event can be triggered.
 * 
 * Note: This function does not perform the actual event handling; it only provides 
 * a mechanism to detect when the buffer usage crosses watermark thresholds.
 * 
 * @param fifo Pointer to the FIFO buffer.
 */
void FIFO_CheckWatermarks(FIFO_Buffer *fifo) {
	if (fifo->count >= fifo->high_watermark) {
		// Trigger high watermark event
	} else if (fifo->count <= fifo->low_watermark) {
		// Trigger low watermark event
	}
}




/*
// Buffer Example Usage

#include "fifo_buffer.h"

#define BUFFER_SIZE 128
uint8_t static_buffer[BUFFER_SIZE];
FIFO_Buffer fifo;

int main(void) {
	// Initialize the FIFO with a statically allocated buffer
	FIFO_Init(&fifo, static_buffer, BUFFER_SIZE);

	// Enable overwrite mode
	FIFO_SetOverwrite(&fifo, true);

	// Push data into the buffer
	for (uint8_t i = 0; i < 150; i++) {
		FIFO_Push(&fifo, i);
	}

	// Disable overwrite mode
	FIFO_SetOverwrite(&fifo, false);

	// Try pushing more data to observe rejection when full
	for (uint8_t i = 0; i < 10; i++) {
		if (!FIFO_Push(&fifo, i)) {
			// Handle buffer full condition
		}
	}

	return 0;
}


*/



