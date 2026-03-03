# Backlog

Deferred items from [ROADMAP-1.1.0.md](ROADMAP-1.1.0.md). To be revisited in future releases.

**Risk** = how much code changes. LOW = 1-5 lines, MED = 10-50 lines, HIGH = 50+ lines or structural.
**Type**: FIX = bug/correctness, OPT = performance optimization, SEC = security, CLEAN = code hygiene/nice-to-have.

---

## Security

| # | Issue | Ref | Risk | Type |
|---|-------|-----|------|------|
| 6 | OTA over plain HTTP | S1 | MED | SEC |
| 7 | Unauthenticated OTA trigger | S2 | MED | SEC |
| 8 | Open WiFi AP | S3 | LOW | SEC |
| 30 | Firmware signing | S1 | HIGH | SEC |
| 31 | ESP32 Secure Boot V2 | S1 | HIGH | SEC |
| 32 | Flash encryption | S13 | HIGH | SEC |
| 33 | Production OTA domain | S8 | LOW | SEC |

## Optimization

| # | Issue | Ref | Risk | Type |
|---|-------|-----|------|------|
| 17 | Meteor pool allocator | P1 | HIGH | OPT |
| 26 | Disable AP after WiFi config | P6 | LOW | OPT |
| 34 | Compact meteor data structure | P8 | MED | OPT |

## UI / Portal

| # | Issue | Ref | Risk | Type |
|---|-------|-----|------|------|
| 35 | OTA progress bar in portal UI | — | MED | FIX |

## Architecture / Code Hygiene

| # | Issue | Ref | Risk | Type |
|---|-------|-----|------|------|
| 21 | Extract modules from main.cpp | A1 | HIGH | CLEAN |
| 22 | Unify configuration system | A2 | HIGH | CLEAN |
| 28 | Implement proper state machine | A1 | HIGH | CLEAN |
