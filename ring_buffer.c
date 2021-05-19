/**
 * \file ring_buffer.c
 * \brief Simple ring buffer implementation
 * \details It can be used as a ring buffer / soft FIFO of the single-chip serial port to improve the serial data transmission capacity;
 * You can quickly store / read data to the ring buffer, and all data can be accurately read without manually recording data reception;
 * No need to manually empty the data buffer, as long as the last received data is read, the buffer is ready to receive the next paragraph;
 * Save time to manually empty the normal buffer area, able to enhance the operational efficiency of the serial program;
 * \author netube_99\netube@163.com
 * \date 2021.01.30
 * \version v1.3.1
 * 
 * 2021.01.19 v1.0.0 Release the first version
 * 2021.01.24 v1.1.0 Add a matching function
 * 2021.01.27 v1.2.0 Remaster matching character lookup feature, now supported 8 digits to 32-bit keyword queries
 * 2021.01.28 v1.3.0 The reset function is modified to delete functions, add keyword insert function (adaptive size end)
 * 2021.01.30 v1.3.1 Fixed a small probability pointer overflow error in String read and write functions
*/

#include "ring_buffer.h"

/**
 * \brief Initialization new buffer
 * \param[out] ring_buffer_handle: Buffer structure handle to be initialized
 * \param[in] buffer_addr: Array array of external definitions, type must be uint8_t
 * \param[in] buffer_size: External defined buffer array space
 * \return Returns the result of the buffer initialization
 *      \arg RING_BUFFER_SUCCESS: Initialization successful
 *      \arg RING_BUFFER_ERROR: initialization failed
*/
uint8_t Ring_Buffer_Init(ring_buffer *ring_buffer_handle, uint8_t *buffer_addr, uint32_t buffer_size)
{
    ring_buffer_handle->head = 0;                 //Reset head pointer
    ring_buffer_handle->tail = 0;                 //Reset tail pointer
    ring_buffer_handle->lenght = 0;               //Reset has stored data length
    ring_buffer_handle->array_addr = buffer_addr; //Buffer storage number base address
    ring_buffer_handle->max_length = buffer_size; //Buffer maximum storage data amount
    if (ring_buffer_handle->max_length < 2)       //Buffer arrays must have two elements or more
        return RING_BUFFER_ERROR;                 //The buffer array is too small, the queue initialization failed
    else
        return RING_BUFFER_SUCCESS; //Buffer initialization success
}

/**
 * \brief Delete data from the head pointer to the specified length
 * \param[out] ring_buffer_handle: Buffer structure
 * \param[in] lenght: To delete the length
 * \return Return to delete the specified length data result
 *      \arg RING_BUFFER_SUCCESS: successfully deleted
 *      \arg RING_BUFFER_ERROR: failed to delete
*/
uint8_t Ring_Buffer_Delete(ring_buffer *ring_buffer_handle, uint8_t lenght)
{
    if (ring_buffer_handle->lenght < lenght)
        return RING_BUFFER_ERROR; //The amount of data that has been stored is less than the amount of data that needs to be deleted.
    else
    {
        if ((ring_buffer_handle->head + lenght) >= ring_buffer_handle->max_length)
            ring_buffer_handle->head = lenght - (ring_buffer_handle->max_length - ring_buffer_handle->head);
        else
            ring_buffer_handle->head += lenght; //Head pointer advances forward, abandon data
        ring_buffer_handle->lenght -= lenght;   //Record the valid data length
        return RING_BUFFER_SUCCESS;             //The amount of data that has been stored is less than the amount of data that needs to be deleted.
    }
}

/**
 * \brief Write one byte to the end of the buffer
 * \param[out] ring_buffer_handle: Buffer structure
 * \param[in] data: To write bytes
 * \return Returns the result of the buffer write byte
 *      \arg RING_BUFFER_SUCCESS: Write success
 *      \arg RING_BUFFER_ERROR: Write failure
*/
uint8_t Ring_Buffer_Write_Byte(ring_buffer *ring_buffer_handle, uint8_t rb_data)
{
    //The array of buffers is full, resulting in an overlay error
    if (ring_buffer_handle->lenght == (ring_buffer_handle->max_length - 1))
        return RING_BUFFER_ERROR;
    else
    {
        *(ring_buffer_handle->array_addr + ring_buffer_handle->tail) = rb_data; //Base site + offset, storage data
        ring_buffer_handle->lenght++;                                           //Data quantity count +1
        ring_buffer_handle->tail++;                                             //Tail pointing
    }
    //If the tail pointer beyond the end of the array, the tail pointer points to the beginning of the buffer array, forming a closed loop.
    if (ring_buffer_handle->tail > (ring_buffer_handle->max_length - 1))
        ring_buffer_handle->tail = 0;
    return RING_BUFFER_SUCCESS;
}

/**
 * \brief Read a byte from the buffer head pointer
 * \param[in] ring_buffer_handle: Buffer structure
 * \return Returns the read byte
*/
uint8_t Ring_Buffer_Read_Byte(ring_buffer *ring_buffer_handle)
{
    uint8_t rb_data;
    if (ring_buffer_handle->lenght != 0) //Data is not read
    {
        rb_data = *(ring_buffer_handle->array_addr + ring_buffer_handle->head); //Read data
        ring_buffer_handle->head++;
        ring_buffer_handle->lenght--; //Data quantity count -1
        //If the head pointer exceeds the end of the array, the head pointer points to the beginning of the array, forming a closed loop.
        if (ring_buffer_handle->head > (ring_buffer_handle->max_length - 1))
            ring_buffer_handle->head = 0;
    }
    return rb_data;
}

/**
 * \brief Write the data of the specified length to the tail of the buffer
 * \param[out] ring_buffer_handle: Buffer structure
 * \param[out] input_addr: Base site to be written to
 * \param[in] write_lenght: Number of bytes to be written
 * \return Returns the result of the end of the buffer to write the specified length byte
 *      \arg RING_BUFFER_SUCCESS: Write success
 *      \arg RING_BUFFER_ERROR: Write failure
*/
uint8_t Ring_Buffer_Write_String(ring_buffer *ring_buffer_handle, void *input_addr, uint32_t write_lenght)
{
    //If you are not enough to store new data, return an error
    if ((ring_buffer_handle->lenght + write_lenght) > (ring_buffer_handle->max_length))
        return RING_BUFFER_ERROR;
    else
    {
        //Set two write lengths
        uint32_t write_size_a, write_size_b;
        //If the order can be used in length than the length of the write, it is necessary to write data twice.
        if ((ring_buffer_handle->max_length - ring_buffer_handle->tail) < write_lenght)
        {
            write_size_a = ring_buffer_handle->max_length - ring_buffer_handle->tail; //Write from the tail pointer to the end of the store
            write_size_b = write_lenght - write_size_a;                               //Write data from the start of the array
        }
        else //If the order can be used for a length greater than or equal to the length of the need to write, it only needs to be written once.
        {
            write_size_a = write_lenght; //Write from the tail pointer to the end of the store
            write_size_b = 0;            //No need to write data from the beginning of the store
        }
        //Start writing data
        if (write_size_b != 0) //Need to write twice
        {
            //Copy A, B data to the storage array, respectively
            memcpy(ring_buffer_handle->array_addr + ring_buffer_handle->tail, input_addr, write_size_a);
            memcpy(ring_buffer_handle->array_addr, input_addr + write_size_a, write_size_b);
            ring_buffer_handle->lenght += write_lenght; //How much data is recorded
            ring_buffer_handle->tail = write_size_b;    //Repositioning the tail pointer position
        }
        else //只需写入一次
        {
            memcpy(ring_buffer_handle->array_addr + ring_buffer_handle->tail, input_addr, write_size_a);
            ring_buffer_handle->lenght += write_lenght; //How much data is recorded
            ring_buffer_handle->tail += write_size_a;   //Repositioning the tail pointer position
            if (ring_buffer_handle->tail == ring_buffer_handle->max_length)
                ring_buffer_handle->tail = 0; //If the write data is written, it is just written to the end of the array, it will return to the beginning and prevent the offside.
        }
        return RING_BUFFER_SUCCESS;
    }
}

/**
 * \brief Read the data of the specified length to the buffer header, save to the specified address
 * \param[in] ring_buffer_handle: Buffer structure
 * \param[out] output_addr: Read data saved address
 * \param[in] read_lenght: Number of bytes to read
 * \return Returns the result of the buffer header read the specified length byte
 *      \arg RING_BUFFER_SUCCESS: Read success
 *      \arg RING_BUFFER_ERROR: Read failure
*/
uint8_t Ring_Buffer_Read_String(ring_buffer *ring_buffer_handle, uint8_t *output_addr, uint32_t read_lenght)
{
    if (read_lenght > ring_buffer_handle->lenght)
        return RING_BUFFER_ERROR;
    else
    {
        uint32_t Read_size_a, Read_size_b;
        if (read_lenght > (ring_buffer_handle->max_length - ring_buffer_handle->head))
        {
            Read_size_a = ring_buffer_handle->max_length - ring_buffer_handle->head;
            Read_size_b = read_lenght - Read_size_a;
        }
        else
        {
            Read_size_a = read_lenght;
            Read_size_b = 0;
        }
        if (Read_size_b != 0) //Need to read twice
        {
            memcpy(output_addr, ring_buffer_handle->array_addr + ring_buffer_handle->head, Read_size_a);
            memcpy(output_addr + Read_size_a, ring_buffer_handle->array_addr, Read_size_b);
            ring_buffer_handle->lenght -= read_lenght; //Record the amount of remaining data
            ring_buffer_handle->head = Read_size_b;    //Repositioning head pointer position
        }
        else
        {
            memcpy(output_addr, ring_buffer_handle->array_addr + ring_buffer_handle->head, Read_size_a);
            ring_buffer_handle->lenght -= read_lenght; //Record the amount of remaining data
            ring_buffer_handle->head += Read_size_a;   //Repositioning head pointer position
            if (ring_buffer_handle->head == ring_buffer_handle->max_length)
                ring_buffer_handle->head = 0; //If the head pointer is just written to the end of the array, it will return to the beginning to prevent the offside.
        }
        return RING_BUFFER_SUCCESS;
    }
}

/**
 * \brief Ring buffer insert keyword
 * \param[in] ring_buffer_handle: Buffer structure
 * \param[in] keyword: Keywords to insert
 * \param[in] keyword_lenght:Key words texture, maximum 4 bytes (32-bit)
 * \return Return the result of the keyword
 *      \arg RING_BUFFER_SUCCESS: Insert success
 *      \arg RING_BUFFER_ERROR: Insert failure
*/
uint8_t Ring_Buffer_Insert_Keyword(ring_buffer *ring_buffer_handle, uint32_t keyword, uint8_t keyword_lenght)
{
    uint8_t *keyword_addr = (uint8_t *)&keyword;
    uint8_t keyword_byte[4];
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    //Small-end mode word sequence arrangement
    keyword_byte[0] = *(keyword_addr + 3);
    keyword_byte[1] = *(keyword_addr + 2);
    keyword_byte[2] = *(keyword_addr + 1);
    keyword_byte[3] = *(keyword_addr + 0);
    //Fill keywords in ring buffer
    return Ring_Buffer_Write_String(ring_buffer_handle, keyword_byte, keyword_lenght);
#else
    //Large end mode word sequence arrangement
    keyword_byte[0] = *(keyword_addr + 0);
    keyword_byte[1] = *(keyword_addr + 1);
    keyword_byte[2] = *(keyword_addr + 2);
    keyword_byte[3] = *(keyword_addr + 3);
    //Fill keywords in ring buffer
    return Ring_Buffer_Write_String(ring_buffer_handle, keyword_byte + (keyword_lenght - 1), keyword_lenght);
#endif
}

/**
 * \brief Start searching for the nearest matching keyword from the head pointer
 * \param[in] ring_buffer_handle: Buffer structure
 * \param[in] keyword: Keywords to find
 * \param[in] keyword_lenght:Key words texture, maximum 4 bytes (32-bit)
 * \return Returns the number of bytes that you need to read, return 0 / RING_BUFFER_ERROR: Find failed
*/
uint32_t Ring_Buffer_Find_Keyword(ring_buffer *ring_buffer_handle, uint32_t keyword, uint8_t keyword_lenght)
{
    uint32_t max_find_lenght = ring_buffer_handle->lenght - keyword_lenght + 1; //Calculate the maximum length that needs to be searched
    uint8_t trigger_word = keyword >> ((keyword_lenght - 1) * 8);               //Calculate bytes (highest) to trigger keyword check
    uint32_t distance = 1, find_head = ring_buffer_handle->head;                //Record keyword distance head pointer length / temporary head pointer gets the original pointer initial value
    while (distance <= max_find_lenght)                                         //Search for keywords within the setting range (prevent pointer offside errors)
    {
        if (*(ring_buffer_handle->array_addr + find_head) == trigger_word)                      //If the high byte match begins to check to the low position
            if (Ring_Buffer_Get_Word(ring_buffer_handle, find_head, keyword_lenght) == keyword) //Meet keyword match
                return distance;                                                                //Return the length, use Ring_Buffer_Read_String to extract data
        find_head++;                                                                            //The current character does not match, the temporary head pointer is moved, check the next one
        distance++;                                                                             //The length also has to move after search
        if (find_head > (ring_buffer_handle->max_length - 1))
            find_head = 0; //If you go to the end of the array, return the beginning of the array (ring buffering characteristics)
    }
    return RING_BUFFER_ERROR; //I found it
}

/**
 * \brief Get the full length of the full length from the specified head pointer address (private function, no pointer-proof protection)
 * \param[in] ring_buffer_handle: Buffer structure
 * \param[in] head: Head pointer offset (the address of the matching characters)
 * \param[in] read_lenght:Key words texture, maximum 4 bytes (32-bit)
 * \return Return complete keyword
*/
static uint32_t Ring_Buffer_Get_Word(ring_buffer *ring_buffer_handle, uint32_t head, uint32_t read_lenght)
{
    uint32_t rb_data = 0, i;
    for (i = 1; i <= read_lenght; i++) //Extract data backwards according to the length of keyword (number of characters)
    {
        //From the highest bit to the lowest position, integrate into a 32-bit data
        rb_data |= *(ring_buffer_handle->array_addr + head) << (8 * (read_lenght - i));
        head++;
        if (head > (ring_buffer_handle->max_length - 1))
            head = 0; //If you go to the end of the array, return the beginning of the array (ring buffering characteristics)
    }
    return rb_data; //Returns 32-bit data after shifting integration
}

/**
 * \brief Get the data length that has been stored in the buffer
 * \param[in] ring_buffer_handle: Buffer structure
 * \return Returns the amount of data already stored in the buffer
*/
uint32_t Ring_Buffer_Get_Length(ring_buffer *ring_buffer_handle)
{
    return ring_buffer_handle->lenght;
}

/**
 * \brief Get a buffer available storage space
 * \param[in] ring_buffer_handle: Buffer structure
 * \return Return to buffer available storage space
*/
uint32_t Ring_Buffer_Get_FreeSize(ring_buffer *ring_buffer_handle)
{
    return (ring_buffer_handle->max_length - ring_buffer_handle->lenght);
}