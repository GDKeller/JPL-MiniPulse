# Codebase Review: JPL-MiniPulse
**Date:** 2026-02-19
**Reviewer:** Gemini CLI

## 1. Executive Summary
**JPL-MiniPulse** is a sophisticated ESP32-based real-time monitor for NASA's Deep Space Network (DSN). It visualizes spacecraft communication data through LED rings using a multi-threaded architecture.

## 2. Architecture & Design
The project demonstrates excellent use of the ESP32's dual-core capabilities:
- **Core 0 (Networking/Parsing):** Handles the heavy lifting of WiFi management, HTTP requests to NASA DSN, and XML parsing using `tinyxml2`.
- **Core 1 (Animation/UI):** Dedicated to the `FastLED` animation loop and user input, ensuring high frame rates (60fps+) without network-induced stutter.
- **Inter-Core Communication:** Implemented via a FreeRTOS queue (`CraftQueueItem*`), which is a robust pattern for embedded systems.

## 3. Key Components
- **SpacecraftData (lib/):** Manages mapping NASA callsigns to human-readable names and handles data rate scaling.
- **Animate (lib/):** A modular animation engine that scales visual complexity based on the signal's bits-per-second (BPS).
- **FileUtils (lib/):** Provides a clean abstraction over LittleFS for configuration persistence (`config.json`).

## 4. Strengths
- **Memory Management:** Uses a static item pool and semaphore-guarded free lists for queue objects, which prevents heap fragmentation in long-running deployments.
- **Resilience:** Includes a watchdog timer (`esp_task_wdt`) and automatic failover to dummy data if the network or XML feed is unavailable.
- **Modularity:** High degree of decoupling between hardware drivers (LEDs) and data logic.

## 5. Areas for Improvement
- **Data Source:** The reliance on raw XML parsing on the ESP32 is resource-intensive. A middleware/proxy to JSON would be more efficient, though the current Core 0 offloading handles it well.
- **Configuration:** Spacecraft names are currently hardcoded in `SpacecraftData.cpp` and written to LittleFS at boot if missing. This makes user-driven updates difficult without a re-flash.
- **Testing:** The `test/` directory contains only a README. Implementing unit tests for the parsing and mapping logic would improve long-term stability.

## 6. Technical Stack
- **Framework:** Arduino / ESP-IDF (via PlatformIO)
- **Primary Libraries:** FastLED, ArduinoJson, tinyxml2, WiFiManager
- **Storage:** LittleFS
