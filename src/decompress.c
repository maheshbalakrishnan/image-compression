#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char BYTE;

#define JPEG_START_OF_IMAGE_FIRST   0xFF
#define JPEG_START_OF_IMAGE_SECOND  0xD8

typedef struct _JFIFHeader
{
  BYTE SOI[2];          /* 00h  Start of Image Marker     */
  BYTE APP0[2];         /* 02h  Application Use Marker    */
  BYTE Length[2];       /* 04h  Length of APP0 Field      */
  BYTE Identifier[5];   /* 06h  "JFIF" (zero terminated) Id String */
  BYTE Version[2];      /* 07h  JFIF Format Revision      */
  BYTE Units;           /* 09h  Units used for Resolution */
  BYTE Xdensity[2];     /* 0Ah  Horizontal Resolution     */
  BYTE Ydensity[2];     /* 0Ch  Vertical Resolution       */
  BYTE XThumbnail;      /* 0Eh  Horizontal Pixel Count    */
  BYTE YThumbnail;      /* 0Fh  Vertical Pixel Count      */
} JFIFHEAD, *pJFIFHEAD;

typedef struct _JFIFExtension
{
BYTE   APP0[2];           /* 00h  Application Use Marker */
BYTE   Length[2];         /* 02h  Length of APP0 Field   */
BYTE   Identifier[5];     /* 04h  "JFXX" (zero terminated) Id String */
BYTE   ExtensionCode;     /* 09h  Extension ID Code      */
} JFIFEXTENSION, *pJFIFEXTENSTION;

int main(int argc, char* argv[]) {
    FILE* pFile = fopen(argv[1], "r");
    if (pFile == NULL) {
        printf("ERR: Failed to open file.");
        return (1);
    }

      // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    long lSize = ftell (pFile);
    rewind (pFile);

    printf("File size: %d bytes\n", lSize);
    
    BYTE* pBuffer = (BYTE*) malloc(sizeof(BYTE) * lSize);
    if (pBuffer == NULL) {
        printf("ERR: Failed to allocate buffer.");
        return (1);
    }

    size_t result = fread (pBuffer, 1, lSize, pFile);
    if (result != lSize) {
        printf("ERR: Reading error");
        exit (3);
    }

    BYTE* pImg = pBuffer;

    pJFIFHEAD pJpegHeader = (JFIFHEAD*) malloc(sizeof(JFIFHEAD));
    if (pJpegHeader == NULL) {
        printf("ERR: Failed to allocate buffer.");
        return (1);
    }

    memcpy(pJpegHeader, pImg, sizeof(JFIFHEAD));
    pImg += sizeof(JFIFHEAD);

    printf("\nJPEG Header analysis");
    printf("\n**** ****** ********");
    printf("\nStart of image bytes: 0x%x, 0x%x", pJpegHeader->SOI[0], pJpegHeader->SOI[1]);
    printf("\nApp use markers: 0x%x, 0x%x", pJpegHeader->APP0[0], pJpegHeader->APP0[1]);
    printf("\nLength: 0x%x, 0x%x", pJpegHeader->Length[0], pJpegHeader->Length[1]);
    printf("\nIdentifier: %s", (char*)&pJpegHeader->Identifier);
    printf("\nVersion: 0x%x, 0x%x", pJpegHeader->Version[0], pJpegHeader->Version[1]);
    printf("\nUnits: 0x%x", pJpegHeader->Units);
    printf("\nResolution: %d x %d px", *((short*)&pJpegHeader->Xdensity), *((short*)&pJpegHeader->Ydensity));
    printf("\nThumbnail pixel resolution: %d x %d px", pJpegHeader->XThumbnail, pJpegHeader->YThumbnail);

    if ((char)0xFF == *pImg) {
        printf("\nJFIF Extension block found\n");
        
        pJFIFEXTENSTION pExtension = (JFIFEXTENSION*) malloc(sizeof(JFIFEXTENSION));
        if (pExtension == NULL) {
            printf("ERR: Failed to allocate buffer.");
            return (1);
        }

        memcpy(pExtension, pImg, sizeof(JFIFEXTENSION));
        pImg += sizeof(JFIFEXTENSION);

        printf("\nJPEG Extension analysis");
        printf("\n**** ********* ********");
        printf("\nApp use markers: 0x%x, 0x%x", pExtension->APP0[0], pExtension->APP0[1]);
        printf("\nLength: 0x%x, 0x%x", pExtension->Length[0], pExtension->Length[1]);
        printf("\nIdentifier: %s", (char*)&pExtension->Identifier);
        printf("\nExtension code: %x", (char*)&pExtension->ExtensionCode);
    }

    
    
    printf("\nExtension format: %s", ((char*)(pImg + 4)));

    printf("\n\nAll done\n");

    fclose(pFile);
    free(pBuffer);
    free(pJpegHeader);
    return 0;
}