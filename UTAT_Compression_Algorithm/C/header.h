#ifndef HEADER
#define HEADER

//Struct Definitions
typedef struct imageMetadata
{
    //Essential
    uint16_t userDefined;
    uint16_t xSize;
    uint16_t ySize;
    uint16_t zSize;

    uint16_t  sampledType;           //Binary value 0/1
    uint16_t  largeDynamicRangeFlag; //Binary value 0/1
    uint16_t  dynamicRange;          //Only 4 bits used

    uint16_t  sampledEncodingOrder;     //Binary value 0/1
    uint16_t subframeInterleavingDepth;

    uint16_t  outputWordSize;                 //Only 3 bits used
    uint16_t  entropyCoderType;               //Only 2 bits used
    uint16_t  quantizerFidelityControlMethod; //Only 2 bits used

    uint16_t  supplementaryInformationTableCount; //Only 4 bits used

    //Supplementary Information Tables
    uint8_t tableType;      //Only 2 bits used
    uint8_t tablePurpose;   //Only 4 bits used
    uint8_t tableStructure; //Only 2 bits used
    
    uint8_t suppUserDefinedData; //Only 4 bits used

    //Table data subbblock

} imageMetadata;

typedef struct predictorMetadata
{
    //Primary

    //Weight Tables
     
    //Quantization

    //Sample Representative

} predictorMetadata;

typedef struct entropyCodingMetadata
{
    //Fill in
} entropyCodingMetadata;

typedef struct encodingParams
{
    imageMetadata image;
    predictorMetadata predictor;
    entropyCodingMetadata entropy;

} encodingParams;


//Function Definitions
void encode_imageMetadata(imageMetadata* params);
void decode_imageMetadata(imageMetadata* params);

//Image Metadata Global Information
uint8_t imageMetadataSize = 13;
uint8_t imageMetadataWidths[] = {8, 16, 16, 16, 1, 1, 4, 1, 16, 3, 2, 2, 4};


#endif