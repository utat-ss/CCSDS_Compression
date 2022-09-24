#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "header.h"
#include "bitstreamer.c"

uint64_t* encode_imageMetadata(imageMetadata* params, uint16_t size)
{
    uint16_t* cursor = (uint16_t*) params;
    uint64_t* bitstream = calloc(sizeof(uint64_t), get_size());
    for(int i = 0; i <= 12; ++i)
    {
        printf("Adding variable to bitstream:%d\n", *(cursor + i));
        add_to_bitstream(*(cursor + i), bitstream, size, imageMetadataWidths[i]);
        PRINT_BITSTREAM
    }
    printf("Final Bitstream: ");
    PRINT_BITSTREAM
    
    padding = ((BUFFER_SIZE * 8) - used) % 64;
    printf("Used: %i\n", used);
    
    printf("Padding: %i\n", padding);

    if(padding != 0)
    {
        add_to_bitstream(0, bitstream, size, padding);
        if(padding + used < BUFFER_SIZE * 8 && used != 0)
            dump_to_disk(bitstream, 1);
    }

    return bitstream;
}

void decode_imageMetadata(imageMetadata* params, uint16_t size)
{
    uint64_t* bitstream = calloc(get_size(), sizeof(uint64_t));
    read_from_disk(bitstream, size);

    //PRINT_BITSTREAM
    
    uint16_t* cursor = (uint16_t*) params;
    printf("Decoding used: %i\n", used);
    printf("Decoding padding: %i\n", padding);

    uint16_t throw_away = 0;
    if(padding != 0)
    {
        printf("Grabbing tw\n");
        throw_away = decode_bitstream(bitstream, get_size(), padding);
    }

    printf("Throw_away: %i\n", throw_away);

    printf("Before grabbing\n"); 
    PRINT_BITSTREAM

    for(int i = 12; i >= 0; --i)
    {
        //printf("%d\n", *(cursor + i));
        uint16_t value = decode_bitstream(bitstream, get_size(), imageMetadataWidths[i]);
        //printf("Grabbed: %hu\n", value);
        printf("Grabbed: %hx\n", value);
        *(cursor + i) = value;
    }
}

int main(void)
{
    printf("==== Image Metadata Test Unit ====\n");
    {
        reset_bin_file();

        imageMetadata* params = calloc(sizeof(imageMetadata), 1);
       
        params->userDefined = 1;
        params->xSize = 434;
        params->ySize = 325;
        params->zSize = 3459;

        params->sampledType= 1;           //Binary value 0/1
        params->largeDynamicRangeFlag= 0; //Binary value 0/1
        params->dynamicRange = 1;          //Only 4 bits used

        params->sampledEncodingOrder = 0;     //Binary value 0/1
        params->subframeInterleavingDepth = 65535;

        params->outputWordSize = 2;                 //Only 3 bits used
        params->entropyCoderType = 3;               //Only 2 bits used
        params->quantizerFidelityControlMethod = 2; //Only 2 bits used

        params->supplementaryInformationTableCount = 15; //Only 4 bits used
        
        uint16_t size = get_size();
        uint64_t* data = encode_imageMetadata(params, size);
        
        printf("\nFINAL DISK CONTENT\n");
        system("./read");

        imageMetadata* params_empty = calloc(sizeof(imageMetadata), 1);
        printf("\nDECODING\n");
        decode_imageMetadata(params_empty, size);

        if(memcmp(params_empty, params, 26) == 0)
            printf("Passed!\n");
        else
            printf("Failed!\n");
    }

    return 0;
}