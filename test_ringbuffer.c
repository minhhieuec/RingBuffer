#include "ring_buffer.h"

#define Read_BUFFER_SIZE        256

// Set a length (byte) of a split keyword and keyword
#define SEPARATE_SIGN           0xCCFB22AA
#define SEPARATE_SIGN_SIZE      4

void test_rb_find_keyword(void)
{
    // New buffer array and RingBuffer handle
    uint8_t buffer[Read_BUFFER_SIZE];
    ring_buffer RB;

    // Record the number of paragraphs
    uint8_t String_Count = 0;

    uint8_t length;
    uint8_t get[16];

    // Initialize the RingBuffer handle, bind the buffer array;
    Ring_Buffer_Init(&RB, buffer, Read_BUFFER_SIZE);

    // Write three data to the ring buffer, insert a separated keyword each paragraph
    Ring_Buffer_Write_String(&RB, "ABCDEFGHIJK\r\n", 13);               // Write a data
    Ring_Buffer_Insert_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE); // Insert a split keyword
    String_Count++;                                                     // Record the number of paragraphs +1

    Ring_Buffer_Write_String(&RB, "abcdefg\r\n", 9);
    Ring_Buffer_Insert_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE);
    String_Count++;

    Ring_Buffer_Write_String(&RB, "1234\r\n", 6);
    Ring_Buffer_Insert_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE);
    String_Count++;

    while (String_Count != 0)
    {
        // Get the distance from the head pointer to the high position of the keyword, the distance from -1 gets the length of the first paragraph
        length = Ring_Buffer_Find_Keyword(&RB, SEPARATE_SIGN, SEPARATE_SIGN_SIZE) - 1;
        Ring_Buffer_Read_String(&RB, get, length);   // Read a piece of data, save to get arrays
        printf("%s", get);                           // Print data
        Ring_Buffer_Delete(&RB, SEPARATE_SIGN_SIZE); // Delete data of the length of the keyword, that is, delete keywords
        String_Count--;                              // Record the number of paragraphs-1
    }
}

void test_rb_simple(void)
{
    // New buffer array and RingBuffer handle
    uint8_t buffer[Read_BUFFER_SIZE];
    ring_buffer RB;
    uint8_t get[16];
    uint32_t num;

    // Initialize the RingBuffer handle, bind the buffer array;
    Ring_Buffer_Init(&RB, buffer, Read_BUFFER_SIZE);

    //Write a byte and one byte to the ring buffer
    Ring_Buffer_Write_String(&RB, "hello world", 11);
    Ring_Buffer_Write_Byte(&RB, '!');

    // Get stored data length, read out data in the ring buffer and print
    num = Ring_Buffer_Get_Length(&RB);

    Ring_Buffer_Read_String(&RB, get, num);
    printf("%s", get);
}

void test_ringbuffer(void)
{
    test_rb_simple();
    test_rb_find_keyword();
}