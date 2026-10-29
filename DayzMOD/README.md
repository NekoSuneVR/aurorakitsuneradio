# AuroraKitsuneRadio Radio

**Current source version: 0.2.2 (GUI/keybind beta).** The version in `server/mod.cpp` must be copied into the published mod folder when rebuilding; publishing to Steam Workshop remains a separate action.

Live internet radio for DayZ handheld radios and vehicles, with experimental Radio_Fungi cassette coexistence.

**Based on:** [LootTeamRadio by offmonreal](https://github.com/offmonreal/LootTeamRadio). AuroraKitsuneRadio Radio is an independent fork that retains credit to the original project and its contributors. See [LICENSE](LICENSE) for licensing terms.

## Features

- 8–48 configurable internet radio stations, including TruckersFM, NekoSune Community Radio, NCS-themed streams and Capital FM UK.
- In-car radio presets using chat commands, without needing a handheld radio item.
- Existing Personal Radio support.
- Radio_Fungi coexistence: automatically pauses the livestream when a cassette is detected in a vehicle's `Cassette` attachment slot.
- Per-player volume settings. Radio audio plays locally through MPC-HC, **not** as shared in-game vehicle audio.

## Setup

1. Install [MPC-HC](https://github.com/clsid2/mpc-hc/releases) on Windows and keep it running.
2. In **View > Options > Player > Web Interface**, enable **Listen on port** and enter `13579`.
3. Install a matching signed AuroraKitsuneRadio Radio client mod and join a server that has integrated its mission-side station configuration.
4. Use a battery-powered **Personal Radio** in your hands, or enter a vehicle with its engine running and select a live station.

## Remote GitHub station catalogue (v0.2.2 test mode)

At mission startup the Windows client requests the JSON catalogue asynchronously from:

`https://raw.githubusercontent.com/NekoSuneProjectsForks/NekoWave/feature/vehicle-radio-multi-stations/server/radio-stations.json`

This **does not require rebuilding the PBO when the JSON content changes** after v0.2.2 is published: edit and commit `server/radio-stations.json` in the linked branch, then reconnect/relaunch the DayZ mission to fetch the latest response. If the request fails (offline, blocked, invalid JSON), the bundled 48 presets remain available. If the active DayZ server sends a station list over the existing RPC, **the server list wins**. Currently, only 8–48 stations are supported; this is a temporary provider and **not yet** a geo-location radio API, geographic search, or an unlimited station library. Future APIs can replace the source and expand the pagination protocol. The HTTP request is made by each client; no Steam Workshop update is needed for subsequent edits to that JSON once this release is installed.

## In-game station menu (experimental)

**v0.2.2 UI fixes:** Bundled 48-station catalogue is shown even if the server has not supplied station metadata yet. A later server snapshot still replaces the bundled catalogue. Station buttons now receive click events directly; menu no longer manipulates the global game-focus counter, and Escape/native game menus provide an emergency close path. If the server mission integration is not installed, your player may see only the bundled preset names instead of the server's custom list. Changes require rebuilding, signing and republishing the client PBO.


A new toggleable radio panel is included. In DayZ **Settings > Controls**, find **AuroraKitsuneRadio Radio → Toggle Radio Menu**. The default is **F7**, and you can remap or clear it just like other DayZ controls. Some other mods may also use F7, so change it if there is a conflict.

- **In a vehicle:** press your assigned hotkey to open/close the station picker; browse eight stations per page, select any of the 48 presets or switch the stream off. Radio_Fungi cassette attachments still have priority.
- **With a Personal Radio in your hands:** the same hotkey opens the station catalogue, but the eight handheld FM frequencies must still be tuned on the physical radio. Vehicle-only slots 9–48 cannot be tuned on an unmodified handheld.
- **Elsewhere:** the hotkey does nothing. The panel closes automatically if you leave the vehicle or stop holding the Personal Radio.
- The panel uses MPC-HC as before; it is not in-game 3D audio. The new UI and keybinding need compilation and in-game testing before release.

## Commands

| Command | Function |
| --- | --- |
| `!radio car 1` to `!radio car 48` | Choose a vehicle livestream station |
| `!radio car` | Select vehicle preset 1 |
| `!radio car off` | Turn off the vehicle livestream |
| `!radio car fungi` | Turn off the livestream before using Radio_Fungi |
| `!radio 50` | Set personal radio volume to 50% (0-100) |
| `!radio help` | Display player help |

Server admins can edit the station catalogue in `$profile:DayZRadio/radio-stations.json`. See [server setup](docs/SERVER.md) and [build instructions](docs/BUILD.md).

## More stations

The server accepts 8–48 ordered station entries in `$profile:DayZRadio/radio-stations.json`. The first eight retain their original physical handheld frequencies. Extra stations are **vehicle-only** and must use sequential slot IDs and unique virtual frequency keys: slot 9 uses `frequency: 200.9`, slot 10 uses `201.0`, and slot 48 uses `204.8` (formula: `(2000 + slotId) / 10`). These are internal identifiers, not actual DayZ radio frequencies.

The included sample now has **48 stations**, spanning UK radio, Canadian stations, US independent stations, gaming radio, SomaFM, Radio Paradise, TruckersFM, and NekoSune Community Radio. Live endpoints can change or have geographic and licensing restrictions; test them before running a public server. Add/edit entries on the server and run admin `!radio reload` to send the new list to connected players. Clients must use this updated mod version.

**GUI status:** A basic paginated in-game station picker with a configurable toggle hotkey is implemented but untested. Search, categories and favourites are still planned. Until then, select extra stations with `!radio car 9`, `!radio car 10` etc.

### Station catalogue regions

- **UK:** Capital, Heart 80s, BBC World Service, LBC.
- **Canada:** CIUT Toronto, Rock 101, CFOX, CJAY 92, The Bear, RdMix.
- **USA:** KEXP, WFMU, SomaFM.
- **Gaming/community:** TruckersFM, Simulator Radio, NekoSune Community Radio.

These are example stream presets, not endorsements, and they have not been tested with MPC-HC in DayZ. The bundled catalogue is a server default; an existing `$profile:DayZRadio/radio-stations.json` takes priority. Add the new entries to that file to upgrade an existing server, then run `!radio reload`.

### Added electronic and lo-fi streams

- NCS-themed Dubstep, Trap, Phonk and Pop via the third-party CLIAMP streams.
- CLIAMP Lo-fi Hip Hop, Synthwave, EDM, Chiptunes and Amiga Tracker.
- SomaFM Cliqhop IDM and Retro Lofi Chiptune Radio.

These are third-party direct stream addresses, **not official NCS broadcasts**. The CLIAMP chiptune and Amiga stations may be temporarily quiet. Official Monstercat and Chillhop services provide listening channels, but no independently verified, stable, directly usable MPC-HC stream URLs were added for them; do not mistake a web player URL or YouTube live URL for an MP3 stream. Check third-party terms, licensing, uptime and regional availability.

### Hardstyle, uptempo, frenchcore and dubstep

Slots **39–48** include MixStream Hardstyle, Platinum Hardstyle, Hardstyle FM, Real Hardstyle Radio, Technolovers Frenchcore, two FrenchcoreRadio.eu endpoints (including a channel carrying uptempo mixes), FrenchcoreRadio.nl, and two SomaFM Dub Step Beyond quality options.

All catalogue slots are now filled (**48/48**). These are third-party streams that might be offline, have certificate/redirect issues, or limit access by region. In particular, direct HTTPS links to IP addresses may fail certificate validation on some players. Test each URL in MPC-HC from the client PC before using it. The two SomaFM entries are the same channel at different bitrates, and should not be described as separate programming.

## Radio_Fungi coexistence

AuroraKitsuneRadio Radio pauses its external livestream when it detects an item attached to the vehicle's `Cassette` slot. Removing the cassette lets the chosen livestream resume when the engine is running. **This checks whether a cassette is inserted, not whether the cassette is currently playing.** It does not control Radio_Fungi or alter its files.

Compatibility with Radio_Fungi and different vehicle mods is **not yet verified in DayZ**. Test both mods together on a staging server before deploying. Stream availability, regional access, and content rights are the server operator's responsibility.

## Development and credits

- **Original mod:** [offmonreal/LootTeamRadio](https://github.com/offmonreal/LootTeamRadio)
- **Enhanced fork:** [NekoSuneProjectsForks/AuroraKitsuneRadio](https://github.com/NekoSuneProjectsForks/NekoWave)
- Internal mod and PBO identifiers are now `AuroraKitsuneRadio`. Rebuild and reinstall after upgrading; existing player settings remain under `$profile:DayZRadio`.
- DayZ, MPC-HC, and Radio_Fungi belong to their respective owners.
