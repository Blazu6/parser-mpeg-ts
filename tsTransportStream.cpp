#include "tsTransportStream.h"

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================


/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
  m_SB = 0;  // Sync Byte
  m_E = 0;   // Error flag
  m_S = 0;   // Start flag
  m_T = 0;   // Priority flag
  m_PID = 0; // Packet Identifier
  m_TSC = 0; // Transport Scrambling Control
  m_AFC = 0; // Adaptation Field Control
  m_CC = 0;  // Continuity Counter
}

/**
  @brief Parse all TS packet header fields
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure) 
  Nie swapuje bo w parsie juz jest swapowane recznie
 */
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
  if (Input == nullptr) {
    return -1;
  }
  m_SB = Input[0];
  if (m_SB != 0x47) { // Sync byte should be 0x47
    return -1;
  }
  m_E = (Input[1] & 0x80) >> 7;//Input[1]x10000000 to daje 128(10) a my chcemy 1 wiec przesuwamy
  m_S = (Input[1] & 0x40) >> 6;//Input[1]x01000000 to daje 64(10) a my chcemy 1 wiec przesuwamy
  m_T = (Input[1] & 0x20) >> 5;//Input[1]x00100000 to daje 32(10) a my chcemy 1 wiec przesuwamy
  m_PID = ((Input[1] & 0x1F) << 8) | Input[2];//Input[1]x00011111 i przesuwam w lewo tworzac miejsce na 8 bitów z bajtu2
  m_TSC = (Input[3] & 0xC0) >> 6;//Input[3]x11000000 to daje 192(10) ale przesuwamy o 6 wiec od 0 do 3 wartosci
  m_AFC = (Input[3] & 0x30) >> 4;//Input[3]x00110000 bit 4 i 5 przeuswamy o 4 pozycje wiec wartosci od 0 do 3
  m_CC = Input[3] & 0x0F;//Input[3]x00001111 to daje 15(10) ostanie 4 bity
  return 4;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  std::cout << "SB=" << (int)m_SB << " ";
  std::cout << "E=" << (int)m_E << " ";
  std::cout << "S=" << (int)m_S << " ";
  std::cout << "T=" << (int)m_T << " ";
  std::cout << "PID=" << (int)m_PID << " ";
  std::cout << "TSC=" << (int)m_TSC << " ";
  std::cout << "AFC=" << (int)m_AFC << " ";
  std::cout << "CC=" << (int)m_CC;
}

//=============================================================================================================================================================================
