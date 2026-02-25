# Roadmap — v1.1.0

Current dev version: **1.1.0-12**

Items sourced from [codebase-analysis.md](codebase-analysis.md) and review findings (2026-02-20). Deferred items moved to [BACKLOG.md](BACKLOG.md).

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

## Short-Term — Next Sprint

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 9 | `randomTypeAny` uninitialized | CQ6 | LOW | FIX | DONE — `b4bae67` |
| 10 | `freeSemaphoreItem` dangling pointer | CQ7 | LOW | FIX | DONE — `b4bae67` |
| 11 | `colorTheme` global shadows config | CQ9 | LOW | FIX | DONE — `b4bae67` |
| 12 | `firstStartupAnimation` logic inverted | CQ10 | LOW | FIX | DONE — `b4bae67`, `e446cb4` |
| 13 | `strncpy` null-termination | CQ11 | LOW | FIX | DONE — `b4bae67` |
| 14 | Pin dependency versions | S7 | LOW | SEC | DONE |
| 15 | Portal input validation | S5 | MED | SEC | DONE (already secure) |
| 16 | `innerHTML` to `textContent` | S10 | LOW | SEC | DONE (already uses textContent) |
| 39 | Add `.catch()` to firmware version fetch | RF4 | LOW | FIX | DONE |
| 18 | Eliminate LittleFS round-trip | P2 | MED | OPT | DONE — `3c57fcc` |
| 19 | Add frame rate limiter | P3 | MED | OPT | DONE — `85870c9` |
| 20 | Replace `std::map` in `termColor()` | P4 | MED | OPT | DONE — `7f74634` |

## Medium-Term — Next Month

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 23 | Remove commented-out code | A5 | MED | CLEAN | TODO |
| 24 | Fix SpacecraftData write-then-read | A3 | MED | OPT | DONE |
| 25 | Reduce semaphore hold time | P5 | MED | OPT | TODO |
| 27 | Optimize boot sequence | P7 | MED | OPT | TODO |
| 29 | Serial output gating | S9 | LOW | CLEAN | TODO |

## Long-Term — Future Releases

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 35 | Move font data to PROGMEM | A3 | MED | OPT | TODO |
| 36 | Batch config file writes | A2 | MED | CLEAN | TODO |

## Additional v1.1.0 Work

| # | Issue | Ref | Risk | Type | Status |
|---|-------|-----|------|------|--------|
| 37 | Binary up signal logic | [PLAN](PLAN-binary-upsignal.md) | MED | FIX | DONE — `fe3f7fc` |
| 38 | Clean up placeholder rate system (after #37) | — | LOW | CLEAN | DONE |
| 40 | GitHub-based OTA firmware updates | [PLAN](PLAN-github-ota.md) | MED | FEAT | TODO |
| 41 | Remote spacecraft data (fetch + cache from GitHub) | [PLAN](PLAN-remote-spacecraft-data.md) | HIGH | FEAT | TODO |
| 42 | Full vs restricted spacecraft list setting | [PLAN](PLAN-remote-spacecraft-data.md) | MED | FEAT | TODO |
| 43 | Update recognized spacecraft callsigns | — | LOW | FIX | TODO |

---

## Dev Version Log

| Version | Date | Summary |
|---------|------|---------|
| 1.1.0-1 | 2026-02-19 | Fixed 5 critical bugs (CQ1–CQ5). OTA WiFi flashing confirmed working. |
| 1.1.0-2 | 2026-02-19 | Fixed 5 short-term code quality issues (CQ6–CQ11). Rewrote pool allocator to fix duplicates/leaks. Fixed display timer reset. Added boot banner color reset. |
| 1.1.0-3 | 2026-02-19 | Performance optimizations: replaced std::map in termColor(), added 30 FPS frame limiter with heap/fragmentation diagnostics, eliminated LittleFS round-trip in data fetch. |
| 1.1.0-11 | 2026-02-24 | Binary up signal detection (#37): simplified from rate-based to binary. Hoisted inline constants to file scope (meteorTimingTable, animationTypeCanSpiralTable, portalHeadHtml, maxHttpRetries). Removed redundant inner_leds_size and animationId variables. |
| 1.1.0-12 | 2026-02-24 | Removed placeholder rate system (#38): deleted getPlaceholderRate(), spacecraftPlaceholderRatesJson (6KB), and all placeholder_rates.json file operations from SpacecraftData. |
