# JPL-MiniPulse

A real-time NASA Deep Space Network monitor built on ESP32.

<!-- TODO: Add a photo of the assembled device here -->
<!-- ![JPL-MiniPulse](path/to/device-photo.jpg) -->

Fetches live spacecraft communication data from the DSN Now XML feed, classifies signal data rates into six tiers, and renders corresponding meteor animations across four WS2812B LED rings. Spacecraft names scroll on the inner ring using a custom bitmap font. Data acquisition and rendering run on separate ESP32 cores via FreeRTOS.

**Firmware version:** 1.0.3

---

## Table of Contents

1. [System Architecture](#system-architecture)
2. [Hardware](#hardware)
3. [Build and Flash](#build-and-flash)
4. [WiFi Configuration](#wifi-configuration)
5. [OTA Firmware Updates](#ota-firmware-updates)
6. [Data Pipeline](#data-pipeline)
7. [Rate Classification and Animation](#rate-classification-and-animation)
8. [Spacecraft Data Management](#spacecraft-data-management)
9. [Configuration Reference](#configuration-reference)
10. [Project Structure](#project-structure)
11. [Dependencies](#dependencies)

---

## System Architecture

MiniPulse runs on both cores of the ESP32 using FreeRTOS:

```
Core 0                                Core 1
──────                                ──────
getData() task                        setup() / loop()
├─ fetchData()                        ├─ doWiFiManager()
│  ├─ HTTP GET DSN XML                ├─ Dequeue CraftQueueItem
│  └─ Save to /temp.xml (LittleFS)   ├─ scrollLetters() — inner ring
├─ parseData()                        ├─ doRateBasedAnimation()
│  ├─ tinyxml2 XML parse              │  ├─ Outer ring (down signal)
│  ├─ checkBlacklist()                │  └─ Middle ring (up signal)
│  ├─ callsignToName()                ├─ drawMeteors()
│  └─ findSignals()                   ├─ updateBottomPixels()
└─ xQueueSend(CraftQueueItem)        └─ FastLED.show()
          │                                    ▲
          └──────── FreeRTOS Queue ────────────┘
                   (max 5 items)
```

**Core 0** runs `getData()` as a pinned FreeRTOS task (6144-byte stack, priority 0). It fetches XML from NASA's DSN feed and parses it into `CraftQueueItem` structs every 10 seconds.

**Core 1** runs `setup()` and `loop()`. It handles WiFi management, dequeues spacecraft data, renders scrolling text, and drives LED animations at 60 FPS.

Inter-core communication uses a FreeRTOS queue (`xQueueCreate(5, sizeof(CraftQueueItem))`). The `CraftQueueItem` memory pool is protected by a semaphore-guarded free list.

| Constant | Value | Purpose |
|----------|-------|---------|
| Data fetch interval | 10,000 ms | Time between HTTP requests |
| Frame rate | 60 FPS | LED animation target |
| Display duration | 20,000 ms minimum | Time each spacecraft is shown |
| Inter-craft delay | 8,000 ms | Pause between spacecraft transitions |
| Watchdog timeout | 60 seconds | ESP32 task watchdog timer |

---

## Hardware

### Pin Assignments

| GPIO | Function | Direction | Notes |
|------|----------|-----------|-------|
| 16 | Bottom LED strip | Output | Status indicator (5 pixels) |
| 17 | Outer LED strip | Output | Down signal animation (800 pixels) |
| 18 | Middle LED strip | Output | Up signal animation (800 pixels) |
| 19 | Inner LED strip | Output | Spacecraft name text (960 pixels) |
| 21 | WiFi reset button | Input | Active LOW, triggers config portal |
| 22 | Output enable | Output | LED power control |
| 32 | Potentiometer | ADC Input | Brightness control |

### LED Ring Configuration

| Strip | Total Pixels | Chunks | Pixels per Chunk | GPIO | Function |
|-------|-------------|--------|-------------------|------|----------|
| Outer | 800 | 10 | 80 | 17 | Down signal (spacecraft-to-Earth) meteors |
| Middle | 800 | 10 | 80 | 18 | Up signal (Earth-to-spacecraft) meteors |
| Inner | 960 | 12 | 80 | 19 | Scrolling spacecraft name text |
| Bottom | 5 | 1 | 5 | 16 | Status indicators |
| **Total** | **2,565** | | | | |

All strips use the WS2812B (NEOPIXEL) protocol.

### Brightness Control

A potentiometer on GPIO 32 provides analog brightness input. The raw ADC value is mapped to a range of 8-160 for `FastLED.setBrightness()`. Brightness can also be set through the WiFi configuration portal.

---

## Build and Flash

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)
- ESP32 development board
- USB cable for initial flash

### Build Environments

| Environment | Board | Partition Table | Upload Protocol | Use Case |
|-------------|-------|-----------------|-----------------|----------|
| `release` | esp32dev | `min_spiffs.csv` | esptool | Production builds |
| `debug` | esp32dev | `min_spiffs.csv` | esptool | Debug builds |
| `devdebug` | esp32doit-devkit-v1 | `huge_app.csv` | esp-prog (JTAG) | Development with JTAG debugger |

The `release` and `debug` environments use `min_spiffs.csv`, which provides dual OTA partitions required for over-the-air updates. The `devdebug` environment uses `huge_app.csv` to maximize application flash space, but does not support OTA.

### Commands

```bash
# Build release firmware
pio run -e release

# Build and upload via USB
pio run -e release -t upload

# Serial monitor (115200 baud)
pio device monitor -b 115200

# Build, upload, and monitor
pio run -e release -t upload && pio device monitor -b 115200
```

The serial monitor uses `colorize` and `esp32_exception_decoder` filters. Colored diagnostic output is enabled when `showSerial` is set to `true` in the device configuration.

---

## WiFi Configuration

### Initial Setup

On first boot, or when no saved credentials are found, the ESP32 starts a WiFi access point using WiFiManager. Connect to this AP to access the captive portal, where network credentials and device parameters can be configured.

The portal runs in non-blocking mode (`wm.startConfigPortal()`), allowing the device to continue operating while the portal is active.

### Portal Parameters

The configuration portal exposes the following adjustable parameters:

| Parameter | Key | Type | Description |
|-----------|-----|------|-------------|
| Serial output | `show_serial` | 0/1 | Enable verbose serial debug output |
| Diagnostics | `show_diagnostics` | 0/1 | Enable FPS and heap usage reporting |
| Brightness | `brightness` | 0-100 | LED brightness (mapped to 8-160 internally) |
| Dummy data | `force_dummy_data` | 0-2 | 0: live data, 1: static test data, 2: animation test data |
| Animation type | `force_animation_type` | 0-5 | Override automatic animation pattern selection |

### WiFi Reset Button

GPIO 21 (active LOW). Pressing the button starts the configuration portal if it is not already running. If the portal is already active, connection instructions are printed to serial.

### Configuration Storage

Device configuration is persisted to `/config.json` on the LittleFS filesystem. The file is created with default values if it does not exist at boot. See [Configuration Reference](#configuration-reference) for all fields.

### Web Portal Endpoints

When the configuration portal is active, the following HTTP endpoints are available:

| Endpoint | Method | Response |
|----------|--------|----------|
| `/` | GET | WiFi configuration portal page |
| `/get-remote-version-number` | GET | Remote firmware version string |
| `/get-latest-version-number` | GET | Current version, remote version, and update availability |
| `/trigger-firmware-update` | GET | Initiates OTA firmware update if a newer version exists |

---

## OTA Firmware Updates

### Overview

The device supports over-the-air firmware updates via HTTP. Updates are triggered manually through the web portal — there is no automatic update mechanism. The ESP32's dual OTA partition scheme (`min_spiffs.csv`) allows safe firmware swaps: the new binary is written to the inactive partition, and the device reboots into it only after a successful download.

### Version Scheme

The current firmware version is stored as a string constant at the top of `src/main.cpp`:

```cpp
const char* currentFirmwareVersion = "1.0.3";
```

The remote version is a plaintext file hosted at:

```
http://develop.kellerdigital.com/minipulse/latest_version.txt
```

Version comparison uses `strcmp` — any difference between the local and remote strings (including downgrades) triggers an available update. This is not semantic versioning; the strings are compared literally.

### Update Flow

```
User clicks "Update Firmware" in web portal
                    │
                    ▼
        GET /trigger-firmware-update
                    │
                    ▼
    checkFirmwareUpdateAvailable()
    ├── getRemoteFirmwareVersion()
    │   └── HTTP GET .../latest_version.txt
    │       └── Returns version string (e.g., "1.0.4")
    ├── strcmp(remote, currentFirmwareVersion)
    │   ├── Same → "Firmware is up to date" → STOP
    │   └── Different → Continue
    │
    ▼
    updateFirmwareOta()
    ├── httpUpdate.update(client, ".../firmware.bin")
    │   ├── HTTP_UPDATE_OK → ESP32 reboots into new firmware
    │   ├── HTTP_UPDATE_FAILED → Error logged, existing firmware continues
    │   └── HTTP_UPDATE_NO_UPDATES → Logged
    └── Progress reported to serial via onProgress callback
```

### Server-Side Setup

Two files must be present on the update server:

```
develop.kellerdigital.com/minipulse/
├── latest_version.txt    # Plain text, version string only (e.g., "1.0.4")
└── firmware.bin          # Compiled binary from PlatformIO
```

The `latest_version.txt` file must contain only the version string with no trailing whitespace or newlines. The comparison uses `strcmp`, so any extra characters will cause a mismatch.

### Building the Firmware Binary

```bash
pio run -e release
```

The compiled binary is output to:

```
.pio/build/release/firmware.bin
```

Upload this file to the server alongside the updated `latest_version.txt`.

### Partition Table Requirement

OTA updates require the `min_spiffs.csv` partition table, which provides two application partitions for safe firmware swapping. The `release` and `debug` environments use this partition table. The `devdebug` environment uses `huge_app.csv` and cannot perform OTA updates.

### Behavior During Update

- The `otaUpdateTriggered` flag pauses the `getData()` task on Core 0 during the update
- Download progress is reported to serial via the `update_progress` callback
- On success (`HTTP_UPDATE_OK`), the ESP32 reboots automatically into the new firmware
- On failure, the existing firmware continues running without interruption

### Release Checklist

1. Increment `currentFirmwareVersion` in `src/main.cpp` (line 1)
2. Build with `pio run -e release`
3. Upload `.pio/build/release/firmware.bin` to the update server
4. Update `latest_version.txt` on the server to match the new version string
5. Verify by hitting `/get-latest-version-number` on a running device

---

## Data Pipeline

### DSN XML Source

NASA's Deep Space Network publishes real-time spacecraft communication data as XML. The feed URL is configured in `FileUtils::config.wifiNetwork.serverName`. A random number is appended as a query parameter to prevent caching.

XML structure:

```xml
<dsn>
  <station friendlyName="Goldstone" name="gdscc">
    <dish name="DSS24">
      <downSignal spacecraft="JWST" dataRate="28000000" signalType="data" />
      <upSignal spacecraft="JWST" dataRate="16000" signalType="data" />
      <target name="JWST" />
    </dish>
  </station>
</dsn>
```

### Processing Steps

1. `getData()` on Core 0 calls `fetchData()` every 10 seconds
2. `fetchHTTPData()` performs an HTTP GET with a 5-second timeout, retrying up to 3 times
3. The XML response is saved to `/temp.xml` on LittleFS (max 20,480 bytes)
4. `parseData()` iterates through `<station>` > `<dish>` > `<target>` elements via tinyxml2
5. `checkBlacklist()` filters out excluded callsigns (test signals, ground stations)
6. `callsignToName()` resolves the callsign to a display name (e.g., `JWST` to `James Webb Space Telescope`)
7. `findSignals()` extracts `downSignal` and `upSignal` data rate attributes from the dish
8. `rateLongToRateClass()` converts the raw bps value to a rate class (0-6)
9. A `CraftQueueItem` is populated and sent to the inter-core queue via `xQueueSend`

### Fallback Behavior

| Condition | Behavior |
|-----------|----------|
| HTTP fetch fails 3 consecutive times | Switches to embedded dummy XML data |
| No targets found for 3 consecutive cycles | Switches to dummy data |
| Signal data rate is 0 bps | Uses a placeholder rate from `SpacecraftData::getPlaceholderRate()` |
| No placeholder rate defined for spacecraft | Falls back to `1.000e+03` (1,000 bps) |
| `forceDummyData` enabled via portal | Bypasses live data entirely |

### Memory Constraints

| Buffer | Max Size |
|--------|----------|
| `spacecraftNamesJson` | 5,120 bytes |
| `spacecraftBlacklistJson` | 1,024 bytes |
| `spacecraftPlaceholderRatesJson` | 6,144 bytes |
| XML download buffer (`MAX_XML_SIZE`) | 20,480 bytes |
| Spacecraft display name (`CraftQueueItem.nameArray`) | 100 characters |
| Spacecraft callsign (`CraftQueueItem.callsignArray`) | 40 characters |

---

## Rate Classification and Animation

### Rate Classes

Signal data rates (in bps) are mapped to one of seven classes, which determine animation density and complexity:

| Class | Threshold | Description |
|-------|-----------|-------------|
| 0 | 0 bps | No data — no animation |
| 1 | < 1 Kbps | Minimal |
| 2 | < 10 Kbps | Low |
| 3 | < 100 Kbps | Moderate |
| 4 | < 500 Kbps | High |
| 5 | < 2.5 Mbps | Very high |
| 6 | >= 2.5 Mbps | Maximum |

Rates above 1 Gbps (`MAX_RATE`) are capped to catch erroneous values from the XML feed.

### Animation Patterns

Higher rate classes progressively unlock additional animation types, layered on top of the base meteor pattern:

| Pattern | Available at Class | Description |
|---------|-------------------|-------------|
| Meteor | 1+ | Individual meteors with configurable tail decay |
| Ring Pulse | 2+ | Full-ring brightness pulse |
| Spiral | 3+ | Spiral pattern across LED chunks |
| Wave | 4+ | Wave propagation with configurable size |
| Zigzag | 5+ | Zigzag pattern across the strip |

Higher classes also increase meteor count, reduce timing offsets, and increase tail decay values, producing denser and more energetic visuals.

### Animation Timing

| Phase | Duration | Description |
|-------|----------|-------------|
| Name scroll | ~6 seconds | Spacecraft name scrolls across inner ring |
| Text-meteor gap | 6 seconds | Delay between text completion and meteor start |
| Animation cycle | ~3 seconds | Gap between repeated animation bursts |
| Minimum display | 20 seconds | Minimum time before transitioning to next spacecraft |
| Craft delay | 8 seconds | Pause after display ends before next spacecraft begins |

### Color Themes

Selectable via the configuration portal (`colorTheme` field):

| ID | Name | Letter Color | Meteor Color | Tail Hue | Tail Saturation |
|----|------|-------------|-------------|----------|-----------------|
| 0 | White | White | White | 0 | 0 |
| 1 | Cyber | Aqua | Aqua | 160 | 255 |
| 2 | Valentine | Pink | Pink | 192 | 255 |
| 3 | Moonlight | White | White | 160 | 127 |

---

## Spacecraft Data Management

All spacecraft data is managed in a single file:

```
lib/SpacecraftData/SpacecraftData.cpp
```

| Task | Function | Lines |
|------|----------|-------|
| Add spacecraft name | `createAndWriteNamesFile()` | 44-124 |
| Add to blacklist | `createAndWriteBlacklistFile()` | 215-218 |
| Add placeholder rates | `createAndWritePlaceholderRatesFile()` | 316-486 |

**Data source:** The official list of DSN spacecraft and callsigns is available from the [DSN Commitments Office](https://deepspace.jpl.nasa.gov/about/commitments-office/current-mission-set/). The Mission Contact List document has the most current spacecraft information.

### Adding a New Spacecraft

```cpp
// In createAndWriteNamesFile() around line 44-124:
spacecraftNamesJson["CALLSIGN"] = "Full Spacecraft Name";
```

### Blacklisting a Callsign

```cpp
// In createAndWriteBlacklistFile() around line 215-218:
spacecraftBlacklistJson["CALLSIGN"] = true;
```

### Adding Placeholder Rates

```cpp
// In createAndWritePlaceholderRatesFile() around line 316-486:
JsonArray craft = spacecraftPlaceholderRatesJson.createNestedArray("CALLSIGN");
craft.add("7.813e+00");   // ~8 bps
craft.add("4.000e+03");   // 4 kbps
```

### Reference Files

The `data_reference/spacecraft_data/` directory contains JSON files for documentation purposes only:

```
data_reference/spacecraft_data/
├── names.json            # Callsign → name mappings
├── blacklist.json        # Ignored callsigns
└── placeholder_rates.json # Fallback data rates
```

These files are not read at runtime. They exist for human reference in source control. The actual data comes from hardcoded values in `SpacecraftData.cpp`, which are written to the ESP32's LittleFS filesystem on each boot.

When updating `SpacecraftData.cpp`, consider updating the reference files to keep documentation in sync.

### Data Architecture

```
┌─────────────────────────────────────────────────────────────┐
│              lib/SpacecraftData/SpacecraftData.cpp          │
│                      (Source of Truth)                      │
│                                                             │
│   createAndWriteNamesFile()            Lines 44-124         │
│   createAndWriteBlacklistFile()        Lines 215-218        │
│   createAndWritePlaceholderRatesFile() Lines 316-486        │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼ Written on each boot
┌─────────────────────────────────────────────────────────────┐
│                     ESP32 LittleFS                          │
│                                                             │
│   /spacecraft_data/names.json                               │
│   /spacecraft_data/blacklist.json                           │
│   /spacecraft_data/placeholder_rates.json                   │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼ Loaded into memory
┌─────────────────────────────────────────────────────────────┐
│              DynamicJsonDocument (Runtime)                  │
│                                                             │
│   spacecraftNamesJson           (5120 bytes max)            │
│   spacecraftBlacklistJson       (1024 bytes max)            │
│   spacecraftPlaceholderRatesJson (6144 bytes max)           │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼ Queried by main.cpp
┌─────────────────────────────────────────────────────────────┐
│                    Lookup Functions                         │
│                                                             │
│   callsignToName("JWST")     → "James Webb Space Telescope" │
│   checkBlacklist("TEST")     → true                         │
│   getPlaceholderRate("MRO")  → "6.250e+01"                  │
└─────────────────────────────────────────────────────────────┘
```

### Data File Details

**names.json** — Maps DSN callsigns to human-readable spacecraft names:

| Callsign | Full Name |
|----------|-----------|
| `JWST` | James Webb Space Telescope |
| `MRO` | Mars Reconnaissance Orbiter |
| `VGR1` | Voyager 1 |

**blacklist.json** — Callsigns excluded from display:

| Callsign | Reason |
|----------|--------|
| `TEST` | Test signal |
| `DSN` | Network identifier |
| `RFC(VLBI)` | Radio astronomy calibration |

**placeholder_rates.json** — Fallback data rates (scientific notation, bps) used when the XML feed reports a rate of 0. A random entry from the array is selected for visual variety:

```json
{
  "JWST": ["1.250e+02", "2.500e+02", "4.000e+03", "2.500e+05"],
  "MRO": ["7.812e+00", "6.250e+01", "1.250e+02", "4.000e+03"]
}
```

If no placeholder rates are defined for a spacecraft, the default fallback is `1.000e+03` (1,000 bps).

---

## Configuration Reference

Configuration is stored in `/config.json` on LittleFS and loaded at boot via `FileUtils::initConfigFile()`. Missing files are created with default values.

The `FileUtils::Config` struct (defined in `lib/FileUtils/FileUtils.h`) contains:

### DebugUtils

| Field | Type | Description |
|-------|------|-------------|
| `testCores` | bool | Log core IDs to serial |
| `showSerial` | bool | Enable verbose serial output |
| `diagMeasure` | bool | Enable FPS and heap diagnostics |
| `disableWiFi` | bool | Skip WiFi connection entirely |
| `testLEDs` | bool | Run LED test pattern on boot |

### WifiNetwork

| Field | Type | Description |
|-------|------|-------------|
| `apSSID` | char[32] | Config portal access point SSID |
| `apPass` | char[32] | Config portal access point password |
| `serverName` | char[128] | DSN XML feed URL |
| `forceDummyData` | bool | Use placeholder data instead of live feed |
| `noTargetLimit` | int | Consecutive empty responses before fallback (default: 3) |
| `retryDataFetchLimit` | int | Failed HTTP requests before fallback (default: 3) |

### PinsHardware

| Field | Type | Description |
|-------|------|-------------|
| `outerPin` | int | GPIO for outer LED strip |
| `middlePin` | int | GPIO for middle LED strip |
| `innerPin` | int | GPIO for inner LED strip |
| `bottomPin` | int | GPIO for bottom LED strip |
| `wifiRst` | int | GPIO for WiFi reset button |
| `outputEnable` | int | GPIO for output enable |
| `potentiometer` | int | GPIO for brightness potentiometer |

### DisplayLED

| Field | Type | Description |
|-------|------|-------------|
| `brightness` | int | Global LED brightness |
| `fps` | int | Target frame rate |
| `outerPixelsTotal` | int | Outer ring pixel count (800) |
| `middlePixelsTotal` | int | Middle ring pixel count (800) |
| `innerPixelsTotal` | int | Inner ring pixel count (960) |
| `bottomPixelsTotal` | int | Bottom strip pixel count (5) |
| `outerChunks` | int | Outer ring segment count (10) |
| `middleChunks` | int | Middle ring segment count (10) |
| `innerChunks` | int | Inner ring segment count (12) |
| `bottomChunks` | int | Bottom strip segment count (1) |
| `craftDelay` | int | Pause between spacecraft transitions (ms) |
| `displayMinDuration` | int | Minimum display time per spacecraft (ms) |

### TextTypography

| Field | Type | Description |
|-------|------|-------------|
| `characterWidth` | int | Character pixel width (3 or 5) |
| `textMeteorGap` | int | Delay between text scroll and meteor animation start |
| `meteorOffset` | int | Pixel offset for meteor start position |

### TimersDelays

| Field | Type | Description |
|-------|------|-------------|
| `timerDelay` | int | Data fetch interval in milliseconds |

### Miscellaneous

| Field | Type | Description |
|-------|------|-------------|
| `colorTheme` | int | Active color theme (0-3) |

---

## Project Structure

```
JPL-MiniPulse/
├── src/
│   └── main.cpp                  # Application entry point (setup, loop, data fetch, animation)
├── lib/
│   ├── Animate/                  # Meteor animation engine (direction, decay, tail rendering)
│   ├── AnimationUtils/           # Color definitions (CHSV), potentiometer brightness control
│   ├── DevUtils/                 # Serial formatting (ANSI colors), boot banners, heap reporting
│   ├── FileUtils/                # LittleFS file I/O, config.json management, Config struct
│   ├── MathHelpers/              # Degree-to-16bit conversion, custom map(), fast exponentiation
│   ├── SpacecraftData/           # Spacecraft name/blacklist/rate lookups, CraftQueueItem struct
│   └── TextCharacters/           # Bitmap font definitions for LED text rendering
├── data_reference/
│   └── spacecraft_data/          # JSON reference files (not loaded at runtime)
├── docs/                         # Technical planning documents
├── platformio.ini                # Build configuration (3 environments)
└── .gitignore
```

The `scripts/` directory (gitignored) contains internal Python utilities for extracting and formatting spacecraft data from DSN mission PDFs. These are development tools and are not part of the firmware.

---

## Dependencies

### External Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson) | ^6.19.4 | JSON parsing and serialization |
| [WiFiManager](https://github.com/tzapu/WiFiManager) | latest | Captive portal WiFi configuration |
| [tinyxml2](https://github.com/leethomason/tinyxml2) | latest | Lightweight XML parsing for DSN feed |
| [FastLED](https://github.com/FastLED/FastLED) | ^3.5.0 | WS2812B LED strip control |

### ESP32 Framework Libraries

| Library | Purpose |
|---------|---------|
| `HTTPClient` | HTTP GET requests for DSN data and version checks |
| `HTTPUpdate` | OTA firmware binary download and flash |
| `LittleFS` | On-device filesystem for config and spacecraft data |
| `esp_task_wdt` | Watchdog timer management |
| `FreeRTOS` | Task creation, queues, semaphores (via ESP32 Arduino core) |
