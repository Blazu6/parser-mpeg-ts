#include "tsCommon.h"
#include "tsTransportStream.h"
#include <iostream>
#include <fstream>

bool isLittleEndian() {
  uint16_t num = 1;
  // Rzutowanie na wskaźnik typu char* pozwala na sprawdzenie najmniejszego bajtu
  return (*(reinterpret_cast<unsigned char*>(&num)) == 1);
}

//=============================================================================================================================================================================

int main(int argc, char *argv[ ], char *envp[ ])
{
  if (isLittleEndian()) {
    std::cout << "System is Little-Endian." << std::endl;
  } else {
      std::cout << "System is Big-Endian." << std::endl;
  }

  // TODO - open file
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
    return EXIT_FAILURE;
  }

  
  FILE *inputFile = fopen(argv[1], "rb");
  // TODO - check if file if opened
  if (!inputFile) {
    std::cerr << "Error: Could not open file " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  xTS_PacketHeader    TS_PacketHeader;

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
    if (TS_PacketHeader.Parse(TS_PacketBuffer) != 4) {
      std::cerr << "Error: Failed to parse TS packet header" << std::endl;
      return EXIT_FAILURE;
    }

    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();
    printf("\n");

    TS_PacketId++;
  }
  
  // TODO - close file
  fclose(inputFile);

  return EXIT_SUCCESS;
}

//=============================================================================================================================================================================
