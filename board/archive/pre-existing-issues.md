# Pre-existing Issues

Discovered during code review on 2026-02-20. These are not regressions — they exist on `main`.

## 1. Inconsistent checkbox parsing for showSerial / showDiagnostics

**Location:** `src/main.cpp` ~lines 949, 971
**Severity:** Medium

The `show_serial` and `show_diagnostics` checkbox params use a strict `strcmp("1")` check:

```cpp
bool showSerialValueBool = strcmp(showSerialValue.c_str(), "1") == 0 ? true : false;
```

The newer checkboxes (`force_dummy_data`, `force_animation_enabled`) use a lenient check:

```cpp
bool enabled = (value.length() > 0 && value != "0");
```

If WiFiManager or the browser ever sends a truthy value other than `"1"` (e.g. `"on"`), the strict check would silently treat it as `false`. Should align all checkbox parsing to the lenient pattern.

## 2. strtol parse failure silently clamps to default

**Location:** `src/main.cpp` ~line 996
**Severity:** Low

`strtol` is called with `nullptr` as the `endptr`, so a complete parse failure (non-numeric input) returns `0` and silently clamps to `1`:

```cpp
long rawAnimType = strtol(forcedAnimationTypeValue.c_str(), nullptr, 10);
animate.forcedAnimationType = (rawAnimType < 1) ? 1 : (rawAnimType > 5) ? 5 : (int)rawAnimType;
```

Low practical risk since the HTML `type='number'` constrains browser input, but could be improved by using the `endptr` to detect and log parse failures.

## 3. Division by zero in heap fragmentation diagnostic

**Location:** `src/main.cpp` ~line 2058
**Severity:** Medium

```cpp
Serial.print((ESP.getMaxAllocHeap() * 100) / ESP.getFreeHeap());
```

If `ESP.getFreeHeap()` returns `0` (extreme memory exhaustion), this causes a hardware exception and crash on ESP32. This is the exact scenario where diagnostics would be most useful.

**Fix:**

```cpp
uint32_t freeHeap = ESP.getFreeHeap();
if (freeHeap > 0) {
    Serial.print((ESP.getMaxAllocHeap() * 100) / freeHeap);
} else {
    Serial.print("ERR");
}
```
