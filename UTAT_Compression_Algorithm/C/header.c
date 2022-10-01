#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "bitstreamer.h"
#include "header.h"

void encode_imageMetadata(imageMetadata* params)
{
    uint64_t* bitstream = init_bitstream();

    uint16_t* cursor = (uint16_t*) params;

    for(int i = 0; i < imageMetadataSize; ++i)
    {
        printf("Adding variable to bitstream:%d\n", *(cursor + i));
        write_to_bitstream(*(cursor + i), bitstream, imageMetadataWidths[i]);
        PRINT_BITSTREAM
    }
    printf("Final Bitstream: ");
    PRINT_BITSTREAM
    
    pad_bitstream(bitstream);

    free(bitstream);
    return;
}

void decode_imageMetadata(imageMetadata* params)
{
    uint64_t* bitstream = init_bitstream();
    read_from_disk(bitstream);

    //PRINT_BITSTREAM
    
    uint16_t* cursor = (uint16_t*) params;

    depad_bitstream(bitstream);

    printf("Before grabbing\n"); 
    PRINT_BITSTREAM

    for(int i = imageMetadataSize - 1; i >= 0; --i)
    {
        //printf("%d\n", *(cursor + i));
        uint64_t value = read_from_bitstream(bitstream, imageMetadataWidths[i]);
        //printf("Grabbed: %hu\n", value);
        printf("Grabbed: %llx\n", value);
        *(cursor + i) = (uint16_t) value;
    }

    free(bitstream);
    return;
}

int main(void)
{
    printf("==== Image Metadata Test Unit ====\n");
    {
        reset_bin_file();
        init_bitstreamer_variables();

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
        
        encode_imageMetadata(params);
        
        printf("\nFINAL DISK CONTENT\n");
        system("./read");

        imageMetadata* params_empty = calloc(sizeof(imageMetadata), 1);
        printf("\nDECODING\n");
        decode_imageMetadata(params_empty);

        if(memcmp(params_empty, params, 26) == 0)
            printf("Passed!\n");
        else
            printf("Failed!\n");
    }

    return 0;
}