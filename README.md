# JPL-MiniPulse

A real-time NASA Deep Space Network monitor for ESP32, displaying spacecraft communications on an LED matrix.

## Quick Reference: Adding Spacecraft Data

All spacecraft data is managed in a single file:

```
lib/SpacecraftData/SpacecraftData.cpp
```

| Task | Function | Lines |
|------|----------|-------|
| Add spacecraft name | `createAndWriteNamesFile()` | 44-124 |
| Add to blacklist | `createAndWriteBlacklistFile()` | 215-218 |
| Add placeholder rates | `createAndWritePlaceholderRatesFile()` | 316-486 |

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

## Reference Files

The `data_reference/spacecraft_data/` directory contains JSON files for **documentation purposes only**:

```
data_reference/spacecraft_data/
├── names.json            # Callsign → name mappings
├── blacklist.json        # Ignored callsigns
└── placeholder_rates.json # Fallback data rates
```

**These files are NOT read at runtime.** They exist purely for human reference in source control. The actual data comes from hardcoded values in `SpacecraftData.cpp`, which are written to the ESP32's LittleFS filesystem on each boot.

If you update `SpacecraftData.cpp`, consider updating the reference files manually to keep documentation in sync.

---

## Spacecraft Data Architecture

The system uses a **hardcoded source of truth** in C++ that gets written to the ESP32's filesystem on boot.

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

## Data Files Explained

### names.json
Maps DSN callsigns to human-readable spacecraft names.

| Callsign | Full Name |
|----------|-----------|
| `JWST` | James Webb Space Telescope |
| `MRO` | Mars Reconnaissance Orbiter |
| `VGR1` | Voyager 1 |

### blacklist.json
Callsigns to exclude from display (test signals, ground stations, etc.)

| Callsign | Reason |
|----------|--------|
| `TEST` | Test signal |
| `DSN` | Network identifier |
| `RFC(VLBI)` | Radio astronomy |

### placeholder_rates.json
Fallback data rates (in scientific notation) used when the actual signal rate is 0. A random rate from the array is selected for visual variety.

```json
{
  "JWST": ["1.250e+02", "2.500e+02", "4.000e+03", "2.500e+05"],
  "MRO": ["7.812e+00", "6.250e+01", "1.250e+02", "4.000e+03"]
}
```

If no placeholder rates are defined for a spacecraft, the default fallback is `1.000e+03` (1000 bps).

## DSN XML Data Flow

The NASA Deep Space Network provides XML data in this format:

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

Processing flow in `src/main.cpp`:
1. `parseData()` extracts the `target name` attribute (callsign)
2. `checkBlacklist()` filters out unwanted callsigns
3. `callsignToName()` converts callsign to display name
4. `findSignals()` extracts data rates, using `getPlaceholderRate()` if rate is 0

## Memory Constraints

This runs on an ESP32 with limited RAM. The JSON document sizes are capped:

| Document | Max Size |
|----------|----------|
| `spacecraftNamesJson` | 5120 bytes |
| `spacecraftBlacklistJson` | 1024 bytes |
| `spacecraftPlaceholderRatesJson` | 6144 bytes |

Keep spacecraft names under 100 characters (limited by `CraftQueueItem.nameArray`).
