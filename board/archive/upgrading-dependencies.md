# Upgrading Dependencies

All library versions in `platformio.ini` are pinned to exact versions for reproducible builds. This means PlatformIO will not automatically pull newer versions.

## Current Pinned Versions

| Library | Version | Source |
|---------|---------|--------|
| ArduinoJson | 6.21.5 | PlatformIO registry |
| WiFiManager | v2.0.17 | GitHub tag |
| tinyxml2 | 9.0.0 | GitHub tag |
| FastLED | 3.10.3 | PlatformIO registry |

## How to Upgrade a Dependency

1. **Check for available updates:**
   ```bash
   # List outdated libraries
   ~/.platformio/penv/bin/pio pkg outdated
   ```

2. **Review the changelog** for the library you want to upgrade. Look for breaking changes, especially across major versions (e.g., ArduinoJson v6 -> v7 is breaking).

3. **Update the version in `platformio.ini`** across all three environments (`release`, `debug`, `devdebug`). All envs must stay in sync.

4. **Clean and rebuild:**
   ```bash
   # Remove cached libraries to force fresh download
   rm -rf .pio/libdeps/

   # Rebuild
   ~/.platformio/penv/bin/pio run -e release
   ```

5. **Test on hardware** before committing. Embedded library updates can introduce subtle runtime issues that compile fine.

6. **Commit the version bump** with a message like:
   ```
   chore(deps): bump FastLED 3.10.3 -> 3.11.0
   ```

## Notes

- **ArduinoJson**: v7 is a major rewrite with a different API. Stay on v6.x unless you're ready to migrate.
- **FastLED**: Generally safe to bump within the 3.x line, but test LED output after upgrading.
- **WiFiManager**: Pinned via GitHub tag (`#v2.0.17`). To upgrade, change the tag in the URL.
- **tinyxml2**: Pinned via GitHub tag (`#9.0.0`). Same process as WiFiManager.
