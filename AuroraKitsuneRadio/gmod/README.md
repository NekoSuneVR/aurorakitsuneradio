# AuroraKitsuneRadio for Garry's Mod

Client-side Lua radio panel with a searchable internet station list and native `sound.PlayURL` audio playback. No Hive365 account, binaries or external sound addon required.

## Install (no compilation)

1. Create `garrysmod/addons/aurorakitsuneradio/lua/autorun/client/`.
2. Copy `lua/autorun/client/aurorakitsuneradio.lua` from this folder into that directory.
3. Restart Garry's Mod or reload the addon.
4. Open the developer console and run:
   ```
   aurora_radio
   ```
5. Search by station name, country or genre. Double-click a station to listen.

## Console commands

| Command | Purpose |
| --- | --- |
| `aurora_radio` | Open the station browser |
| `aurora_radio_stop` | Stop playback |
| `aurora_radio_volume 35` | Set volume from 0–100 |
| `aurora_radio_refresh` | Reload stations from the API |

The addon downloads the full enabled station catalogue from `/api/stations` into client memory; it renders only the current page, so the list isn't restricted to 48.

## Limitations and troubleshooting

Audio is heard **only on the local client**, not broadcast to other players; there is no 3D proximity audio. Source's supported codecs/streams may differ from the API's reported formats. The current Lua source has not been tested in Garry's Mod; inspect console Lua errors and network failures if the menu or audio does not work.

If stations do not appear, check client access to `https://aurora-kitsune.fumikoecho.ca/api/stations`. If a station is listed but silent, test the actual stream format and endpoint. The `volume` convar is client-side.
