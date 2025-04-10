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
  m_E = (Input[1] & 0x80) >> 7;
  m_S = (Input[1] & 0x40) >> 6;
  m_T = (Input[1] & 0x20) >> 5;
  m_PID = ((Input[1] & 0x1F) << 8) | Input[2]; //Input[2] & 0xFF
  m_TSC = (Input[3] & 0xC0) >> 6;
  m_AFC = (Input[3] & 0x30) >> 4;
  m_CC = Input[3] & 0x0F;
  return 4;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  printf("SB=%2d E=%d S=%d P=%d PID=%4d TSC=%d AF=%d CC=%2d",
         m_SB, m_E, m_S, m_T, m_PID, m_TSC, m_AFC, m_CC);
}

//=============================================================================================================================================================================

/// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset()
{
  m_AdaptationFieldControl = 0;
  m_AdaptationFieldLength = 0;
  m_DC = 0; 
  m_RA = 0;
  m_SP = 0;
  m_PR = 0;
  m_OR = 0;
  m_SF = 0;
  m_TP = 0;
  m_EX = 0;
  m_PCR = 0;
  m_OPCR = 0;
  m_StuffingBytes = 0;
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl)
{
   if (PacketBuffer == nullptr) {
        std::cerr << "Error: Adaptation field buffer is null!" << std::endl;
        return -1;
    }

    if (AdaptationFieldControl != 2 && AdaptationFieldControl != 3) {
        return -1; 
    }

    m_AdaptationFieldLength = PacketBuffer[4];
    m_AdaptationFieldControl = AdaptationFieldControl;

    uint8_t index = 5;  // Pozycja bajtu flag
    
    m_DC = (PacketBuffer[index] & 0x80) >> 7;
    m_RA = (PacketBuffer[index] & 0x40) >> 6;
    m_SP = (PacketBuffer[index] & 0x20) >> 5;
    m_PR = (PacketBuffer[index] & 0x10) >> 4;
    m_OR = (PacketBuffer[index] & 0x08) >> 3;
    m_SF = (PacketBuffer[index] & 0x04) >> 2;
    m_TP = (PacketBuffer[index] & 0x02) >> 1;
    m_EX = (PacketBuffer[index] & 0x01);
    
    index++;

    if (m_PR) {
        //teraz packetBuffer[6]
        m_PCR_base = (PacketBuffer[index] << 25) |  //caly 6 to 8 bit
                     (PacketBuffer[index + 1] << 17) | //caly 7 to 16bit
                     (PacketBuffer[index + 2] << 9)  | //caly 8 to 24bit
                     (PacketBuffer[index + 3] << 1)  | //caly 9 to 32bit
                     ((PacketBuffer[index + 4] & 0x80) >> 7); 
        //pomijam 6 bitów zarezerwowanych wiec do extension zostaje mi ostani bit PacketBuffer[10]
        m_PCR_extension = ((PacketBuffer[index + 4] & 0x01) << 8) | PacketBuffer[index + 5];
        
        m_PCR = (m_PCR_base * 300) + m_PCR_extension;
                
        index += 6; // Przesuwamy indeks o 6 bajtów (5 bajtów PCR + 1 bajt rezerwowy)
    } else {
        m_PCR = 0;
    }

    if (m_OR) { 
      m_OPCR_base = (PacketBuffer[index] << 25) | 
                    (PacketBuffer[index + 1] << 17) | 
                    (PacketBuffer[index + 2] << 9)  | 
                    (PacketBuffer[index + 3] << 1)  | 
                    ((PacketBuffer[index + 4] & 0x80) >> 7); 
      
      // Parsowanie OPCR extension
      m_OPCR_extension = ((PacketBuffer[index + 4] & 0x01) << 8) | PacketBuffer[index + 5];
      
      m_OPCR = (m_OPCR_base * 300) + m_OPCR_extension;  // Ostateczna wartość OPCR

      index += 6; // Przesuwamy indeks o 6 bajtów (5 bajtów OPCR + 1 bajt rezerwowy)
    } else {
      m_OPCR = 0;  // Jeśli flaga OPCR nie jest ustawiona, ustawiamy OPCR na 0
    }

    m_StuffingBytes = m_AdaptationFieldLength - (index - 5);
    if (m_StuffingBytes < 0) {
        m_StuffingBytes = 0;
    }

    return m_AdaptationFieldLength + 1;
}


// @brief Print all adaptation field fields
void xTS_AdaptationField::Print() const
{
    printf("AF:L=%2d DC=%d RA=%d SP=%d PR=%d OR=%d SF=%d TP=%d EX=%d",
           m_AdaptationFieldLength, m_DC, m_RA, m_SP, m_PR, m_OR, m_SF, m_TP, m_EX);

    // Jeśli jest PCR, drukujemy jego wartość
    if (m_PR) {
        printf(" PCR=%lu", m_PCR);
        printf(" (Time=%.6f s)", static_cast<double>(m_PCR) / ExtendedClockFrequency_Hz);
    }

    // Jeśli jest OPCR, drukujemy jego wartość
    if (m_OR) {
        printf(" OPCR=%lu", m_OPCR);
        printf(" (Time=%.6f s)", static_cast<double>(m_OPCR) / ExtendedClockFrequency_Hz);
    }

    // Drukujemy ilość stuffing bytes
    printf(" StuffingBytes=%d", m_StuffingBytes);

}

//=============================================================================================================================================================================

void xPES_PacketHeader::Reset()
{
  m_PacketStartCodePrefix = 0;
  m_StreamId = 0;
  m_PacketLength = 0;
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input)
{
  if (Input == nullptr) {
    return -1;
  }
  m_PacketStartCodePrefix = (Input[0] << 16) | (Input[1] << 8) | Input[2];
  m_StreamId = Input[3];
  m_PacketLength = (Input[4] << 8) | Input[5];
  return 6;
}

void xPES_PacketHeader::Print() const
{
  printf(" PES:PSCP=%1d SID=%3d L=%4d",
         m_PacketStartCodePrefix, m_StreamId, m_PacketLength);
}

//=============================================================================================================================================================================
// Constructor: initialize variables and buffer pointer
xPES_Assembler::xPES_Assembler()
    : m_PID(-1),
      m_Buffer(nullptr),
      m_BufferSize(0),
      m_DataOffset(0),
      m_LastContinuityCounter(-1),
      m_Started(false)
{
    // Optionally, allocate initial memory or leave it for Init()
}

// Destructor: free allocated memory
xPES_Assembler::~xPES_Assembler() {
    if (m_Buffer) {
        delete[] m_Buffer;
        m_Buffer = nullptr;
    }
}


void xPES_Assembler::Init(int32_t PID)
{
    
}



xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField)
{
  uint8_t curentCC = PacketHeader->getCC();
  if (m_LastContinuityCounter >= 0 && (m_LastContinuityCounter + 1) % 16 != curentCC) {
    return eResult::StreamPackedLost;
  }
  m_LastContinuityCounter = curentCC; //sprawdzenie ciągłości pakietów

  int payloadStart = 4; // TS nagłówek
    if (PacketHeader->hasAdaptationField()) {
        payloadStart += AdaptationField->getAdaptationFieldLength() + 1;
    }

    const uint8_t* payload = TransportStreamPacket + payloadStart;

  if (PacketHeader->getStartFlag() == 1) {//flaga startu poczatek PES
    m_PESH.Reset();
    m_PESH.Parse(payload);
    xBufferReset();
    m_BufferSize = m_PESH.getPacketLength();
    if (m_BufferSize > 0) {
      m_Buffer = new uint8_t[m_BufferSize];
    }
     xBufferAppend(payload + 6, m_PESH.getPacketLength());
    return eResult::AssemblingStarted;
  }
  return eResult::AssemblingContinue;
}



void xPES_Assembler::xBufferReset()
{
  if (m_Buffer != nullptr) {
      delete[] m_Buffer;
      m_Buffer = nullptr;
  }
  m_BufferSize = 0;
  m_DataOffset = 0;
}

void xPES_Assembler::xBufferAppend(const uint8_t *input, int32_t size) 
{
   if (m_DataOffset + size > m_BufferSize) {
        std::cerr << "Error: Buffer overflow!" << std::endl;
        return;
    }
    memcpy(m_Buffer + m_DataOffset, input, size); // Kopiowanie danych do bufora
    m_DataOffset += size; // Aktualizacja offsetu
}



    

