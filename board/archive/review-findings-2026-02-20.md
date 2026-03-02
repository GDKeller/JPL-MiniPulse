# Security Hardening Review Findings — 2026-02-20

Pre-existing issues surfaced during review of the `fix/security-hardening` branch. These are **not regressions** from the security hardening work but are worth tracking for future cleanup.

---

## 1. Dead `brightnessMapped` variable

**File:** `src/main.cpp`, ~line 1005
**Severity:** Low

```cpp
int brightnessMapped = MathHelpers::map(brightnessInt, 0, 100, 8, 160);
```

This variable is computed but never used. `brightnessInt` (already clamped to [8, 160]) is written directly to config at line 1009. The `map()` call assumes input range [0, 100] but receives values in [8, 160], so it would produce incorrect results if it were ever wired up. This appears to be a leftover from an older percentage-based implementation.

**Fix:** Remove the dead `map()` call and the `brightnessMapped` variable entirely.

---

## 2. `force_dummy_data` parameter name mismatch

**File:** `src/main.cpp`, line ~3316 vs line ~1023
**Severity:** Medium (feature is non-functional)

The WiFiManagerParameter is registered with the HTML field name `"force-dummy-data"` (hyphens), but `saveParamsCallback()` reads it with `getParam("force_dummy_data")` (underscores). Since these don't match, `getParam` always returns `"0"` — the force dummy data toggle can never be set to `1` from the portal.

**Fix:** Change the parameter ID to `"force_dummy_data"` to match the `getParam` call and be consistent with other parameters.

---

## 3. `force_dummy_data` parameter initialization is copy-pasted from brightness

**File:** `src/main.cpp`, line ~3316
**Severity:** Medium

```cpp
new (&param_force_dummy_data) WiFiManagerParameter(
    "force-dummy-data",
    "Force placeholder data",
    String(FileUtils::config.displayLED.brightness).c_str(),  // wrong default (uses brightness value)
    3,
    "type='range' min='8' max='160' step='1'"                 // wrong constraints (brightness range)
);
```

The default value and HTML input constraints are copied from the brightness parameter. Should be a 0/1 toggle instead.

**Fix:**
```cpp
new (&param_force_dummy_data) WiFiManagerParameter(
    "force_dummy_data",
    "Force Dummy Data",
    FileUtils::config.wifiNetwork.forceDummyData ? "1" : "0",
    1,
    "type='number' min='0' max='1' step='1'"
);
```

---

## 4. Missing `.catch()` on firmware version fetch

**File:** `src/main.cpp`, inside `update_button_html` raw string (~line 3342)
**Severity:** Low

The `fetch('/get-latest-version-number')` promise chain has no `.catch()` handler, unlike the `/trigger-firmware-update` fetch below it. Network failures silently leave the `#firmwareStatus` element empty with no user feedback.

**Fix:** Add a `.catch()` handler:
```javascript
.catch(function (error) {
    document.getElementById("firmwareStatus").textContent = "Failed to check firmware version";
    console.error(error);
});
```

---

## 5. Plaintext HTTP firmware download

**File:** `src/main.cpp`, lines ~451 and ~503
**Severity:** High (security)

Both the version check and firmware binary download use plain HTTP:
```cpp
httpFirmware.begin("http://develop.kellerdigital.com/minipulse/latest_version.txt");
t_httpUpdate_return ret = httpUpdate.update(client, "http://develop.kellerdigital.com/minipulse/firmware.bin");
```

A MITM attacker could replace the firmware binary with a malicious one, achieving full device compromise. This is the highest-severity security issue in the codebase.

**Fix:** Switch to HTTPS with `WiFiClientSecure` and certificate pinning, or at minimum HTTPS with root CA verification. This is likely a separate roadmap item.
