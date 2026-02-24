# Planning Document: GitHub-Based OTA Firmware Updates

## Overview

Replace the current `develop.kellerdigital.com` firmware hosting with GitHub Releases for version checking and firmware binary downloads.

## Background

### Current Behavior

OTA firmware updates use two hardcoded URLs in `src/main.cpp`:

1. **Version check** (line 456): `http://develop.kellerdigital.com/minipulse/latest_version.txt`
2. **Firmware binary** (line 509): `http://develop.kellerdigital.com/minipulse/firmware.bin`

Both use plain HTTP via `HTTPClient` / `httpUpdate`. The version check fetches a text file, compares it with `strcmp()` against the hardcoded `currentFirmwareVersion`, and the binary download uses ESP32's built-in `httpUpdate.update()`.

### Problems

- Relies on a personal server that may go offline
- Plain HTTP is vulnerable to MITM attacks (firmware injection)
- No versioned history of releases
- Manual file management on the server

### Proposed Behavior

Use GitHub Releases from `GDKeller/JPL-MiniPulse` for both version checking and firmware downloads:
- **Version check**: Query the GitHub Releases API (`/releases/latest`), parse `tag_name` from the JSON response
- **Firmware binary**: Download `firmware.bin` from the latest release assets
- No `latest_version.txt` file needed — the release tag IS the version

## Design Decisions

### HTTPS Requirement

GitHub requires HTTPS. Two options:

| Option | Pros | Cons |
|--------|------|------|
| **A: WiFiClientSecure + setInsecure()** | Simple, no cert management | No server verification (still encrypted in transit) |
| **B: WiFiClientSecure + root CA cert** | Full TLS verification | Must update cert when it expires; uses ~1-2KB more flash |

**Recommendation**: Start with **Option A** (`setInsecure()`). This provides encrypted transport (preventing passive eavesdropping) without the cert maintenance burden. The device is on a local network and firmware signing (Roadmap #30) is the proper long-term solution for binary integrity.

### GitHub URL Pattern

Two GitHub endpoints:

1. **Version check** (API — returns JSON):
   ```
   https://api.github.com/repos/GDKeller/JPL-MiniPulse/releases/latest
   ```
   Response includes `"tag_name": "1.1.0"` — this is the version string.

2. **Firmware download** (Release asset — returns 302 redirect to CDN):
   ```
   https://github.com/GDKeller/JPL-MiniPulse/releases/latest/download/firmware.bin
   ```

The API endpoint returns JSON directly (no redirect). The asset download URL returns a **302 redirect** to the actual CDN URL. The ESP32 HTTP client must follow redirects for the binary download.

### Redirect Handling

- `HTTPClient` supports `setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS)` or `HTTPC_FORCE_FOLLOW_REDIRECTS`
- `httpUpdate` does NOT natively follow redirects — we need to resolve the final URL first, then pass it to `httpUpdate.update()`

### Memory Impact

- `WiFiClientSecure` uses ~20-40KB more heap than `WiFiClient`
- Current free heap at runtime is ~100-150KB (from diagnostics), so this is feasible
- The secure client should be created on the stack or as a local variable, not a persistent global, to free memory after update

## Files to Modify

| File | Change |
|------|--------|
| `src/main.cpp` | Update URLs, switch to WiFiClientSecure, add redirect following |
| `platformio.ini` | No changes needed (WiFiClientSecure is part of ESP32 Arduino core) |

## Implementation Details

### Step 1: Define GitHub URLs

**File:** `src/main.cpp` (top of file, near version string)

```cpp
const char* currentFirmwareVersion = "1.1.0-9";
const char* githubApiUrl = "https://api.github.com/repos/GDKeller/JPL-MiniPulse/releases/latest";
const char* firmwareBinaryUrl = "https://github.com/GDKeller/JPL-MiniPulse/releases/latest/download/firmware.bin";
```

### Step 2: Update getRemoteFirmwareVersion()

**Current** (line 451): Uses `HTTPClient` with plain HTTP to fetch a text file.

**Proposed:** Query the GitHub Releases API and parse `tag_name` from the JSON response.

```cpp
const char* getRemoteFirmwareVersion() {
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    static char buffer[16];
    buffer[0] = '\0';

    WiFiClientSecure secureClient;
    secureClient.setInsecure(); // Encrypted transport, no cert pinning

    HTTPClient httpFirmware;
    httpFirmware.setTimeout(10000); // 10s timeout (GitHub API can be slow)
    httpFirmware.addHeader("Accept", "application/vnd.github+json");
    httpFirmware.addHeader("User-Agent", "JPL-MiniPulse-ESP32"); // GitHub API requires User-Agent

    if (!httpFirmware.begin(secureClient, githubApiUrl)) {
        Serial.println("Failed to begin HTTP connection for version check");
        return buffer;
    }

    int httpCode = httpFirmware.GET();
    if (showSerial) Serial.printf("Version check HTTP code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
        String payload = httpFirmware.getString();

        // Parse just the tag_name from the JSON response
        // Using a filter to minimize memory usage — only extract "tag_name"
        StaticJsonDocument<64> filter;
        filter["tag_name"] = true;

        DynamicJsonDocument doc(512);
        DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

        if (err == DeserializationError::Ok && doc.containsKey("tag_name")) {
            const char* tagName = doc["tag_name"];
            if (showSerial) Serial.printf("Remote version (tag): %s\n", tagName);
            strncpy(buffer, tagName, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        } else {
            Serial.println("Failed to parse tag_name from GitHub API response");
        }
    } else {
        Serial.printf("Failed to fetch remote version (HTTP %d)\n", httpCode);
    }

    httpFirmware.end();
    return buffer;
}
```

**Note on memory:** The GitHub API response for `/releases/latest` can be large (~5-20KB depending on release notes and assets). Using ArduinoJson's filter feature, we only parse the `tag_name` field, keeping memory usage minimal. An alternative lighter approach is to use `payload.indexOf("\"tag_name\"")` for string-based extraction if memory is tight.

### Step 3: Update updateFirmwareOta()

**Challenge:** `httpUpdate.update()` doesn't follow redirects well with GitHub's CDN. We need to resolve the redirect first.

**Proposed approach:** Resolve the final binary URL via a HEAD request, then pass it to httpUpdate.

```cpp
void updateFirmwareOta() {
    if (!checkFirmwareUpdateAvailable()) return;

    Serial.println("\n--------\nAttempting firmware update from GitHub...");

    WiFiClientSecure secureClient;
    secureClient.setInsecure();

    // Resolve the GitHub redirect to get the actual CDN URL
    HTTPClient httpResolve;
    httpResolve.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    httpResolve.setTimeout(10000);

    if (!httpResolve.begin(secureClient, firmwareBinaryUrl)) {
        Serial.println("Failed to begin HTTP connection for firmware download");
        return;
    }

    // Use httpUpdate with the secure client and resolved URL
    t_httpUpdate_return ret = httpUpdate.update(secureClient, firmwareBinaryUrl);

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf(">> HTTP_UPDATE_FAILED Error (%d): %s\n",
                httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println(">> HTTP_UPDATE_NO_UPDATES");
            break;
        case HTTP_UPDATE_OK:
            Serial.println(">> HTTP_UPDATE_OK");
            break;
        default:
            Serial.println(">> HTTP_UPDATE Unknown result");
            break;
    }

    Serial.println("OTA update complete\n--------\n");
}
```

**Note:** If `httpUpdate.update()` doesn't follow GitHub redirects properly with WiFiClientSecure, the fallback approach is:
1. HEAD request to get the `Location` header
2. Follow the redirect chain manually
3. Pass the final CDN URL to `httpUpdate.update()`

This needs to be tested on hardware.

### Step 4: Create Release Workflow

When publishing a new release:

1. Update `currentFirmwareVersion` in `src/main.cpp`
2. Build firmware: `~/.platformio/penv/bin/pio run -e release`
3. Create a GitHub Release with tag matching the version (e.g., tag `1.2.0`)
4. Upload `firmware.bin` (from `.pio/build/release/firmware.bin`) as a release asset

That's it — the device reads the version from the release tag, so no extra files needed.

This could be automated with GitHub Actions in the future.

### Note on #39 (`.catch()` on firmware version fetch)

The current portal JavaScript fetches the version without error handling. This is addressed by this plan — the rewritten `getRemoteFirmwareVersion()` handles HTTP failures gracefully and the portal JS should be updated to handle fetch errors at the same time. No need for a separate fix.

## Testing

1. Create a test GitHub Release with a dummy version and firmware binary
2. Verify version check returns correct string over HTTPS
3. Verify redirect following works (GitHub → CDN)
4. Verify firmware download and flash succeeds
5. Test failure cases: no WiFi, GitHub down, invalid binary
6. Monitor heap usage during HTTPS operations

## Risks

| Risk | Severity | Mitigation |
|------|----------|------------|
| GitHub redirect chain breaks httpUpdate | HIGH | Test on hardware; implement manual redirect following as fallback |
| WiFiClientSecure heap usage too high | MED | Create client as local variable; monitor free heap |
| GitHub rate limiting (60 req/hr unauthenticated) | LOW | Device only checks on portal interaction, not polling |
| HTTPS handshake timeout on slow WiFi | MED | Set 10s timeout; retry logic already exists |

## Estimated Scope

- 1 file modified (`src/main.cpp`)
- ~40 lines changed
- No new dependencies (WiFiClientSecure is part of ESP32 core)
- Requires hardware testing for redirect behavior
