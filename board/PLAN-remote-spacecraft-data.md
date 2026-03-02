# Planning Document: Remote Spacecraft Data + Full/Restricted List

## Overview

Replace hardcoded spacecraft data with remotely-hosted JSON files fetched from GitHub and cached on LittleFS. Add a portal setting to toggle between a "full" callsign list and a NASA-approved "restricted" list.

## Background

### Current Behavior

`SpacecraftData.cpp` hardcodes all spacecraft data in three `createAndWrite*()` functions:

1. **`createAndWriteNamesFile()`** — 81 callsign→name mappings, written to `/spacecraft_data/names.json`
2. **`createAndWriteBlacklistFile()`** — 3-4 always-filtered callsigns, written to `/spacecraft_data/blacklist.json`
3. **`createAndWritePlaceholderRatesFile()`** — 26 spacecraft with fallback rate arrays, written to `/spacecraft_data/placeholder_rates.json`

On every boot, these functions **overwrite** the LittleFS files with hardcoded data, then the `load*()` functions read them back into `DynamicJsonDocument` objects. This means updating any spacecraft requires a firmware update.

### Problems

- Adding/removing spacecraft requires a new firmware build and OTA
- The NASA client approved a limited subset of craft, but the full list is hardcoded
- No way to toggle between full and restricted lists without code changes
- The `createAndWrite*()` → `load*()` round-trip is redundant (hardcode → write → read)

### Proposed Behavior

1. Host spacecraft JSON files in the GitHub repo under `remote/spacecraft/`
2. On boot, if WiFi is connected, fetch the latest JSON files from GitHub
3. Cache fetched files on LittleFS; use cache if fetch fails
4. If no cache and no network, fall back to a minimal hardcoded set
5. A portal setting toggles between "Full" and "Restricted" spacecraft lists
6. In restricted mode, only NASA-approved callsigns are displayed

## Architecture

### Remote File Hosting

Host JSON files in the repository at `remote/spacecraft/`:

```
remote/
└── spacecraft/
    ├── names.json              # All callsign→name mappings
    ├── blacklist.json          # Always-filtered callsigns
    ├── restricted.json         # Array of NASA-approved callsigns (allowlist)
    └── placeholder_rates.json  # Fallback uplink rates (until #37 removes need)
```

**Fetch URL pattern** (GitHub raw content):
```
https://raw.githubusercontent.com/GDKeller/JPL-MiniPulse/main/remote/spacecraft/{file}.json
```

Using `main` branch ensures the device always gets the latest published data. Could also use a tag or dedicated `data` branch for more control.

### Data Flow

```
Boot
 │
 ├─ WiFi connected?
 │   ├─ YES → Fetch remote JSONs from GitHub
 │   │         ├─ Success → Write to LittleFS cache, load into memory
 │   │         └─ Failure → Load from LittleFS cache (if exists)
 │   │                       └─ No cache → Load hardcoded minimal fallback
 │   └─ NO  → Load from LittleFS cache (if exists)
 │             └─ No cache → Load hardcoded minimal fallback
 │
 └─ Apply full/restricted filter based on config setting
```

### Full vs Restricted Mode

**Full mode** (default for development/personal use):
- Display all spacecraft from `names.json`
- Still filter out `blacklist.json` entries (TEST, DSN, etc.)

**Restricted mode** (for NASA client units):
- Display ONLY spacecraft whose callsigns appear in `restricted.json`
- Also filter out `blacklist.json` entries

**`restricted.json` format:**
```json
["JWST", "MSL", "M20", "VGR1", "VGR2", "JNO", "PSYC", "EURC", "LUCY", "NHPC", "SPP", "LRO"]
```

This is an **allowlist** — simple array of approved callsigns. The filtering logic becomes:

```cpp
bool shouldDisplay(const char* callsign) {
    // Always filter blacklisted
    if (checkBlacklist(callsign)) return false;

    // In restricted mode, must be in allowlist
    if (restrictedMode && !isInRestrictedList(callsign)) return false;

    return true;
}
```

### LittleFS Cache Structure

```
/spacecraft_data/
├── names.json              # Cached from remote (or hardcoded fallback)
├── blacklist.json          # Cached from remote
├── restricted.json         # Cached from remote (NEW)
├── placeholder_rates.json  # Cached from remote
└── last_fetch.txt          # Timestamp of last successful fetch (NEW)
```

### Freshness Strategy

- **Fetch on every boot** if WiFi is connected (spacecraft data is small, ~5-8KB total)
- Store timestamp in `last_fetch.txt` for diagnostics
- No periodic re-fetch during runtime (device usually runs unattended)
- Portal could add a "Refresh Spacecraft Data" button for manual trigger

## Files to Modify

| File | Change |
|------|--------|
| `lib/SpacecraftData/SpacecraftData.h` | Add fetch methods, restricted list support, new JSON doc |
| `lib/SpacecraftData/SpacecraftData.cpp` | Replace `createAndWrite*()` with fetch+cache logic, add filtering |
| `lib/FileUtils/FileUtils.h` | Add `spacecraftListMode` to config struct |
| `lib/FileUtils/FileUtils.cpp` | Add default value, read/write support for new setting |
| `src/main.cpp` | Add portal parameter for list mode, update `saveParamsCallback()`, add fetch call after WiFi connect |
| `remote/spacecraft/names.json` | NEW — hosted spacecraft names (move from `data_reference/`) |
| `remote/spacecraft/blacklist.json` | NEW — hosted blacklist |
| `remote/spacecraft/restricted.json` | NEW — NASA-approved callsign allowlist |
| `remote/spacecraft/placeholder_rates.json` | NEW — hosted placeholder rates |

## Implementation Details

### Phase 1: Remote Data Hosting

Create `remote/spacecraft/` in the repo with JSON files derived from the master CSV at `data_reference/MiniPulse Craft List Jan 2026 - spacecraft_list.csv.csv`.

**Source CSV fields**: Callsign, Approved, MiniPulse Display Name, Full Name, Alt Name, Agency, New, Status, Type, Mission URL

**`names.json`** — Generated from CSV. Uses "MiniPulse Display Name" as the display value. Includes all rows where Type = "spacecraft" (excludes blacklist and removed). Also include rows with Status = "" or Status != "removed"/"Ended" unless they are new/upcoming craft.

```json
{
    "ACE": "ACE",
    "AGM1": "Griffin Lander",
    "BEPI": "BepiColombo",
    "BIOS": "BioSentinel",
    "CAPS": "CAPSTONE",
    "CGO": "Carruthers Observatory",
    "CHDR": "Chandra",
    "DSCO": "DSCOVR",
    "EMM": "Emirates Mars",
    "ESCB": "EscaPADE Blue",
    "ESCG": "EscaPADE Gold",
    "EURC": "Europa Clipper",
    "HYB2": "Hayabusa 2",
    "IMAP": "IMAP",
    "JNO": "Juno",
    "JWST": "JWST",
    "KPLO": "Danuri",
    "LRO": "Lunar Recon Orbiter",
    "LUCY": "Lucy",
    "M01O": "Mars Odyssey",
    "M20": "Perseverance",
    "MEX": "Mars Express",
    "MMS1": "Magneto MultiScale 1",
    "MMS2": "Magneto MultiScale 2",
    "MMS3": "Magneto MultiScale 3",
    "MMS4": "Magneto MultiScale 4",
    "MRO": "Mars Recon Orbiter",
    "MSL": "Curiosity",
    "MVN": "MAVEN",
    "NHPC": "New Horizons",
    "ORX": "OSIRIS APEX",
    "PSYC": "Psyche",
    "SOHO": "Solar Heliospheric Observatory",
    "SOLAR": "SOLAR",
    "SPP": "Parker Solar Probe",
    "STA": "STEREO A",
    "TESS": "TESS",
    "TGO": "ExoMars TGO",
    "THB": "Themis B",
    "THC": "Themis C",
    "VGR1": "Voyager 1",
    "VGR2": "Voyager 2",
    "WIND": "Wind",
    "XMM": "XMM Newton"
}
```

(Plus all non-blacklisted, non-removed craft from CSV — full list ~70+ entries including upcoming missions)

**`blacklist.json`** — Derived from CSV rows where Type = "blacklist":
```json
{
    "ATOT": true,
    "DSSR": true,
    "EGS": true,
    "GBRA": true,
    "GSSR": true,
    "GVRT": true,
    "HCRA": true,
    "RFC": true,
    "SGP": true,
    "TEST": true,
    "DSN": true
}
```

**`restricted.json`** — Derived from CSV rows where Approved = "TRUE" (43 callsigns):
```json
[
    "ACE", "BEPI", "BIOS", "CAPS", "CGO", "CHDR", "DSCO", "EMM",
    "ESCB", "ESCG", "EURC", "HYB2", "IMAP", "JNO", "JWST", "KPLO",
    "LRO", "LUCY", "M01O", "M20", "MEX", "MMS1", "MMS2", "MMS3",
    "MMS4", "MRO", "MSL", "MVN", "NHPC", "ORX", "PSYC", "SOHO",
    "SOLAR", "SPP", "STA", "TESS", "TGO", "THB", "THC", "VGR1",
    "VGR2", "WIND", "XMM"
]
```

**`placeholder_rates.json`** — Copy from current. Note: after #37 (binary up signal), this file may become unnecessary since up signals won't use rates. Down signals always have real rates from the XML feed.

### Phase 2: Fetch + Cache System

Add to `SpacecraftData`:

```cpp
// New public methods
static bool fetchRemoteData();           // Fetch all files from GitHub
static bool fetchAndCacheFile(           // Fetch single file
    const char* filename,
    const char* url
);
static bool hasCachedData();             // Check if LittleFS cache exists
static void loadHardcodedFallback();     // Minimal hardcoded data

// New private members
static const char* baseUrl;  // "https://raw.githubusercontent.com/GDKeller/JPL-MiniPulse/main/remote/spacecraft/"
```

**`fetchAndCacheFile()` implementation outline:**

```cpp
bool SpacecraftData::fetchAndCacheFile(const char* filename, const char* url) {
    WiFiClientSecure secureClient;
    secureClient.setInsecure();

    HTTPClient http;
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setTimeout(10000);

    if (!http.begin(secureClient, url)) return false;

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    // Validate it's parseable JSON before caching
    DynamicJsonDocument testDoc(payload.length() * 1.5);
    if (deserializeJson(testDoc, payload) != DeserializationError::Ok) {
        Serial.printf("Invalid JSON from remote: %s\n", filename);
        return false;
    }

    // Write to LittleFS cache
    char filepath[64];
    snprintf(filepath, sizeof(filepath), "/spacecraft_data/%s", filename);
    File f = LittleFS.open(filepath, "w");
    if (!f) return false;
    f.print(payload);
    f.close();

    return true;
}
```

**`fetchRemoteData()` implementation outline:**

```cpp
bool SpacecraftData::fetchRemoteData() {
    const char* files[] = {"names.json", "blacklist.json", "restricted.json", "placeholder_rates.json"};
    bool allSuccess = true;

    for (const char* file : files) {
        char url[256];
        snprintf(url, sizeof(url), "%s%s", baseUrl, file);

        Serial.printf("Fetching %s...\n", file);
        if (!fetchAndCacheFile(file, url)) {
            Serial.printf("Failed to fetch %s, will use cache\n", file);
            allSuccess = false;
        }
    }

    // Write fetch timestamp
    File ts = LittleFS.open("/spacecraft_data/last_fetch.txt", "w");
    if (ts) {
        ts.print(millis());  // Uptime-based (no RTC)
        ts.close();
    }

    return allSuccess;
}
```

### Phase 3: Hardcoded Fallback

Replace the massive `createAndWrite*()` functions with a minimal fallback for when there's no network and no cache. This should cover the most common/important spacecraft:

```cpp
void SpacecraftData::loadHardcodedFallback() {
    spacecraftNamesJson.clear();
    // Minimal set — just the most iconic craft
    spacecraftNamesJson["JWST"] = "James Webb Space Telescope";
    spacecraftNamesJson["VGR1"] = "Voyager 1";
    spacecraftNamesJson["VGR2"] = "Voyager 2";
    spacecraftNamesJson["MSL"] = "Curiosity";
    spacecraftNamesJson["M20"] = "Mars 2020";
    spacecraftNamesJson["JNO"] = "Juno";
    spacecraftNamesJson["PSYC"] = "Psyche";
    spacecraftNamesJson["EURC"] = "Europa Clipper";
    // ... ~15-20 entries max

    spacecraftBlacklistJson.clear();
    spacecraftBlacklistJson["TEST"] = true;
    spacecraftBlacklistJson["DSN"] = true;

    spacecraftPlaceholderRatesJson.clear();
    // Empty — fallback to default rate in getPlaceholderRate()
}
```

### Phase 4: Restricted List Support

**New JSON document and load function:**

```cpp
// SpacecraftData.h
static DynamicJsonDocument spacecraftRestrictedListJson; // 1024 bytes

// SpacecraftData.cpp
static bool restrictedMode = false;

bool SpacecraftData::isRestricted(const char* callsign) {
    if (!restrictedMode) return false; // Full mode — allow everything

    JsonArray arr = spacecraftRestrictedListJson.as<JsonArray>();
    for (JsonVariant v : arr) {
        if (strcmp(v.as<const char*>(), callsign) == 0) {
            return false; // Found in allowlist — not restricted
        }
    }
    return true; // Not in allowlist — restricted (hide it)
}
```

**Integration point** — in `src/main.cpp` where spacecraft are processed (the XML parsing loop), after the blacklist check:

```cpp
// Existing check
if (SpacecraftData::checkBlacklist(callsign)) continue;

// New check
if (SpacecraftData::isRestricted(callsign)) continue;
```

### Phase 5: Portal Setting

**New WiFiManager parameter:**

```cpp
// In parameter declarations (main.cpp ~line 290)
WiFiManagerParameter param_spacecraft_list_mode(
    "spacecraft_list_mode",
    "Use full spacecraft list",
    "1",
    1,
    "type='checkbox' checked style='...'"
);

// Add to WiFiManager
wm.addParameter(&param_spacecraft_list_mode);
```

**In `saveParamsCallback()`:**

```cpp
String listModeValue = getParam("spacecraft_list_mode");
bool fullMode = (listModeValue.length() > 0 && listModeValue != "0");
SpacecraftData::setRestrictedMode(!fullMode);
FileUtils::writeConfigFileBool("spacecraftFullList", fullMode);
```

**In config struct (`FileUtils.h`):**

```cpp
struct MiscellaneousConfig {
    // ... existing fields
    bool spacecraftFullList = true; // Default: full list
};
```

### Phase 6: Updated Load Sequence

Replace current `SpacecraftData::loadJson()`:

```cpp
void SpacecraftData::loadJson() {
    // Set restricted mode from config
    restrictedMode = !FileUtils::config.miscellaneous.spacecraftFullList;

    Serial.println("Loading spacecraft data...");

    // Try to load from LittleFS cache first (fast)
    bool cacheExists = hasCachedData();

    if (cacheExists) {
        // Load from cache
        loadSpacecraftNamesFile();
        loadSpacecraftBlacklistFile();
        loadSpacecraftRestrictedListFile();
        loadSpacecraftPlaceholderRatesFile();
        Serial.println("Spacecraft data loaded from cache");
    } else {
        // No cache — use minimal hardcoded fallback
        loadHardcodedFallback();
        Serial.println("Spacecraft data loaded from hardcoded fallback");
    }
}
```

**Remote fetch is triggered separately** after WiFi connects in `setup()`:

```cpp
// After WiFi connection confirmed (main.cpp setup, ~line 3510)
if (isWiFiConnected()) {
    Serial.println("Fetching spacecraft data from GitHub...");
    if (SpacecraftData::fetchRemoteData()) {
        // Reload from fresh cache
        SpacecraftData::loadJson();
    }
}
```

## Spacecraft Data Source

All JSON files are derived from the master CSV:
`data_reference/MiniPulse Craft List Jan 2026 - spacecraft_list.csv.csv`

CSV fields: Callsign, Approved, MiniPulse Display Name, Full Name, Alt Name, Agency, New, Status, Type, Mission URL

**Derivation rules:**
- `names.json`: All rows where Type = "spacecraft" → `{Callsign: "MiniPulse Display Name"}`
- `blacklist.json`: All rows where Type = "blacklist" → `{Callsign: true}` (plus TEST, DSN)
- `restricted.json`: All rows where Approved = "TRUE" → `[Callsign, ...]`
- Rows where Status = "removed" or "Ended" are excluded from names.json

**Key changes from current firmware:**
- 30+ new craft added (BEPI, CGO, ESCB, ESCG, IMAP, SOLAR, NEOS, HST, JUICE, HERA, etc.)
- Many removed/ended missions dropped (ARGO, CH2, CUE3, DART, EQUL, GAIA, etc.)
- Display names updated (e.g., "Mars 2020" → "Perseverance", "Korea Pathfinder Lunar Orbiter" → "Danuri")
- 5 new blacklist entries added (DSSR, HCRA, RFC, etc.)
- Test entries (Rate1-Rate6) excluded from remote files
- 43 NASA-approved callsigns form the restricted list

## Dependency on Other Plans

| Dependency | Impact |
|------------|--------|
| **#37 Binary up signal** | After implementing, `placeholder_rates.json` is only needed for down signals (or can be removed entirely if down signals always have real rates) |
| **GitHub OTA** | Shares the HTTPS + WiFiClientSecure pattern; implement together to avoid duplicating the secure client setup |

## Testing

1. **Remote fetch**: Verify all 4 files download correctly from GitHub raw URLs
2. **Cache fallback**: Disconnect WiFi, verify device uses cached LittleFS data
3. **No cache + no WiFi**: Delete LittleFS files, disconnect WiFi, verify hardcoded fallback works
4. **JSON validation**: Push invalid JSON to test branch, verify device rejects it and keeps cache
5. **Restricted mode**: Toggle setting in portal, verify only approved craft are displayed
6. **Full mode**: Toggle back, verify all craft appear
7. **Blacklist + restricted**: Verify blacklisted items are filtered even if they appear in restricted list
8. **Memory**: Monitor heap usage during HTTPS fetch (WiFiClientSecure overhead)
9. **Portal**: Verify checkbox persists state correctly across saves

## Risks

| Risk | Severity | Mitigation |
|------|----------|------------|
| WiFiClientSecure heap usage during fetch | MED | Fetch sequentially, destroy client after each file |
| GitHub raw content caching/CDN delay after push | LOW | Content usually available within minutes |
| LittleFS space exhaustion | LOW | Total spacecraft data is ~8KB; LittleFS partition has room |
| Corrupted cache from interrupted write | MED | Validate JSON before writing; keep hardcoded fallback |
| ESP32 HTTPS handshake slow on first connect | LOW | Expected; add appropriate timeouts |

## Estimated Scope

- 3 files modified (`SpacecraftData.h`, `SpacecraftData.cpp`, `main.cpp`, `FileUtils.h`, `FileUtils.cpp`)
- 4 new remote files created (`remote/spacecraft/*.json`)
- ~150-200 lines changed/added
- `createAndWrite*()` functions can be largely removed (~400 lines deleted)
- Net reduction in firmware binary size (less hardcoded data)

## Implementation Order

1. Create `remote/spacecraft/` files in repo and push to `main`
2. Add restricted list JSON document and `isRestricted()` to SpacecraftData
3. Add `fetchAndCacheFile()` and `fetchRemoteData()` to SpacecraftData
4. Replace `createAndWrite*()`/`load*()` flow with fetch+cache+fallback
5. Add portal setting for full/restricted mode
6. Update config struct and save/load logic
7. Wire up the fetch call after WiFi connects in `setup()`
8. Test on hardware
