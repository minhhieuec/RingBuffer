# RingBuffer Lightweight ring buffer
[![GitHub repository](https://img.shields.io/badge/github-RingBuffer-blue)](https://github.com/netube99/RingBuffer) [![GitHub license](https://img.shields.io/github/license/netube99/RingBuffer?color=green)](https://github.com/netube99/RingBuffer/blob/main/LICENSE) [![Language](https://img.shields.io/badge/make%20with-C-red)]()

## Introduction
RingBuffer is a lightweight ring buffer developed based on the C language, suitable for application scenarios such as serial port transceiver on various embedded platforms;

Compared with the ordinary receiving and sending buffer array, the operation of the ring buffer is simpler and the performance is more powerful; using the functions provided by RingBuffer, you can easily implement data reading, writing, query, and deletion operations, without the need to write complex address records and reads. Writing counting and other codes reduces the risk of data pointer overflow and data being overwritten. At the same time, RingBuffer also provides functions such as inserting and searching for customizable separation keywords. It can be used to store multiple pieces of data in the embedded serial port transceiver. The paragraphs are separated independently, and the read-write buffer is no longer blocked by hardware timing, which reduces the requirement of the serial port for the chip's real-time response and improves the reliability of data transmission;

The code has been compiled on the GD32F130C8T6 platform, and the serial port receiving and sending stress test of a large amount of data has been carried out, and no obvious BUG has been found for the time being.

## begin
1. Copy the library file to the project and quote the RingBuffer header file in the source code;
2. Create a new uint8_t array, the size of the array is the size of the ring buffer;
3. Create a new RingBuffer operation handle;
4. Initialize the RingBuffer operation handle, bind it to the newly created array, and convert it into a ring buffer;
5. At this time, we can use various functions provided by the library to read and write the newly created buffer;
6. Please refer to the notes in ring_buffer.c during the programming process for the details of the function;

## Example
Here I show how to create and initialize RingBuffer, read and write ring buffer, query data length and other basic operations; you can learn about the function naming and parameter style and basic usage of RingBuffer

```c
#include <stdio.h>
#include <ring_buffer.h>

#define Read_BUFFER_SIZE	256

int main()
{
    //New buffer array and RingBuffer operation handle
    uint8_t buffer[Read_BUFFER_SIZE] ;
    ring_buffer RB ;
    
    //Initialize the RingBuffer operation handle and bind the buffer array;
    Ring_Buffer_Init(&RB, buffer, Read_BUFFER_SIZE);
    
    //Write a byte and a byte to the ring buffer
    Ring_Buffer_Write_String(&RB, "hello world", 11);
    Ring_Buffer_Write_Byte(&RB, '!');
    
    //Get the stored data length, read out the data in the ring buffer and print
    uint32_t num = Ring_Buffer_Get_Lenght(&RB);
    uint8_t get[16] ;
    Ring_Buffer_Read_String(&RB, get, num);
    printf("%s", get);
    
    return 0 ;
}
```
In addition to the basic read and write operations, in order to make better use of the ring feature, I have added functions such as separating keywords, querying keywords, and deleting data. Based on these functions, you can realize multi-segment data caching and data transmission in the serial port. Accurate reading; reduces the requirement of real-time response and improves the performance of serial port transceiver

```c
#include <stdio.h>
#include <ring_buffer.h>

#define Read_BUFFER_SIZE	256

//Set a separation keyword and the length of the keyword (bytes)
#define SEPARATE_SIGN       0xCCFB22AA
#define SEPARATE_SIGN_SIZE  4

int main()
{
    //New buffer array and RingBuffer operation handle
    uint8_t buffer[Read_BUFFER_SIZE] ;
    ring_buffer RB ;

    //Initialize the RingBuffer operation handle and bind the buffer array;
    Ring_Buffer_Init(&RB, buffer, Read_BUFFER_SIZE);

    //Number of recorded paragraphs
    uint8_t String_Count = 0 ;

    //Write three pieces of data to the ring buffer, insert a separation keyword between each piece
    Ring_Buffer_Write_String(&RB, "ABCDEFGHIJK\r\n", 13);//Write a piece of data
    Ring_Buffer_Insert_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE);//Insert a separate keyword
    String_Count ++ ;//Number of recorded paragraphs +1

    Ring_Buffer_Write_String(&RB, "abcdefg\r\n", 9);
    Ring_Buffer_Insert_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE);
    String_Count ++ ;

    Ring_Buffer_Write_String(&RB, "1234\r\n", 6);
    Ring_Buffer_Insert_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE);
    String_Count ++ ;

    while(String_Count != 0)
    {
        uint8_t get[16] ;
        //Get the distance from the head pointer to the high position of the keyword, the distance -1 gets the length of the first piece of data
        uint8_t lenght = Ring_Buffer_Find_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE) - 1 ;
        Ring_Buffer_Read_String(&RB, get, lenght);//Read a piece of data and save it to the get array
        printf("%s", get);//Print data
        Ring_Buffer_Delete(&RB, SEPARATE_SIGN_SIZE);//Delete the data that separates the length of the keyword, that is, delete the keyword
        String_Count -- ;//Number of recorded paragraphs -1
    }

    return 0 ;
}
```
## Update
2021.01.19 v1.0.0 Release the first version  
2021.01.24 v1.1.0 Add matching character search function  
2021.01.27 v1.2.0 Remade matching character search function, now supports 8-bit to 32-bit keyword query
2021.01.28 v1.3.0 The reset function is modified to delete function, add keyword insertion function (adaptive size end)
2021.01.30 v1.3.1 Fixed a small probability pointer overflow error in String read and write functions