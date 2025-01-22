/*
 * fifo_buffer.h
 *
 * Created: 1/21/2025 10:34:57 AM
 *  Author: yamil
 */ 


#ifndef FIFO_BUFFER_H_
#define FIFO_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <atmel_start.h>

typedef struct {
    uint8_t *buffer;			///< Pointer to the circular buffer
    uint16_t size;				///< Total size of the buffer
    uint16_t head;				///< Write pointer
    uint16_t tail;				///< Read pointer
    uint16_t count;				///< Current number of elements in the buffer
    uint16_t high_watermark;	///< High watermark threshold
    uint16_t low_watermark;		///< Low watermark threshold
	bool overwrite_enabled;		///< Enable overwrite when buffer is full
} FIFO_Buffer;


bool FIFO_Init_Dynamic(FIFO_Buffer *fifo, uint16_t size);
void FIFO_Free(FIFO_Buffer *fifo);
void FIFO_Reset(FIFO_Buffer *fifo);
bool FIFO_Push(FIFO_Buffer *fifo, uint8_t data);
void FIFO_PushOverwrite(FIFO_Buffer *fifo, uint8_t data);
bool FIFO_Pop(FIFO_Buffer *fifo, uint8_t *data);
bool FIFO_Peek(FIFO_Buffer *fifo, uint16_t index, uint8_t *data);
bool FIFO_IsEmpty(FIFO_Buffer *fifo);
bool FIFO_IsFull(FIFO_Buffer *fifo);
void FIFO_DebugPrint(FIFO_Buffer *fifo);
bool FIFO_PushSafe(FIFO_Buffer *fifo, uint8_t data);
bool FIFO_PopSafe(FIFO_Buffer *fifo, uint8_t *data);
void FIFO_SetOverwrite(FIFO_Buffer *fifo, bool enable);
void FIFO_CheckWatermarks(FIFO_Buffer *fifo);

#endif /* FIFO_BUFFER_H_ */