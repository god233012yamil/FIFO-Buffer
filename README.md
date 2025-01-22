# Enhanced FIFO Buffer Implementation for Embedded Systems

A robust, feature-rich FIFO (First-In-First-Out) buffer implementation in C, designed specifically for embedded systems. This implementation includes advanced features such as watermark monitoring, dynamic allocation options, and interrupt-safe operations.

## Features

- **Dual Allocation Modes**
  - Static allocation for memory-constrained systems
  - Dynamic allocation for flexible buffer sizing
  
- **Thread and Interrupt Safety**
  - Atomic operations support
  - Safe interrupt handling with state preservation
  
- **Advanced Buffer Management**
  - Configurable overwrite mode for full buffer handling
  - Watermark monitoring for buffer level tracking
  - Peek functionality without data removal
  
- **Debug Support**
  - Built-in debug printing functionality
  - Buffer state monitoring capabilities

## Installation

1. Clone this repository:
```bash
git clone https://github.com/god233012yamil/fifo-buffer
```

2. Include both `fifo_buffer.h` and `fifo_buffer.c` in your project.

3. Include the header file in your source code:
```c
#include "fifo_buffer.h"
```

## Usage Examples

### Basic Static Buffer Usage

```c
#define BUFFER_SIZE 128
uint8_t static_buffer[BUFFER_SIZE];
FIFO_Buffer fifo;

// Initialize the buffer
FIFO_Init(&fifo, static_buffer, BUFFER_SIZE);

// Push data
uint8_t data = 0x42;
if (FIFO_Push(&fifo, data)) {
    // Data pushed successfully
}

// Pop data
uint8_t received;
if (FIFO_Pop(&fifo, &received)) {
    // Data retrieved successfully
}
```

### Dynamic Allocation

```c
FIFO_Buffer fifo;

// Initialize with dynamic allocation
if (FIFO_Init_Dynamic(&fifo, 256)) {
    // Buffer created successfully
}

// Use the buffer...

// Free the buffer when done
FIFO_Free(&fifo);
```

### Interrupt-Safe Operations

```c
// In an interrupt context
void UART_RX_Handler(void) {
    uint8_t received_byte = UART_GetByte();
    FIFO_PushSafe(&fifo, received_byte);
}

// In main context
void ProcessData(void) {
    uint8_t data;
    while (FIFO_PopSafe(&fifo, &data)) {
        // Process the data...
    }
}
```

### Overwrite Mode

```c
// Enable overwrite mode
FIFO_SetOverwrite(&fifo, true);

// Now pushing to a full buffer will overwrite oldest data
FIFO_Push(&fifo, new_data);  // Will succeed even if buffer is full
```

## API Reference

### Initialization Functions

- `FIFO_Init(FIFO_Buffer *fifo, uint8_t *buffer, uint16_t size)`
  - Initializes a static FIFO buffer
  
- `FIFO_Init_Dynamic(FIFO_Buffer *fifo, uint16_t size)`
  - Creates and initializes a dynamically allocated buffer

### Core Operations

- `FIFO_Push(FIFO_Buffer *fifo, uint8_t data)`
  - Pushes data to the buffer
  
- `FIFO_Pop(FIFO_Buffer *fifo, uint8_t *data)`
  - Retrieves and removes data from the buffer
  
- `FIFO_Peek(FIFO_Buffer *fifo, uint16_t index, uint8_t *data)`
  - Reads data without removing it

### Safety and Control

- `FIFO_PushSafe(FIFO_Buffer *fifo, uint8_t data)`
  - Interrupt-safe push operation
  
- `FIFO_PopSafe(FIFO_Buffer *fifo, uint8_t *data)`
  - Interrupt-safe pop operation
  
- `FIFO_SetOverwrite(FIFO_Buffer *fifo, bool enable)`
  - Controls buffer overwrite behavior

### Status and Monitoring

- `FIFO_IsEmpty(FIFO_Buffer *fifo)`
  - Checks if buffer is empty
  
- `FIFO_IsFull(FIFO_Buffer *fifo)`
  - Checks if buffer is full
  
- `FIFO_CheckWatermarks(FIFO_Buffer *fifo)`
  - Monitors buffer fill levels

## Implementation Details

### Buffer Structure

```c
typedef struct {
    uint8_t *buffer;           // Pointer to the circular buffer
    uint16_t size;            // Total size of the buffer
    uint16_t head;            // Write pointer
    uint16_t tail;            // Read pointer
    uint16_t count;           // Current number of elements
    uint16_t high_watermark;  // High watermark threshold
    uint16_t low_watermark;   // Low watermark threshold
    bool overwrite_enabled;   // Overwrite mode flag
} FIFO_Buffer;
```

### Memory Management

- Static allocation: Zero heap usage, suitable for resource-constrained systems
- Dynamic allocation: Flexible sizing with proper memory management functions
- Circular buffer implementation for efficient memory usage

### Thread Safety

The implementation provides interrupt-safe operations through:
- State preservation using `SREG`
- Atomic operations for critical sections
- Optional overwrite protection

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

[Your Name]
- GitHub: [@yourusername](https://github.com/god233012yamil)
- Email: god233012yamil@yahoo.com

## Acknowledgments

- Inspired by best practices in embedded systems programming
- Designed with real-world applications in mind
