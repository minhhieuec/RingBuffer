/**
 * \file ring_buffer.h
 * \brief Simple ring buffer correlation definition and statement
 * \author netube_99\netube@163.com
 * \date 2021.01.30
 * \version v1.3.1
*/

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

// #include <stdint.h>
#include <string.h>
#include "../mic_rx_config.h"

#define RING_BUFFER_SUCCESS         0x01
#define RING_BUFFER_ERROR           0x00

// Ring buffer structure
typedef struct
{
    uint32_t head;       //Operating head pointer
    uint32_t tail;       //Operate tail pointer
    uint32_t lenght;     //Saved data volume
    uint8_t *array_addr; //Buffer storage number base address
    uint32_t max_length; //Buffer maximum storage data amount
} ring_buffer;

uint8_t Ring_Buffer_Init(ring_buffer *ring_buffer_handle, uint8_t *buffer_addr, uint32_t buffer_size);         //Initialization new buffer
uint8_t Ring_Buffer_Delete(ring_buffer *ring_buffer_handle, uint8_t lenght);                                   //Delete data from the head pointer to the specified length
uint8_t Ring_Buffer_Write_Byte(ring_buffer *ring_buffer_handle, uint8_t rb_data);                              //Write a byte to the buffer
uint8_t Ring_Buffer_Read_Byte(ring_buffer *ring_buffer_handle);                                                //Read a byte from the buffer
uint8_t Ring_Buffer_Write_String(ring_buffer *ring_buffer_handle, void *input_addr, uint32_t write_lenght);    //Write the specified length data to the buffer
uint8_t Ring_Buffer_Read_String(ring_buffer *ring_buffer_handle, uint8_t *output_addr, uint32_t read_lenght);  //Read the specified length data from the buffer
uint8_t Ring_Buffer_Insert_Keyword(ring_buffer *ring_buffer_handle, uint32_t keyword, uint8_t keyword_lenght); //Ring buffer insert keyword
uint32_t Ring_Buffer_Find_Keyword(ring_buffer *ring_buffer_handle, uint32_t keyword, uint8_t keyword_lenght);  //Start searching for the nearest matching character from head pointer
static uint32_t Ring_Buffer_Get_Word(ring_buffer *ring_buffer_handle, uint32_t head, uint32_t read_lenght);    //Get the full length of the full length from the specified head pointer address (private function, no pointer-proof protection)
uint32_t Ring_Buffer_Get_Length(ring_buffer *ring_buffer_handle);                                              //Get the data length that has been stored in the buffer
uint32_t Ring_Buffer_Get_FreeSize(ring_buffer *ring_buffer_handle);                                            //Get a buffer available storage space

#endif