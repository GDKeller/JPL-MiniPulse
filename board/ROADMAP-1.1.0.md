# Roadmap — v1.1.0

Current dev version: **1.1.0-3**

Items sourced from [codebase-analysis.md](codebase-analysis.md) and review findings (2026-02-20).

**Risk** = how much code changes. LOW = 1-5 lines, MED = 10-50 lines, HIGH = 50+ lines or structural.
**Type**: FIX = bug/correctness, OPT = performance optimization, SEC = security, CLEAN = code hygiene/nice-to-have.

---

## Immediate — Critical Bugs + Security

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 1 | Queue created after task | CQ1 | LOW | FIX | DONE — `dd8c388` |
| 2 | `==` instead of `=` on semaphore flag | CQ2 | LOW | FIX | DONE — `dd8c388` |
| 3 | Queue sends pointer-to-pointer | CQ3 | LOW | FIX | DONE — `dd8c388` |
| 4 | Pool allocator reads wrong index | CQ4 | MED | FIX | DONE — `dd8c388`, `f097b40` |
| 5 | `isWiFiConnected` missing `()` | CQ5 | LOW | FIX | DONE — `dd8c388` |
| 6 | OTA over plain HTTP | S1 | MED | SEC | DEFERRED |
| 7 | Unauthenticated OTA trigger | S2 | MED | SEC | DEFERRED |
| 8 | Open WiFi AP | S3 | LOW | SEC | DEFERRED |

## Short-Term — Next Sprint

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 9 | `randomTypeAny` uninitialized | CQ6 | LOW | FIX | DONE — `b4bae67` |
| 10 | `freeSemaphoreItem` dangling pointer | CQ7 | LOW | FIX | DONE — `b4bae67` |
| 11 | `colorTheme` global shadows config | CQ9 | LOW | FIX | DONE — `b4bae67` |
| 12 | `firstStartupAnimation` logic inverted | CQ10 | LOW | FIX | DONE — `b4bae67`, `e446cb4` |
| 13 | `strncpy` null-termination | CQ11 | LOW | FIX | DONE — `b4bae67` |
| 14 | Pin dependency versions | S7 | LOW | SEC | TODO |
| 15 | Portal input validation | S5 | MED | SEC | TODO |
| 16 | `innerHTML` to `textContent` | S10 | LOW | SEC | TODO |
| 39 | Add `.catch()` to firmware version fetch | RF4 | LOW | FIX | TODO |
| 17 | Meteor pool allocator | P1 | HIGH | OPT | TODO |
| 18 | Eliminate LittleFS round-trip | P2 | MED | OPT | DONE — `3c57fcc` |
| 19 | Add frame rate limiter | P3 | MED | OPT | DONE — `85870c9` |
| 20 | Replace `std::map` in `termColor()` | P4 | MED | OPT | DONE — `7f74634` |

## Medium-Term — Next Month

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 21 | Extract modules from main.cpp | A1 | HIGH | CLEAN | TODO |
| 22 | Unify configuration system | A2 | HIGH | CLEAN | TODO |
| 23 | Remove commented-out code | A5 | MED | CLEAN | TODO |
| 24 | Fix SpacecraftData write-then-read | A3 | MED | OPT | TODO |
| 25 | Reduce semaphore hold time | P5 | MED | OPT | TODO |
| 26 | Disable AP after WiFi config | P6 | LOW | OPT | TODO |
| 27 | Optimize boot sequence | P7 | MED | OPT | TODO |
| 28 | Implement proper state machine | A1 | HIGH | CLEAN | TODO |
| 29 | Serial output gating | S9 | LOW | CLEAN | TODO |
| 30 | Firmware signing | S1 | HIGH | SEC | TODO |

## Long-Term — Future Releases

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 31 | ESP32 Secure Boot V2 | S1 | HIGH | SEC | TODO |
| 32 | Flash encryption | S13 | HIGH | SEC | TODO |
| 33 | Production OTA domain | S8 | LOW | SEC | TODO |
| 34 | Compact meteor data structure | P8 | MED | OPT | TODO |
| 35 | Move font data to PROGMEM | A3 | MED | OPT | TODO |
| 36 | Batch config file writes | A2 | MED | CLEAN | TODO |

## Additional v1.1.0 Work

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 37 | Binary up signal logic | [PLAN](PLAN-binary-upsignal.md) | MED | FIX | TODO |
| 38 | Clean up placeholder rate system (after #37) | — | LOW | CLEAN | TODO |

---

## Dev Version Log

| Version | Date | Summary |
|---------|------|---------|
| 1.1.0-1 | 2026-02-19 | Fixed 5 critical bugs (CQ1–CQ5). OTA WiFi flashing confirmed working. |
| 1.1.0-2 | 2026-02-19 | Fixed 5 short-term code quality issues (CQ6–CQ11). Rewrote pool allocator to fix duplicates/leaks. Fixed display timer reset. Added boot banner color reset. |
| 1.1.0-3 | 2026-02-19 | Performance optimizations: replaced std::map in termColor(), added 30 FPS frame limiter with heap/fragmentation diagnostics, eliminated LittleFS round-trip in data fetch. |
