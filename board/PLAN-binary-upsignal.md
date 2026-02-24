# Planning Document: Binary Up Signal Logic

## Overview

Change the up signal animation logic from rate-based (6 levels) to binary (single medium animation when data type detected).

## Background

### Current Behavior

The system currently processes up signals identically to down signals:

1. Parse XML for `<upSignal>` elements
2. Check if `signalType="data"`
3. Extract `dataRate` attribute value
4. If rate is 0, use a placeholder rate from `SpacecraftData::getPlaceholderRate()`
5. Convert rate to animation class (1-6) via `rateLongToRateClass()`
6. Store in `CraftQueueItem.upSignal`

### Problem

The XML data source no longer includes meaningful `dataRate` values for up signals. The current logic falls back to placeholder rates, which adds complexity and doesn't reflect actual data.

### Proposed Behavior

Binary detection: If an `<upSignal>` with `signalType="data"` exists, trigger a medium animation. No rate parsing needed.

## Files to Modify

| File | Lines | Change |
|------|-------|--------|
| `src/main.cpp` | 2128-2159 | Simplify up signal detection to binary |

## Implementation Details

### Location: `findSignals()` function

**File:** `src/main.cpp`
**Lines:** ~2128-2159

### Current Code Flow

```cpp
const char* rate = xmlSignal->Attribute("dataRate");
if (rate == nullptr) continue;

double rateDouble = stod(rate);
unsigned long rateLong = static_cast<unsigned long>(rateDouble);

if (rateLong == 0) {
    if (isDown) {
        continue;
    } else {
        // Up signal: use placeholder rate
        const char* placeholderRate = SpacecraftData::getPlaceholderRate(tempNewCraft->callsign);
        rateDouble = stod(placeholderRate);
        rateLong = static_cast<unsigned long>(rateDouble);
    }
}

unsigned int rateClass = rateLongToRateClass(rateLong);

if (rateClass == 0) continue;

if (isDown == true) {
    tempNewCraft->downSignal = rateClass;
    foundSignals.downSignal = rateClass;
} else if (isDown == false) {
    tempNewCraft->upSignal = rateClass;
    foundSignals.upSignal = rateClass;
}
```

### Proposed Code Flow

```cpp
// For up signals: binary detection (medium animation if data type found)
if (!isDown) {
    // signalType="data" already verified at line 2123
    const unsigned int MEDIUM_RATE_CLASS = 3;
    tempNewCraft->upSignal = MEDIUM_RATE_CLASS;
    foundSignals.upSignal = MEDIUM_RATE_CLASS;
    continue;
}

// For down signals: keep existing rate-based logic
const char* rate = xmlSignal->Attribute("dataRate");
if (rate == nullptr) continue;

double rateDouble = stod(rate);
unsigned long rateLong = static_cast<unsigned long>(rateDouble);

if (rateLong == 0) continue;

unsigned int rateClass = rateLongToRateClass(rateLong);

if (rateClass == 0) continue;

tempNewCraft->downSignal = rateClass;
foundSignals.downSignal = rateClass;
```

## Rate Class Reference

| Class | Current Meaning | Animation Level |
|-------|-----------------|-----------------|
| 0 | No data | None |
| 1 | <1 Kbps | Slowest |
| 2 | <10 Kbps | Slow |
| 3 | <100 Kbps | Medium-slow |
| 4 | <500 Kbps | Medium-fast |
| 5 | <2.5 Mbps | Fast |
| 6 | >=2.5 Mbps | Fastest |

**Recommendation:** Use rate class `3` or `4` for the binary up signal animation.

## Code Cleanup Opportunities

After this change, consider removing:

1. **Placeholder rates for up signals** - No longer needed
   - `SpacecraftData::getPlaceholderRate()` calls for up signals
   - Potentially the entire placeholder rates system if only used for up signals

2. **Up signal rate parsing** - Entire dataRate extraction block for up signals

## Testing

1. Verify up signal animation triggers when `<upSignal signalType="data">` present
2. Verify no animation when up signal absent or `signalType` is not "data"
3. Verify down signal behavior unchanged (still rate-based)
4. Test with live DSN XML feed

## Risks

- **Low risk**: Change is isolated to up signal handling
- **No breaking changes**: Down signal logic remains untouched
- **Backwards compatible**: Old XML with dataRate values will still work (just ignored for up signals)

## Estimated Scope

- 1 file modified
- ~20 lines changed
- No new dependencies
