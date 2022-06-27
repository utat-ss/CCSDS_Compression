#ifndef HEADER
#define HEADER

int encodeHeader(void);
int decodeHeader(void);
int getConfig(void);


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

} entropyCodingMetadata;

typedef struct encodingParams
{
    imageMetadata image;
    predictorMetadata predictor;
    entropyCodingMetadata entropy;

} encodingParams;




//Image Metadata
//Essential
const uint8_t  userDefined = 1;
const uint16_t xSize = 5675;
const uint16_t ySize = 876;
const uint16_t zSize = 756;

const uint8_t  sampledType = 1;           //Binary value 0/1
const uint8_t  largeDynamicRangeFlag = 1; //Binary value 0/1
const uint8_t  dynamicRange = 31;          //Only 4 bits used

const uint8_t  sampledEncodingOrder = 1;     //Binary value 0/1
const uint16_t subframeInterleavingDepth = 765;

const uint8_t  outputWordSize = 2;                 //Only 3 bits used
const uint8_t  entropyCoderType = 1;               //Only 2 bits used
const uint8_t  quantizerFidelityControlMethod = 1; //Only 2 bits used

const uint8_t  supplementaryInformationTableCount = 2; //Only 4 bits used

//Supplementary Information Tables
const uint8_t tableType = 1;      //Only 2 bits used
const uint8_t tablePurpose = 1;   //Only 4 bits used
const uint8_t tableStructure = 1; //Only 2 bits used

const uint8_t suppUserDefinedData = 1; //Only 4 bits used

uint8_t imageMetadataSize = 13;
uint8_t imageMetadataWidths[] = {8, 16, 16, 16, 1, 1, 4, 1, 16, 3, 2, 2, 4};

#endif