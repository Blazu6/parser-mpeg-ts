# MPEG-2 Transport Stream Parser

This project provides an implementation of an MPEG-2 Transport Stream (TS) parser that processes the structure of MPEG-2 TS packets, including adaptation fields and PES (Packetized Elementary Stream) headers. It allows for extracting and handling various components of the MPEG-2 TS, such as PID (Packet Identifier), continuity counters, adaptation fields, and PES packets.

## Features

- **MPEG-2 TS Packet Parsing**: The parser reads and processes MPEG-2 transport stream packets. It handles different fields in each packet's header and extracts useful information such as PID, continuity counter, and adaptation field control.
  
- **Adaptation Field Handling**: Support for adaptation fields in TS packets, including flags and optional fields such as PCR (Program Clock Reference) and OPCR (Original Program Clock Reference).
  
- **PES Packet Assembly**: The parser assembles PES packets from TS packets. It handles the splitting of PES packets across multiple TS packets, maintaining the continuity of the stream.
  
- **Error Handling**: The parser detects and reports errors such as missing or out-of-order packets, invalid PIDs, and incomplete PES packet assemblies.

## Usage

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/Blazu6/parser-mpeg-ts.git
   cd parser-mpeg-ts

2.  **Build the Project: The project uses CMake to manage the build process. To build the project, follow these steps**:

    ```bash
    mkdir build
    cd build
    cmake ..
    make

3. **Run the Parser: Once the project is built, you can run it with the input MPEG-2 TS file**:
    
    ```bash
    ./ts-parser <input_ts_file>
