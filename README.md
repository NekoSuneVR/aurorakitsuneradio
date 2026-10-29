# AuroraKitsuneRadio

Cross-game internet radio project using the Aurora Kitsune public catalogue:

- **Station API:** `https://aurora-kitsune.fumikoecho.ca/api/stations`
- **Playback URL:** `https://aurora-kitsune.fumikoecho.ca` + each enabled station's `stream_path`
- **Station title:** `name`
- **Now playing:** not implemented in this project yet.

The API contains thousands of stations. Each game uses its own interface and playback mechanism. This repository contains **source code, not ready-to-use compiled releases**. The current implementations have not been verified in their respective games.

## Choose your platform

| Platform | Directory / guide | Audio method | Current state |
| --- | --- | --- | --- |
| DayZ | [dayz](dayz/README.md) | Windows MPC-HC Web Interface, port 13579 | Experimental source; PBO build/sign and client/server testing required |
| FiveM | [fivem](fivem/README.md) | NUI HTML5 audio, private to each player | Resource source; runtime testing required |
| Garry's Mod | [gmod](gmod/README.md) | Client-side `sound.PlayURL` | Lua addon source; runtime testing required |
| Minecraft Paper | [minecraft-paper](minecraft-paper/README.md) | OpenAudioMc per-player browser audio session | Java plugin source; build/API compatibility unverified |
| Minecraft Fabric / Forge / NeoForge | [minecraft-client-mods](minecraft-client-mods/README.md) | Would require client-side audio decoding | **Not implemented; no distributable JARs** |
| Other games / browser | [web-player](web-player/README.md) | HTML5 `<audio>` | Static prototype; CORS and codec support must be checked |

## Repository layout

```text

dayz/                      DayZ source, signing, mission/server guides
fivem/                     FiveM NUI radio resource
gmod/                      Garry's Mod client Lua addon
minecraft-paper/           Gradle/Paper plugin, optional OpenAudioMc
minecraft-client-mods/     Fabric/Forge/NeoForge development roadmap
web-player/                Standalone companion web player
README.md                  This overview
```

## How the station catalogue works

The response is expected to contain `{"stations":[...]}`. Stations are eligible when `enabled` is true, `name` is nonempty, and `stream_path` is a safe relative stream path. Example:

```json
{
  "stations": [{
    "id": "example-station-id",
    "name": "Example Station",
    "enabled": true,
    "country": "CA",
    "stream_path": "/stream/CA/example-station-id/station.mp3"
  }]
}
```

The stream URL becomes `https://aurora-kitsune.fumikoecho.ca/stream/CA/example-station-id/station.mp3`. A stream ending in `.mp3` is **not** a guarantee its actual audio codec is MP3; the playback engine must support the response's codec. Station lists may change and some streams may be geo-restricted or offline.

### Important behavioral differences

- **DayZ:** API catalogue is searchable independently of the legacy 48-slot protocol, and the selected vehicle station is mapped to a single dynamic slot. The original eight handheld frequencies are preserved. MPC-HC is a separate local Windows player; audio is not broadcast to occupants.
- **FiveM:** searches all loaded stations and listens through the player's own NUI audio player. This does *not* yet provide shared/positional xSound vehicle speakers.
- **Garry's Mod:** client plays audio locally through Source's audio implementation.
- **Paper:** needs the optional OpenAudioMc plugin and an OpenAudioMc browser audio session; a server-only plugin cannot inject arbitrary live HTTP audio into the normal Minecraft sound engine.
- **Fabric / Forge / NeoForge:** no functioning native client mod implementation is present. See the roadmap rather than attempting to build a nonexistent mod.
- **Web companion:** works only when browser access, stream format and CORS policy allow it.

## Quick start

1. Open the linked README for the platform you want.
2. Install/build only that platform's component and its required dependencies.
3. Verify the API is reachable from the game client or server (depending on platform).
4. Test playback and Stop/volume controls with one station before using a full catalogue.
5. Keep changes on a staging game server until the build and game-specific tests pass.

## Development and release status

This project is being developed in [PR #1](https://github.com/NekoSuneVR/gameMods-aura-kitsune-lists/pull/1); modifications on the feature branch are not automatically released on Steam Workshop or installed to game servers. No now-playing integration is implemented yet. Avoid shipping source as a compiled release or assuming every listed station is playable. Licenses and stream distribution rights remain the operator's responsibility.

The DayZ fork retains credit to the original [LootTeamRadio](https://github.com/offmonreal/LootTeamRadio) project; consult [dayz/LICENSE](dayz/LICENSE).
