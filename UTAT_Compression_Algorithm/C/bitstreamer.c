#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "bitstreamer.h"

uint64_t* init_bitstream(void)
{
    uint64_t* bitstream = calloc(sizeof(uint64_t), size);
    
    return bitstream;
}

void init_bitstreamer_variables(void)
{
    used = 0;
    read = 0;
    padding = 0;
    file_cursor = 0;
    size = get_size();
}

void write_to_disk(uint64_t* bitstream)
{
    printf("Writing to disk: ");
    PRINT_BITSTREAM
    FILE *fptr;
    fptr = fopen("output/encoded.bin","ab");  // a for append, b for binary
    fwrite(bitstream, sizeof(uint64_t), size, fptr);
    fclose(fptr);

    for(uint8_t i = 0; i < size; ++i)
        bitstream[i] = 0;

    used = 0;
}

void read_from_disk(uint64_t* bitstream)
{
    //Read bitstream from bin file
    //Check if SEEK_CUR is reset when closing
    FILE *fptr;
    fptr = fopen("output/encoded.bin", "rb");
    file_cursor -= size * 8;
    fseek(fptr, file_cursor, SEEK_END);
    fread(bitstream, size, 8, fptr);
    fclose(fptr);
    read = 0;
    //PRINT_BITSTREAM
}

void write_to_bitstream(uint64_t value, uint64_t* bitstream, uint8_t width)
{
    //Buffer overflowing
    if(used + width >= BUFFER_SIZE * 8)
    {
        //Dump to disk
        uint64_t temp = 0;
        uint8_t temp_width  = (BUFFER_SIZE * 8) - used;
        uint64_t temp_value = value >> (width - temp_width);
        printf("Adding part of %llu to bitstream: %llu\n", value, temp_value);
        for(int i = 0; i < size; ++i)
        {
            temp = UINT64_MAX << (64 - temp_width); //Create bitmask
            temp = temp & bitstream[i];             //Grab the MSBs that overflow
            bitstream[i] = bitstream[i] << temp_width;
            bitstream[i] = bitstream[i] | temp_value;
            temp_value = temp >> (64 - temp_width);
        }
        //DUMP TO DISK HERE
        write_to_disk(bitstream);

        width = (width - temp_width);
        value = value >> temp_width;
    }   
    
    uint64_t temp = 0;
    for(int i = 0; i < size; ++i)
    {
        temp = UINT64_MAX << (64 - width); //Create bitmask
        temp = temp & bitstream[i];        //Grab the MSBs that overflow
        bitstream[i] = bitstream[i] << width;
        bitstream[i] = bitstream[i] | value;
        value = temp >> (64 - width);
    }
    used += width;
}

uint64_t read_from_bitstream(uint64_t* bitstream, uint8_t width)
{
    uint64_t temp = 0;
    uint64_t value = 0;

    PRINT_BITSTREAM

    //Value cut between this buffer and the next
    if(read + width >= BUFFER_SIZE * 8) //Maybe >=
    {   
        //printf("Value bring cut. Going to splice\n");
        //printf("Read: %i\n", read);
        
        uint16_t temp_width = (BUFFER_SIZE * 8) - read;
        //printf("temp_width: %hu\n", temp_width);
        uint64_t temp_value = 0;
        //printf("Debugging Here\n");
        for(int i = size - 1; i >=0 ; --i)
        {  
            temp = UINT64_MAX >> (64 - temp_width); //Create bitmask
            //printf("Bitmask: %llx\n", temp);
            temp = temp & bitstream[i];        //Grab the LSBs that overflow
            //printf("Overflow: %llx\n", temp);
            bitstream[i] = bitstream[i] >> temp_width;
            bitstream[i] = bitstream[i] | temp_value; 
            temp_value = temp << (64 - temp_width);
            //printf("temp_value: %llx\n", temp_value);
            //PRINT_BITSTREAM
        }
        read += temp_width;
        temp_value = temp_value >> (64 - temp_width);
        //printf("temp_value: %llx\n", temp_value);
        //printf("DEBEUGGING Done\n");
        
        //Read from disk
        //printf("READING FROM DISK\n");
        read_from_disk(bitstream);
        //PRINT_BITSTREAM

        width = width - temp_width;
        //printf("Read: %i\n", read);
        //printf("Width: %i\n", width);
        if(width != 0)
        {
            for(int i = size - 1; i >=0 ; --i)
            {  
                temp = UINT64_MAX >> (64 - width); //Create bitmask
                temp = temp & bitstream[i];        //Grab the LSBs that overflow
                bitstream[i] = bitstream[i] >> width;
                bitstream[i] = bitstream[i] | value; 
                value = temp << (64 - width);
                //PRINT_BITSTREAM
            }
            read += width;
            value = value >> (64 - width);
            printf("Value: %llu, temp_width: %i, temp_value: %llu\n", value, temp_width, temp_value);
            
            printf("Spliced Value: %llu\n", value);
        }
        value = (value << temp_width) | temp_value;
        
        return value;
        
    }
    else
    { 
        for(int i = size - 1; i >=0 ; --i)
        {  
            temp = UINT64_MAX >> (64 - width); //Create bitmask
            temp = temp & bitstream[i];        //Grab the LSBs that overflow
            bitstream[i] = bitstream[i] >> width;
            bitstream[i] = bitstream[i] | value; 
            value = temp << (64 - width);
            //PRINT_BITSTREAM
        }
        read += width;
        //PRINT_BITSTREAM
        return (value >> (64 - width));
    }    
}

void pad_bitstream(uint64_t* bitstream)
{
    padding = ((BUFFER_SIZE * 8) - used) % 64;
    printf("Used: %i\n", used);
    
    printf("Padding: %i\n", padding);

    if(padding != 0)
    {
        write_to_bitstream(0, bitstream, padding);
        if(padding + used < BUFFER_SIZE * 8 && used != 0)
            write_to_disk(bitstream);
    }

    return;
}

void depad_bitstream(uint64_t* bitstream)
{
    printf("Decoding used: %i\n", used);
    printf("Decoding padding: %i\n", padding);

    uint16_t throw_away = 0;
    if(padding != 0)
    {
        printf("Grabbing tw\n");
        throw_away = read_from_bitstream(bitstream, padding);
    }

    printf("Throw_away: %i\n", throw_away);

    return;
}

uint16_t get_size()
{
    return ceil((float) BUFFER_SIZE / 8);
}

void reset_bin_file(void)
{
    //Reset bin file
    FILE *fptr;
    fptr = fopen("output/encoded.bin","wb");  // w for write, b for binary
    int* empty = calloc(1, sizeof(uint64_t));
    fwrite(empty, sizeof(uint64_t), 1, fptr);
    fclose(fptr);
}