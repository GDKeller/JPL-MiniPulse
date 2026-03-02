# Planning Document: GitHub-Based OTA Firmware Updates

## Overview

Replace the current `develop.kellerdigital.com` firmware hosting with GitHub Releases for version checking and firmware binary downloads.

## Background

### Current Behavior

OTA firmware updates use two hardcoded URLs in `src/main.cpp`:

1. **Version check** (~line 391): `http://develop.kellerdigital.com/minipulse/latest_version.txt`
2. **Firmware binary** (~line 448): `http://develop.kellerdigital.com/minipulse/firmware.bin`

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

## Rollout Strategy

### The Bootstrapping Problem

v1.1.0 is the release that *contains* the new GitHub OTA code, but it must itself be distributed via the **old** kellerdigital.com OTA system (since devices in the field are still pointing there). This creates a two-phase rollout:

| Phase | Version | Distributed Via | Contains |
|-------|---------|-----------------|----------|
| **1 — Bridge release** | `1.1.0` | kellerdigital.com (old OTA) | New GitHub OTA code + all other 1.1.0 features |
| **2 — First GitHub release** | `1.1.1` | GitHub Releases (new OTA) | Minimal changes — exists primarily to validate the new OTA path |

### Version Comparison

The current code uses `strcmp(latestVersion, currentFirmwareVersion) != 0` — any difference triggers an update, which means a stale or rolled-back release tag could cause an accidental downgrade. We'll replace this with a proper semver comparison that only triggers when the remote version is strictly newer (higher major, minor, or patch).

### Phase 1: Ship v1.1.0 via kellerdigital.com

1. Finalize all v1.1.0 code (including the new GitHub OTA functions)
2. Set `currentFirmwareVersion = "1.1.0"`
3. Build and upload `firmware.bin` to `develop.kellerdigital.com/minipulse/`
4. Update `latest_version.txt` on the server to `1.1.0`
5. Devices in the field update via the old path — they now have GitHub OTA code

### Phase 2: Test with v1.1.1 on GitHub Releases

1. Bump `currentFirmwareVersion` to `"1.1.1"` (no other code changes needed)
2. Build firmware: `~/.platformio/penv/bin/pio run -e release`
3. Create GitHub Release with tag `1.1.1`, upload `firmware.bin` as asset
4. A device already running v1.1.0 should detect `1.1.1` via the GitHub API and OTA update successfully

### After Validation

Once Phase 2 succeeds, kellerdigital.com is no longer needed for OTA. Future releases (1.1.2, 1.2.0, etc.) go exclusively through GitHub Releases.

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
   Response includes `"tag_name": "1.1.1"` — this is the version string.

2. **Firmware download** (Release asset — returns 302 redirect to CDN):
   ```
   https://github.com/GDKeller/JPL-MiniPulse/releases/latest/download/firmware.bin
   ```

The API endpoint returns JSON directly (no redirect). The asset download URL returns a **302 redirect** to the actual CDN URL. The ESP32 HTTP client must follow redirects for the binary download.

### Redirect Handling

- `HTTPClient` supports `setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS)` or `HTTPC_FORCE_FOLLOW_REDIRECTS`
- `httpUpdate` also supports `setFollowRedirects()` — confirmed in the installed ESP32 framework (`HTTPUpdate.h`). It passes the setting through to its internal `HTTPClient`. This means we can use `httpUpdate` directly without manual redirect resolution.

### Failure During Update (Power Loss / Network Drop)

The `release` and `debug` builds use `min_spiffs.csv`, which has a dual A/B partition layout (`ota_0` + `ota_1`). The ESP32 OTA mechanism writes new firmware to the **inactive** partition and only flips the bootloader flag after the full binary is written and verified.

| Failure Scenario | What Happens |
|-----------------|--------------|
| **Internet drops mid-download** | `httpUpdate` returns `HTTP_UPDATE_FAILED` (timeout). Inactive partition has partial write but bootloader flag never switches. Device stays on current firmware. |
| **Power loss mid-write** | Same — `otadata` flag hasn't been flipped. Device reboots into current firmware on the intact active partition. |
| **Power loss after write, before reboot** | `otadata` flag already updated. Device boots into new firmware on next power-up. This is fine — the write completed successfully. |

No code changes needed — the A/B partition scheme handles this inherently.

**Note:** The `devdebug` env uses `huge_app.csv` which has only one app partition (no `ota_1`). OTA updates will fail on that build since there's no partition to write to. This is expected — `devdebug` is for JTAG debugging, not production use.

### Memory Impact

- `WiFiClientSecure` uses ~20-40KB more heap than `WiFiClient`
- Current free heap at runtime is ~100-150KB (from diagnostics), so this is feasible
- The secure client should be created on the stack or as a local variable, not a persistent global, to free memory after update

## Files to Modify

| File | Change |
|------|--------|
| `src/main.cpp` | Add `#include <WiFiClientSecure.h>`, update URLs, switch to WiFiClientSecure, enable redirect following on httpUpdate |
| `platformio.ini` | No changes needed (WiFiClientSecure is part of ESP32 Arduino core) |

## Implementation Details

### Step 1: Define GitHub URLs

**File:** `src/main.cpp` (top of file, near version string)

```cpp
// v1.1.0 ships with these URLs — first GitHub Release will be v1.1.1
const char* currentFirmwareVersion = "1.1.0";
const char* githubApiUrl = "https://api.github.com/repos/GDKeller/JPL-MiniPulse/releases/latest";
const char* firmwareBinaryUrl = "https://github.com/GDKeller/JPL-MiniPulse/releases/latest/download/firmware.bin";
```

### Step 2: Add semver comparison helper

Replace the `strcmp != 0` check with a proper semver comparison. Parses `major.minor.patch` from both strings and returns true only when the remote version is strictly newer.

```cpp
// Returns true if `remote` is a higher semver than `current`.
// Expects format "major.minor.patch" (e.g. "1.2.3"). Returns false on parse failure.
bool isNewerVersion(const char* remote, const char* current) {
    int rMajor = 0, rMinor = 0, rPatch = 0;
    int cMajor = 0, cMinor = 0, cPatch = 0;

    if (sscanf(remote, "%d.%d.%d", &rMajor, &rMinor, &rPatch) != 3) return false;
    if (sscanf(current, "%d.%d.%d", &cMajor, &cMinor, &cPatch) != 3) return false;

    if (rMajor != cMajor) return rMajor > cMajor;
    if (rMinor != cMinor) return rMinor > cMinor;
    return rPatch > cPatch;
}
```

Then update `checkFirmwareUpdateAvailable()` to use it:

```cpp
bool checkFirmwareUpdateAvailable() {
    const char* latestVersion = getRemoteFirmwareVersion();

    if (strcmp(latestVersion, "") == 0) {
        return false;
    }

    return isNewerVersion(latestVersion, currentFirmwareVersion);
}
```

### Step 3: Update getRemoteFirmwareVersion()

**Current** (~line 391): Uses `HTTPClient` with plain HTTP to fetch a text file.

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

    if (!httpFirmware.begin(secureClient, githubApiUrl)) {
        if (showSerial) Serial.println("Failed to begin HTTP connection for version check");
        return buffer;
    }

    // Headers must be added after begin() — begin() may clear internal state
    httpFirmware.addHeader("Accept", "application/vnd.github+json");
    httpFirmware.addHeader("User-Agent", "JPL-MiniPulse-ESP32"); // GitHub API requires User-Agent

    int httpCode = httpFirmware.GET();
    if (showSerial) Serial.printf("Version check HTTP code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
        // Parse directly from stream to avoid allocating full response (5-20KB) on heap
        WiFiClient& stream = httpFirmware.getStream();

        StaticJsonDocument<64> filter;
        filter["tag_name"] = true;

        DynamicJsonDocument doc(512);
        DeserializationError err = deserializeJson(doc, stream, DeserializationOption::Filter(filter));

        if (err == DeserializationError::Ok && doc.containsKey("tag_name")) {
            const char* tagName = doc["tag_name"];
            if (showSerial) Serial.printf("Remote version (tag): %s\n", tagName);
            strncpy(buffer, tagName, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        } else {
            if (showSerial) Serial.println("Failed to parse tag_name from GitHub API response");
        }
    } else {
        if (showSerial) Serial.printf("Failed to fetch remote version (HTTP %d)\n", httpCode);
    }

    httpFirmware.end();
    return buffer;
}
```

**Note on memory:** The GitHub API response for `/releases/latest` can be large (~5-20KB depending on release notes and assets). By using `getStream()` with ArduinoJson's filter, we parse directly from the HTTP stream without allocating the full response as a `String` on the heap. This is critical — on a ~100-150KB heap, avoiding a 5-20KB intermediate allocation matters.

### Step 4: Update updateFirmwareOta()

**Current** (~line 439): Uses `WiFiClient` (plain HTTP) with the kellerdigital.com URL.

**Proposed:** Use `WiFiClientSecure` with `httpUpdate`'s native redirect support. The ESP32 framework's `HTTPUpdate` class has `setFollowRedirects()` which passes through to its internal `HTTPClient`, so no manual redirect resolution is needed.

```cpp
void updateFirmwareOta() {
    if (!checkFirmwareUpdateAvailable()) return;
    bool showSerial = FileUtils::config.debugUtils.showSerial;

    if (showSerial) Serial.println("\n--------\nAttempting firmware update from GitHub...");

    WiFiClientSecure secureClient;
    secureClient.setInsecure();

    httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

    t_httpUpdate_return ret = httpUpdate.update(secureClient, firmwareBinaryUrl);

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            if (showSerial) Serial.printf(">> HTTP_UPDATE_FAILED Error (%d): %s\n",
                httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            if (showSerial) Serial.println(">> HTTP_UPDATE_NO_UPDATES");
            break;
        case HTTP_UPDATE_OK:
            if (showSerial) Serial.println(">> HTTP_UPDATE_OK");
            break;
        default:
            if (showSerial) Serial.println(">> HTTP_UPDATE Unknown result");
            break;
    }

    if (showSerial) Serial.println("OTA update complete\n--------\n");
}
```

**Fallback:** If `httpUpdate.setFollowRedirects()` doesn't handle GitHub's cross-host redirect chain (GitHub → CDN) properly on hardware, the fallback is to manually resolve the redirect via a HEAD request and pass the final CDN URL to `httpUpdate.update()`. This needs to be tested on hardware.

### Step 5: Create Release Workflow

For the v1.1.0 bridge release (Phase 1 — old OTA path):

1. Set `currentFirmwareVersion = "1.1.0"` in `src/main.cpp`
2. Build firmware: `~/.platformio/penv/bin/pio run -e release`
3. Upload `firmware.bin` to `develop.kellerdigital.com/minipulse/`
4. Update `latest_version.txt` on the server to `1.1.0`

For v1.1.1 and all future releases (Phase 2+ — GitHub OTA path):

1. Update `currentFirmwareVersion` in `src/main.cpp` (e.g., `"1.1.1"`)
2. Build firmware: `~/.platformio/penv/bin/pio run -e release`
3. Create a GitHub Release with tag matching the version (e.g., tag `1.1.1`)
4. Upload `firmware.bin` (from `.pio/build/release/firmware.bin`) as a release asset

That's it — the device reads the version from the release tag, so no extra files needed.

This could be automated with GitHub Actions in the future.

### Note on #39 (`.catch()` on firmware version fetch)

The current portal JavaScript fetches the version without error handling. This is addressed by this plan — the rewritten `getRemoteFirmwareVersion()` handles HTTP failures gracefully and the portal JS should be updated to handle fetch errors at the same time. No need for a separate fix.

## Testing

### Phase 1 Validation (v1.1.0 via kellerdigital.com)

1. Flash v1.1.0 to a device via USB or existing OTA
2. Verify the device's portal shows version `1.1.0`
3. Confirm the GitHub OTA code compiles and doesn't break existing functionality
4. (The old kellerdigital.com OTA path should still work for distributing 1.1.0 to field devices)

### Phase 2 Validation (v1.1.1 via GitHub Releases)

1. Create GitHub Release tagged `1.1.1` with a built `firmware.bin`
2. On a device running v1.1.0, trigger a firmware update check
3. Verify version check hits the GitHub API and returns `1.1.1` over HTTPS
4. Verify redirect following works (GitHub → CDN) for the binary download
5. Verify firmware download and flash succeeds — device reboots into v1.1.1
6. Monitor heap usage during HTTPS operations (WiFiClientSecure overhead)

### Failure Cases

1. No WiFi — update check should fail gracefully, no crash
2. GitHub API down or rate-limited (HTTP 403) — should report error, not hang
3. No releases exist yet (HTTP 404) — should return empty version, no update offered
4. Invalid/corrupt binary on GitHub — httpUpdate should reject it
5. Slow network — 10s timeout should prevent indefinite blocking

## Risks

| Risk | Severity | Mitigation |
|------|----------|------------|
| GitHub redirect chain breaks httpUpdate | MED | `httpUpdate.setFollowRedirects()` confirmed in framework; test on hardware; manual redirect resolution as fallback |
| WiFiClientSecure heap usage too high | MED | Create client as local variable; monitor free heap |
| GitHub rate limiting (60 req/hr unauthenticated) | LOW | Device only checks on portal interaction, not polling |
| HTTPS handshake timeout on slow WiFi | MED | Set 10s timeout; retry logic already exists |
| Accidental version downgrade | LOW | Mitigated: `isNewerVersion()` does proper semver comparison (major.minor.patch), only triggers when remote is strictly newer |
| No releases exist yet (404 from API) | LOW | Handled by the `httpCode != HTTP_CODE_OK` path — returns empty buffer, no update offered |

## Estimated Scope

- 1 file modified (`src/main.cpp`)
- ~40 lines changed
- No new dependencies (WiFiClientSecure is part of ESP32 core)
- Two-phase rollout: v1.1.0 via old server, v1.1.1 via GitHub Releases
- Requires hardware testing for redirect behavior
- kellerdigital.com can be decommissioned for OTA after v1.1.1 is validated
