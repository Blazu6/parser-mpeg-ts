#include "tsCommon.h"
#include "tsTransportStream.h"
#include <iostream>
#include <fstream>

//=============================================================================================================================================================================

int main(int argc, char *argv[], char *envp[])
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
    return EXIT_FAILURE;
  }

  FILE *inputFile = fopen(argv[1], "rb");
  if (!inputFile) {
    std::cerr << "Error: Could not open file " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  xTS_PacketHeader    TS_PacketHeader;
  xTS_AdaptationField TS_AdaptationField;

  int32_t TS_PacketId = 0;
  uint8_t TS_PacketBuffer[xTS::TS_PacketLength];
  size_t bytesRead;

while (!feof(inputFile) && TS_PacketId < 34) {
    bytesRead = fread(TS_PacketBuffer, 1, xTS::TS_PacketLength, inputFile);
    if (bytesRead != xTS::TS_PacketLength) {
      if (feof(inputFile)) {
        break; // Koniec pliku
      } else {
        std::cerr << "Error: Could not read a full TS packet" << std::endl;
        fclose(inputFile);
        return EXIT_FAILURE;
      }
    }

    TS_PacketHeader.Reset();
    if (TS_PacketHeader.Parse(TS_PacketBuffer) != xTS::TS_HeaderLength) {
      std::cerr << "Error: Failed to parse TS packet header" << std::endl;
      fclose(inputFile);
      return EXIT_FAILURE;
    }

    TS_AdaptationField.Reset();
    

    if (TS_PacketHeader.hasAdaptationField()){
      TS_AdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAFC());
    }
    

    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();
    if (TS_PacketHeader.hasAdaptationField()) {
      printf(" ");
      TS_AdaptationField.Print();
    }
    printf("\n");

    TS_PacketId++;
}

  fclose(inputFile);

  return EXIT_SUCCESS;
}

//=============================================================================================================================================================================