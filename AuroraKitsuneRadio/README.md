# AuroraKitsuneRadio — cross-game radio starter (v0.1.0)

Original implementations informed by platform capabilities in user-supplied Hive365 distributions, **not** repackaged Hive365 binaries, code, trademarks or services.

**Radio API:** https://aurora-kitsune.fumikoecho.ca/api/stations
**Playback URL:** https://aurora-kitsune.fumikoecho.ca + stream_path

Only enabled entries with valid relative /stream/ paths are offered. Search covers the entire returned catalogue, not a fixed 48-slot limit. Now-playing is not implemented yet.

## Garry's Mod (client-side playback)

Copy `gmod/lua/autorun/client/aurorakitsuneradio.lua` into `garrysmod/lua/autorun/client/`. Console commands:
- `aurora_radio` — browse/search stations and double-click to listen.
- `aurora_radio_stop` — stop playback.
- `aurora_radio_volume 30` — set volume (0–100).
- `aurora_radio_refresh` — reload the catalogue.

This uses GMod's built-in `sound.PlayURL` for 2D playback. Not all streams/codecs necessarily work with Source's audio backend.

## Minecraft Paper (directory and external listening links)

Requires Paper 1.20.6 and Java 21. Build `minecraft-paper/` with Gradle 8.7+ (`gradle build`), then place the resulting JAR in the server `plugins/` folder.

- `/radio` — list up to 20 stations.
- `/radio <name>` — search station names.
- `/radio play <number>` — clickable external stream URL.
- `/radio refresh` — operator-only refresh.

A server-only Paper plugin cannot make unmodified Minecraft Java clients decode HTTP radio streams. In-game playback requires a Fabric/NeoForge client addon.

## Browser companion

Serve `web-player/index.html` on a static host or open locally to search and play stations while gaming. Your API may need CORS to support browser clients.

## SourceMod / Counter-Strike / TF2

SourceMod audio support is **not implemented**. The supplied Hive365 archives contain compiled binaries rather than reusable source.

## Status and licensing

This is a starter project: not yet built or game-tested. Respect radio stream licensing and API usage limits. No proprietary Hive365 binaries or copied code are included.
