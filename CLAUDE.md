# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XiaoZhi ESP32 is an AI chatbot project running on ESP32 microcontrollers that provides voice interaction capabilities using large language models. The project supports 70+ ESP32 development boards and implements an MCP (Model Control Protocol) based architecture for AI-controlled hardware interaction.

## Development Commands

### Build and Flash

- `source ../esp-idf/export.sh` before idf.py command, otherwise idf.py will not be available, once this being executed, no need to run it again during the same session, do not add it into every idf.py command
- `idf.py build` - Build the project
- `idf.py flash` - Flash firmware to device
- `idf.py monitor` - Monitor serial output
- `idf.py menuconfig` - Configure project settings
- `idf.py merge-bin` - Create merged binary for distribution
- `python scripts/release.py [board_name]` - Build and package firmware for specific board

### Board-Specific Development
- Each board configuration is in `main/boards/[board-name]/`
- Use `idf.py set-target esp32s3` (or esp32, esp32c3, etc.) to set target chip
- Board type is automatically detected from build configuration

### Audio and Font Tools
- `python scripts/gen_lang.py` - Generate language assets
- `python scripts/mp3_to_ogg.sh` - Convert audio files
- `python scripts/p3_tools/` - Audio format conversion tools

## Architecture

### Core Components

**Application Layer** (`main/application.h/cc`):
- Main application singleton managing device state and event loops
- Handles voice interaction flow: wake word → ASR → LLM → TTS
- Manages audio service, protocol communication, and OTA updates

**Board Abstraction** (`main/boards/`):
- Each board inherits from `Board`, `WifiBoard`, `Ml307Board`, or `DualNetworkBoard`
- Board classes provide hardware-specific initialization and drivers
- Use `DECLARE_BOARD(ClassName)` macro to register boards

**Communication Protocols** (`main/protocols/`):
- WebSocket and MQTT+UDP protocols for server communication
- Stream-based audio transmission using OPUS codec
- Protocol selection based on network capabilities

**Audio System** (`main/audio/`):
- ESP-SR based wake word detection and voice activity detection
- I2S audio interface with various codec support (ES8311, ES8374, etc.)
- Audio processor with optional AEC (Acoustic Echo Cancellation)

**MCP Server** (`main/mcp_server.h/cc`):
- Device-side MCP implementation for AI tool calling
- Property-based tool definitions with type validation
- Common tools: speaker control, screen brightness, battery status, GPIO control

**Display System** (`main/display/`):
- LVGL-based UI with LCD and OLED support
- Emoji and multilingual font rendering
- Adaptive brightness control

### Key Design Patterns

**Singleton Pattern**: Application, Board, and McpServer use singleton pattern for global access

**Factory Pattern**: Board creation uses factory pattern with `create_board()` function

**Observer Pattern**: Device state changes trigger events throughout the system

**Strategy Pattern**: Different audio codecs, displays, and network types are interchangeable

## Custom Board Development

### Creating a New Board
1. Create directory: `main/boards/my-board/`
2. Implement board class inheriting from appropriate base class
3. Create `config.h` with pin mappings and hardware configuration
4. Create `config.json` with build configuration
5. Use board-specific partition table if needed

### Required Files
- `my_board.cc` - Board implementation
- `config.h` - Hardware configuration
- `config.json` - Build settings
- `README.md` - Board documentation (optional)

### Board Configuration
- Pin mappings for I2S, I2C, SPI, GPIO
- Audio codec settings and sample rates
- Display parameters (resolution, orientation, backlight)
- Power management settings

## MCP Tool Development

### Adding Device Tools
```cpp
McpServer::GetInstance().AddTool(
    "tool_name",
    "Tool description",
    PropertyList({
        Property("param", kPropertyTypeInteger, 0, 100),
    }),
    [](const PropertyList& props) -> ReturnValue {
        // Tool implementation
        return "Success";
    }
);
```

### Property Types
- `kPropertyTypeBoolean` - Boolean values
- `kPropertyTypeInteger` - Integer with optional min/max range
- `kPropertyTypeString` - String values

## Build Configuration

### Target Platforms
- ESP32 (classic)
- ESP32-S3 (most common)
- ESP32-C3 (low cost)
- ESP32-C6
- ESP32-P4

### Memory Configuration
- Partition tables in `partitions/v1/` for different flash sizes
- PSRAM support for ESP32-S3 boards
- Optimized settings for memory-constrained devices

### SDK Requirements
- ESP-IDF 5.4 or higher
- Component dependencies managed via `idf_component.yml`
- LVGL 9.2.2 with custom configuration

## Audio Development

### Supported Codecs
- ES8311 (most common I2S codec)
- ES8374, ES8388, ES8389 (audio codecs)
- ES7210 (microphone array)
- ADC/PDM microphones

### Wake Word Integration
- ESP-SR library for offline wake word detection
- Custom wake word training supported
- AFE (Audio Front End) for noise reduction

### Audio Processing
- 16kHz/24kHz sample rates depending on board
- OPUS encoding for network transmission
- Voice Activity Detection (VAD)
- Optional Acoustic Echo Cancellation

## Testing and Debugging

### Serial Monitoring
- Use `idf.py monitor` for real-time debugging
- Log levels configurable per component
- Audio debugging server available: `python scripts/audio_debug_server.py`

### Common Issues
- Ensure correct I2S pin configuration for audio
- Check I2C addresses for codec chips
- Verify partition table matches flash size
- Audio issues often related to PA (Power Amplifier) enable pin

## Network Protocols

### WebSocket Protocol
- Real-time bidirectional communication
- JSON message format
- Automatic reconnection handling

### MQTT+UDP Hybrid
- MQTT for control messages
- UDP for audio streaming
- Fallback option for restricted networks

### 自定义
- 如果查不到需要的资料，请提出问题，不要瞎写