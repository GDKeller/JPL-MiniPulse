# JPL-MiniPulse Codebase Analysis

**Date:** 2026-02-18
**Firmware version analyzed:** 1.0.3
**Branch:** `data/add-spacecraft`

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Architecture Analysis](#architecture-analysis)
3. [Code Quality Findings](#code-quality-findings)
4. [Security Assessment](#security-assessment)
5. [Performance and Resource Analysis](#performance-and-resource-analysis)
6. [Prioritized Remediation Roadmap](#prioritized-remediation-roadmap)

---

## Executive Summary

JPL-MiniPulse is a well-conceived embedded system that fetches live NASA Deep Space Network telemetry and renders spacecraft communication data as LED animations across 2,565 WS2812B pixels on four concentric rings. The architecture leverages ESP32 dual-core FreeRTOS effectively, with a clean producer/consumer split between data acquisition (Core 0) and rendering (Core 1).

**Quality Score: 6.5/10.** The system works and demonstrates strong domain understanding, but this analysis uncovered 4 critical bugs, 11 high-severity issues, and numerous medium-priority improvements across architecture, code quality, security, and performance.

### Key Findings at a Glance

| Category | Critical | High | Medium | Low |
|----------|----------|------|--------|-----|
| Code Quality / Bugs | 4 | 7 | 7 | - |
| Security | 2 | 4 | 5 | 3 |
| Performance | 1 | 4 | 6 | - |
| Architecture | - | 3 | 4 | - |

The most urgent issues are: a queue/task initialization race condition, `==` vs `=` bugs in semaphore tracking, a queue send type mismatch, OTA firmware updates over plain HTTP, and an unauthenticated firmware update trigger endpoint.

---

## Architecture Analysis

### Overall Pattern: Dual-Core Producer-Consumer

The system follows a classic producer-consumer pattern mediated by a FreeRTOS queue:

```
Core 0 (Producer)             FreeRTOS Queue            Core 1 (Consumer)
getData() task       --->   xQueueCreate(5)    --->    loop() dequeues
  fetchData()                                           updateAnimation()
  parseData()                                           scrollLetters()
  xQueueSend()                                          doRateBasedAnimation()
                                                        drawMeteors()
                                                        FastLED.show()
```

**Strengths:**
- Clean separation of I/O-bound work (HTTP fetch, XML parse) from real-time rendering
- Queue provides natural back-pressure (max 5 items)
- Watchdog timer integration prevents hangs on either core

**Weaknesses:**
- No formal state machine governs lifecycle transitions. States are tracked via ~10 scattered boolean flags (`nameScrollDone`, `animationInProgress`, `animateFirstCycleDown`, `dataStarted`, `portalRunning`, `otaUpdateTriggered`, `usingDummyData`, etc.)

### Module Organization

| Module | Location | LOC | Responsibility | Coupling |
|--------|----------|-----|----------------|----------|
| **main** | `src/main.cpp` | ~3,650 | Everything else | Monolith |
| **Animate** | `lib/Animate/` | ~220 | Meteor rendering engine | AnimationUtils, MathHelpers, FastLED |
| **AnimationUtils** | `lib/AnimationUtils/` | ~130 | Color definitions, brightness | FileUtils, FastLED, MathHelpers |
| **DevUtils** | `lib/DevUtils/` | ~180 | Serial formatting, ANSI colors | WiFiManager |
| **FileUtils** | `lib/FileUtils/` | ~560 | LittleFS I/O, config management | ArduinoJson, LittleFS |
| **MathHelpers** | `lib/MathHelpers/` | ~38 | Utility math | None (pure) |
| **SpacecraftData** | `lib/SpacecraftData/` | ~910 | Name/blacklist/rate lookups | FileUtils, DevUtils, ArduinoJson |
| **TextCharacters** | `lib/TextCharacters/` | ~1,780 | Bitmap font definitions | None (pure data) |

### Architectural Issues

#### A1. Monolithic main.cpp (~3,650 lines) — HIGH

`src/main.cpp` contains animation orchestration, data fetching, HTTP handling, XML parsing, WiFi management, OTA updates, queue management, semaphore pool management, serial debugging, and the main loop. At minimum these should be extracted:
- `DataFetcher` — `fetchData`, `fetchHTTPData`, `generateFetchUrl`, `handleHttpResponse`
- `DataParser` — `parseData`, `findSignals`, `rateLongToRateClass`
- `AnimationController` — `doRateBasedAnimation`, `createMeteor`, all animation patterns
- `WiFiController` — `doWiFiManager`, `webServerCallback`, `saveParamsCallback`
- `OTAUpdater` — `setupOtaUpdate`, `updateFirmwareOta`, `checkFirmwareUpdateAvailable`

#### A2. Configuration Duplication — HIGH

Compile-time constants in `main.cpp` (lines 350-415) duplicate runtime config values in `FileUtils::config`. For example, `outerPixelsTotal = 800` at `main.cpp:377` duplicates `config.displayLED.outerPixelsTotal = 800` at `FileUtils.cpp:31`. The compile-time constants are actually used for LED array sizing and runtime loops, while many config values go unused. This creates confusion about source of truth.

Additionally, `setConfigValuesFromFile()` (`FileUtils.cpp:287-314`) only reads 3 keys (`showSerial`, `diagMeasure`, `brightness`) from the JSON config file, even though the Config struct has dozens of fields.

#### A3. SpacecraftData Redundant Write-Then-Read Cycle — HIGH

`loadSpacecraftNamesFile()` (`SpacecraftData.cpp:155`) calls `createAndWriteNamesFile()` which populates the in-memory JSON document AND writes it to a file, then immediately reads the file back and deserializes it again. The data is already in memory after the write. Same pattern repeats for blacklist and placeholder rates. This doubles boot I/O and flash wear.

#### A4. Coupling Issues — MEDIUM

- `AnimationUtils` includes `FileUtils` for brightness access — a pure animation utility should not need filesystem access
- `DevUtils` depends on `WiFiManager` just for the `printWiFiConfigBanner()` signature
- `SpacecraftData` includes `FastLED` but never uses it directly
- Forward declaration workaround in `FileUtils.h:33` (`class DevUtils;`) suggests a past circular dependency that was worked around rather than resolved

#### A5. Commented-Out Code Debris — MEDIUM

~500+ lines of dead commented-out code across the codebase:
- `SpacecraftData.h:67-124` — entire duplicate class definition
- `SpacecraftData.cpp:734-907` — hundreds of lines of PROGMEM implementations
- `main.cpp:278-344` — WiFi parameter code

All of this is preserved in git history and should be removed.

---

## Code Quality Findings

### Critical Bugs

#### CQ1. Task Created Before Queue Exists — CRITICAL

**Location:** `src/main.cpp:3495-3504`

The `getData` task is pinned to Core 0 at line 3495 before the queue is created at line 3504. The task's `getData()` function references `queue` on line 3169 (`uxQueueSpacesAvailable(queue)`). If the task wakes up before `queue = xQueueCreate(...)` executes on Core 1, it will call FreeRTOS queue functions on a NULL handle, causing a crash or undefined behavior.

```cpp
// Line 3495 — task created BEFORE queue exists
xTaskCreatePinnedToCore(getData, "getData", 6144, NULL, 0, &xHandleData, 0);

// Line 3504 — queue created AFTER
queue = xQueueCreate(5, sizeof(CraftQueueItem));
```

**Fix:** Create the queue before creating the task.

#### CQ2. Semaphore Assignment Bug: `==` Instead of `=` — CRITICAL

**Location:** `src/main.cpp:2746, 2767`

Two code paths use the comparison operator `==` where assignment `=` is required. `semaphoreTaken` is never set to `false`, meaning on the next call to `parseData()` the function may attempt to give a mutex that is already released.

```cpp
// Line 2746 — comparison, not assignment
semaphoreTaken == false;   // BUG: should be semaphoreTaken = false;
```

**Fix:** Change `==` to `=` at both locations.

#### CQ3. Queue Sends Pointer-to-Pointer Instead of Struct — CRITICAL

**Location:** `src/main.cpp:2200, 3558`

`xQueueSend` is called with `&newCraft` where `newCraft` is a `CraftQueueItem*`. This sends the address of the pointer (4 bytes of meaningful data) into a queue slot sized for the full struct (156 bytes). The receive side has the same mismatch.

```cpp
// Line 2200 — sends CraftQueueItem** instead of the struct
if (xQueueSend(queue, &newCraft, 100) == pdPASS) {
```

**Fix:** Send the struct by value: `xQueueSend(queue, newCraft, 100)`.

#### CQ4. assignValuesToCraftSemaphore Always Reads freeList[freeListTop] — CRITICAL

**Location:** `src/main.cpp:2257-2286`

The loop iterates `i` from `MAX_ITEMS - 1` down to `1`, checks `freeList[i] != nullptr`, but then always reads from `freeList[freeListTop]` instead of `freeList[i]`. This may pick a null or wrong pool item.

```cpp
for (uint8_t i = MAX_ITEMS - 1; i > 0; i--) {
    if (freeList[i] != nullptr) {
        newCraft = freeList[freeListTop]; // BUG: should be freeList[i]
        ...
    }
}
```

### High-Severity Issues

#### CQ5. `isWiFiConnected` Missing Parentheses — Function Pointer Always Truthy

**Location:** `src/main.cpp:3120`

```cpp
if (!isWiFiConnected && wm.getWiFiIsSaved() == true) {
```

`isWiFiConnected` is a function. Without `()`, this evaluates the function's address (always non-null, always truthy), so `!isWiFiConnected` is always `false`. **The WiFi auto-reconnect logic never executes.**

#### CQ6. `randomTypeAny` Uninitialized for Rate Classes 1 and 2

**Location:** `src/main.cpp:1665-1682`

When `animateFirstCycleDown/Up` is true and `rateClass` is 1 or 2, the `if/else if` chain falls through without assigning `randomTypeAny`. The uninitialized value is used as an array index, causing out-of-bounds access.

**Fix:** Initialize `uint8_t randomTypeAny = 0;`.

#### CQ7. `freeSemaphoreItem` Nulls Local Copy, Not Caller's Pointer

**Location:** `src/main.cpp:748-783`

`infoBuffer = nullptr` at line 777 only nulls the local parameter copy. The caller retains a dangling pointer to a pool item that has been returned, enabling use-after-free if the item is immediately reused.

#### CQ8. `delay()` Inside FreeRTOS Context

**Location:** `src/main.cpp:1484, 1492`

`laserGunAnimation()` uses `delay()` which blocks Core 1 entirely without yielding to the FreeRTOS scheduler, potentially triggering the watchdog.

**Fix:** Replace with `vTaskDelay(pdMS_TO_TICKS(...))`.

#### CQ9. Global `colorTheme` Constant Shadows Parameter

**Location:** `src/main.cpp:276, 868, 3478`

A global `const uint8_t colorTheme = 0` is declared at line 276. At line 3478, `setColorTheme(colorTheme)` passes this constant value of 0, meaning the color theme can never be changed from the portal config. Should use `FileUtils::config.miscellaneous.colorTheme` instead.

#### CQ10. `firstStartupAnimation` Logic Inverted

**Location:** `src/main.cpp:3599-3603`

```cpp
if (!firstStartupAnimation) {
    firstStartupAnimation = false;  // already false — no-op
} else {
    displayDurationTimer = currentMillis;
}
```

The flag is never set to `false`, meaning `displayDurationTimer` is reset on every new craft. The branches appear swapped.

#### CQ11. `strncpy` Without Null-Termination Guarantee

**Location:** `lib/FileUtils/FileUtils.cpp:367-370`

`strncpy` does not guarantee null-termination if the source fills the buffer. The fields `apSSID[32]`, `apPass[32]`, and `serverName[128]` are used as C-strings throughout. If a value exactly fills the buffer, string operations will read past the end.

**Fix:** Add explicit null-termination after each `strncpy`, or use `strlcpy`.

### Medium-Severity Issues

#### CQ12. `LittleFS.begin()` Called Twice at Setup

**Location:** `src/main.cpp:3212-3219`

Both calls pass `true` (format-if-failed). The second call is redundant and potentially re-formats the filesystem.

#### CQ13. `printCurrentQueue` Drops Items on Failure

**Location:** `src/main.cpp:664-703`

Creates a temp queue, drains the real queue, and refills it. If `xQueueSend` to the temp queue fails mid-loop, items already dequeued are silently lost. Called on every queue receive when `showSerial` is true.

#### CQ14. `snprintf` Missing Reset Color in Output

**Location:** `src/main.cpp:1692-1703`

The format string has 4 specifiers but 5 arguments are passed. The reset color escape code is dropped, leaving the terminal stuck in cyan.

#### CQ15. `retrieveQueueBuffer` Built but Never Printed

**Location:** `src/main.cpp:3561-3570`

A formatted header string is populated via `snprintf` but `Serial.print()` is never called on it.

#### CQ16. `handleException()` Is a No-Op

**Location:** `lib/DevUtils/DevUtils.cpp:74-108`

The entire exception handling body is commented out. Every catch block that calls `dev.handleException()` effectively silences the error with a generic message.

#### CQ17. Uninitialized `characterTotalPixels` in `getCharacter()`

**Location:** `lib/TextCharacters/TextCharacters.cpp:1756-1778`

If `width` is not 3, 4, or 5, `characterTotalPixels` is uninitialized. The switch statement lacks a `default` case.

#### CQ18. Config File Handle Leak on Deserialization Failure

**Location:** `lib/FileUtils/FileUtils.cpp:247-257`

If `deserializeJson` fails in `setConfigValuesFromFile()`, the function returns without closing the file, relying on the File destructor.

---

## Security Assessment

**Overall Risk Level: HIGH**

While the device handles no sensitive user data, it has significant security weaknesses that could allow an attacker to fully compromise the device.

### Critical Security Findings

#### S1. OTA Firmware Update Over Unencrypted HTTP — CRITICAL

**Location:** `src/main.cpp:451, 503`

Both the version check and firmware binary download use plain HTTP to `develop.kellerdigital.com`. There is no TLS, no firmware signing, no hash verification, and no certificate pinning.

```cpp
httpFirmware.begin("http://develop.kellerdigital.com/minipulse/latest_version.txt");
httpUpdate.update(client, "http://develop.kellerdigital.com/minipulse/firmware.bin");
```

**Impact:** Any attacker on the same network can MITM and serve arbitrary firmware, gaining complete control of the device. This can be used to exfiltrate WiFi credentials, brick the device, or use it as a network pivot.

**Fix:** Switch to HTTPS with `WiFiClientSecure` and certificate validation. Implement firmware signing.

#### S2. Unauthenticated OTA Trigger Endpoint — CRITICAL

**Location:** `src/main.cpp:857-865`

The WiFi portal exposes `GET /trigger-firmware-update` with no authentication.

```cpp
wm.server->on("/trigger-firmware-update", HTTP_GET, []() {
    bool updateAvailable = checkFirmwareUpdateAvailable();
    updateFirmwareOta();
    ...
});
```

**Impact:** Combined with S1, an attacker can both trigger the OTA process AND serve malicious firmware. A single `curl` command is sufficient.

**Fix:** Require authentication, change to POST, add rate limiting, or require physical button press.

### High-Severity Security Findings

#### S3. Open WiFi Access Point With No Password

**Location:** `lib/FileUtils/FileUtils.cpp:14-15`

The config AP password is set to an empty string, creating an open network anyone can join.

**Fix:** Set a default WPA2 password.

#### S4. No CSRF Protection on Portal Forms

**Location:** `src/main.cpp:943-1047`

The `saveParamsCallback` processes form submissions without CSRF token validation. Any client on the network can forge configuration changes.

#### S5. Input Validation Weaknesses on Portal Parameters

**Location:** `src/main.cpp:995, 1005, 1030`

Portal values are converted via `atoi()` with no range validation before being applied to config.

**Fix:** Use `strtol()` with error checking and clamp values server-side.

#### S6. DSN Data Fetch May Lack TLS Verification

**Location:** `lib/FileUtils/FileUtils.cpp:16; src/main.cpp:2866`

The DSN URL uses HTTPS but `HTTPClient` is initialized as a plain `HTTPClient` without `WiFiClientSecure`. The TLS handshake may proceed without certificate verification or may silently fail.

**Fix:** Explicitly use `WiFiClientSecure` with root CA certificate for `eyes.nasa.gov`.

### Medium-Severity Security Findings

#### S7. Unpinned Dependency Versions (Supply Chain Risk)

**Location:** `platformio.ini:29-30`

WiFiManager and tinyxml2 are pulled from git HEAD without version pinning. A supply chain attack or breaking change would be pulled automatically.

**Fix:** Pin to specific commit hashes or tagged releases.

#### S8. Hardcoded Developer Infrastructure URL

**Location:** `src/main.cpp:451, 503`

OTA server is hardcoded to `develop.kellerdigital.com` — a development subdomain. If the domain registration lapses, an attacker could register it and serve malicious firmware to all deployed devices.

**Fix:** Use a dedicated production domain; make OTA URL configurable.

#### S9. Verbose Serial Output Exposes Operational Data

**Location:** `src/main.cpp:3208, 3238, 3458-3459`

Several lines unconditionally output firmware version, DSN server URL, WiFi credential status, and the AP password to serial, regardless of the `showSerial` flag.

**Fix:** Gate all serial output behind `showSerial`.

#### S10. XSS via innerHTML in Portal JavaScript

**Location:** `src/main.cpp:3366, 3376`

Custom portal HTML uses `innerHTML` to render server responses directly into the DOM. A MITM attacker on the HTTP firmware endpoint could inject JavaScript.

**Fix:** Use `textContent` instead of `innerHTML`.

#### S11. `fetchUrl` Buffer Fragility

**Location:** `src/main.cpp:2824-2839`

The `fetchUrl` buffer is 64 bytes but `serverName` in config allows up to 128 bytes. If the server URL is changed to something longer, the URL would be silently truncated.

**Fix:** Increase buffer to 192 bytes or validate `serverName` length on config load.

### Full Attack Scenario

1. Attacker approaches within WiFi range
2. Connects to open "JPL MiniPulse" AP (no password — S3)
3. Navigates to `http://192.168.4.1/trigger-firmware-update` (no auth — S2)
4. Simultaneously performs ARP spoofing to intercept traffic to `develop.kellerdigital.com`
5. Serves malicious firmware binary over plain HTTP (S1)
6. Device flashes attacker's firmware — **full compromise**

---

## Performance and Resource Analysis

### Memory Budget

| Component | Size | Type |
|-----------|------|------|
| LED pixel arrays (4 strips) | ~7.7 KB | Static |
| FastLED RMT double-buffer | ~7.7 KB | Static (internal) |
| `xmlDataBuffer[20480]` | 20 KB | Static local |
| SpacecraftData JSON documents | ~12.3 KB | Heap (permanent) |
| FreeRTOS queue (5 items) | ~780 B | Heap |
| CraftQueueItem pool (5 items) | ~780 B | Static |
| ActiveMeteors pointer array | 2 KB | Static |
| Peak Meteor objects (up to 500) | up to ~36 KB | Heap (dynamic) |
| WiFiManager + HTTP overhead | ~15-25 KB | Heap |
| TinyXML2 parse tree (temporary) | ~5-15 KB | Heap (during parse) |
| **Total peak estimate** | **~107-130 KB** | |

ESP32 has ~320 KB SRAM total, with IDF overhead consuming ~100-130 KB. This leaves ~190-220 KB available. At peak meteor count, the system operates with **~60-110 KB of free heap** — workable but with limited margin, especially given heap fragmentation from `new`/`delete` Meteor churn.

### LED Rendering Performance

`FastLED.show()` at `main.cpp:2025` uses the WS2812B protocol across all 4 strips. With ESP32 RMT outputting in parallel, the blocking time equals the longest strip:

```
960 LEDs * 24 bits * 1.25us/bit = ~28.8ms (inner ring)
```

This gives a hard ceiling of **~34 FPS maximum**. The target of 60 FPS (`fpsRate = 60` at line 352) is unachievable. The `fpsInMs = 1000 / fpsRate` value computed at line 531 is **never used as a frame limiter** — `FastLED.show()` is called unconditionally on every loop iteration.

### Critical Performance Issues

#### P1. Meteor Heap Fragmentation — HIGH

Each animation cycle creates multiple `Meteor` objects via `new` (`main.cpp:1241`) and deletes them via `delete` (`main.cpp:1903`). With up to 500 slots and animations repeating every ~3 seconds, this creates continuous heap fragmentation. Over hours/days of runtime, this can lead to allocation failures.

**Fix:** Implement a fixed-size Meteor pool allocator.

#### P2. LittleFS Double Write-Read Per Data Cycle — HIGH

In `fetchHTTPData()`, the HTTP response is written to `/temp.xml` on LittleFS, then immediately read back into `xmlDataBuffer` in `fetchData()`. The `String res` already holds the data in RAM. This doubles I/O per cycle and accumulates flash wear (~8,640 write/delete cycles per day).

**Fix:** Pass the HTTP response string directly to `parseData()` instead of routing through the filesystem.

#### P3. No Frame Rate Limiter — HIGH

`updateAnimation()` is called on every `loop()` iteration with no delay or frame-rate gate. `FastLED.show()` is called unconditionally, transmitting all pixel data even when nothing visually meaningful has changed since the last frame. This wastes CPU and could be capped at the achievable ~30 FPS.

**Fix:** Add `EVERY_N_MILLISECONDS(33)` around the render path, and `vTaskDelay(1)` when skipping frames.

#### P4. `DevUtils::termColor()` Uses `std::map` with Heap Allocation — HIGH

`DevUtils.cpp:4-55` uses a `static std::map<std::string, const char*>` for ANSI escape code lookup. Every call constructs a temporary `std::string` on the heap for the key lookup. This runs dozens of times per parse cycle when serial output is enabled.

**Fix:** Replace with a simple switch statement or lookup array returning `const char*`.

#### P5. Semaphore Held for ~1 Second During Parse — MEDIUM

`parseData()` holds `freeListMutex` for the entire parse duration (~900-1000ms including `vTaskDelay(100)` calls at each loop iteration). `freeSemaphoreItem()` on Core 1 waits only 500ms. If the mutex is held longer, item return fails silently.

**Fix:** Reduce critical section scope — only hold the mutex during pool allocation/deallocation, not during the full parse.

#### P6. WiFi Always-On Dual Mode — MEDIUM

WiFi runs in `WIFI_AP_STA` mode permanently, keeping both station and soft AP active. The AP consumes ~50-100mA continuously even when no clients are connected.

**Fix:** After initial configuration, switch to `WIFI_STA` mode.

#### P7. Boot Time: ~15-20 Seconds — MEDIUM

Two `delay(1000)` calls contribute 2 seconds; WiFi connection takes 3-10 seconds; SpacecraftData unconditionally writes all 3 JSON files to flash on every boot (even though the data is hardcoded and cannot change between boots). Adding a version check could skip the writes on normal boots.

#### P8. Double Pass Over 500-Slot Meteor Array Per Frame — MEDIUM

Every frame iterates the full `ActiveMeteors[500]` array twice: once in `drawMeteors()` and once in `updateMeteors()`. When only 10 meteors are active, 98% of iterations are null-pointer checks. A packed array with an active count would eliminate this waste.

#### P9. XML Parse Traversal Overhead — MEDIUM

The counter-based position tracking system (`stationCount`, `dishCount`, `targetCount`) requires iterating through the full DOM to reach the desired position on each parse cycle. The outer loop at line 2396 (`for (int i = 0; i < 2; i++)`) runs the entire parse logic twice as a retry, doubling parse time.

#### P10. `logOutput()` Uses `malloc` Per Call — LOW

The debug function allocates a dynamic buffer via `malloc` on every invocation. A fixed-size stack buffer would eliminate heap allocation in the logging path.

---

## Prioritized Remediation Roadmap

### Immediate — Fix Now (Critical Bugs + Security)

| # | Issue | Ref | Fix |
|---|-------|-----|-----|
| 1 | Queue created after task | CQ1 | Swap lines — create queue before `xTaskCreatePinnedToCore` |
| 2 | `==` instead of `=` on semaphore flag | CQ2 | Change to `semaphoreTaken = false` at lines 2746 and 2767 |
| 3 | Queue sends pointer-to-pointer | CQ3 | Change to `xQueueSend(queue, newCraft, 100)` |
| 4 | Pool allocator reads wrong index | CQ4 | Change `freeList[freeListTop]` to `freeList[i]` |
| 5 | `isWiFiConnected` missing `()` | CQ5 | Add parentheses: `!isWiFiConnected()` |
| 6 | OTA over plain HTTP | S1 | Switch to HTTPS with `WiFiClientSecure` |
| 7 | Unauthenticated OTA trigger | S2 | Add authentication or require physical button |
| 8 | Open WiFi AP | S3 | Set a default WPA2 password |

### Short-Term — Next Sprint (1-2 Weeks)

| # | Issue | Ref | Fix |
|---|-------|-----|-----|
| 9 | `randomTypeAny` uninitialized | CQ6 | Initialize to 0 |
| 10 | `freeSemaphoreItem` dangling pointer | CQ7 | Pass pointer by reference or use return value |
| 11 | `colorTheme` global shadows config | CQ9 | Use `FileUtils::config.miscellaneous.colorTheme` |
| 12 | `firstStartupAnimation` logic | CQ10 | Fix branch logic and set flag to false |
| 13 | `strncpy` null-termination | CQ11 | Switch to `strlcpy` |
| 14 | Pin dependency versions | S7 | Pin to specific commits/tags in `platformio.ini` |
| 15 | Portal input validation | S5 | Add server-side range clamping |
| 16 | `innerHTML` to `textContent` | S10 | Replace in portal JavaScript |
| 17 | Meteor pool allocator | P1 | Replace `new`/`delete` with fixed-size pool |
| 18 | Eliminate LittleFS round-trip | P2 | Pass HTTP response directly to parser |
| 19 | Add frame rate limiter | P3 | Gate `FastLED.show()` to ~30 FPS |
| 20 | Replace `std::map` in `termColor()` | P4 | Use switch statement returning `const char*` |

### Medium-Term — Next Month (2-4 Weeks)

| # | Issue | Ref | Fix |
|---|-------|-----|-----|
| 21 | Extract modules from main.cpp | A1 | Create DataFetcher, DataParser, AnimationController, WiFiController, OTAUpdater |
| 22 | Unify configuration system | A2 | Remove compile-time constant duplicates, use `FileUtils::config` exclusively |
| 23 | Remove commented-out code | A5 | Delete ~500 lines preserved in git history |
| 24 | Fix SpacecraftData write-then-read | A3 | Populate JSON in-memory without filesystem round-trip |
| 25 | Reduce semaphore hold time | P5 | Only lock during pool operations, not full parse |
| 26 | Disable AP after WiFi config | P6 | Switch to `WIFI_STA` after credentials saved |
| 27 | Optimize boot sequence | P7 | Add version check to skip redundant flash writes |
| 28 | Implement proper state machine | A1 | Replace boolean flags with enum-based states |
| 29 | Serial output gating | S9 | Gate all output behind `showSerial` |
| 30 | Firmware signing | S1 | Implement ED25519 or RSA signature verification for OTA |

### Long-Term — Future Releases

| # | Issue | Ref | Fix |
|---|-------|-----|-----|
| 31 | ESP32 Secure Boot V2 | S1 | Enable for production deployments |
| 32 | Flash encryption | S13 | Protect stored WiFi credentials |
| 33 | Production OTA domain | S8 | Dedicated domain with long-term registration |
| 34 | Compact meteor data structure | P8 | Packed array with active count |
| 35 | Move font data to PROGMEM | A3 | Free ~7 KB of RAM |
| 36 | Batch config file writes | A2 | Single write per portal submission instead of 3 |
