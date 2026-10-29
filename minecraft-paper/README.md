# AuroraKitsuneRadio for Minecraft Paper

**Server plugin source** for Paper **1.20.6 / Java 21**, fetching stations from the Aurora Kitsune API. Unlike a basic clickable URL directory, this version **attempts per-player streaming with OpenAudioMc**.

## Requirements

- Java Development Kit **21**.
- Gradle **8.7+** installed locally (the repository currently has no Gradle wrapper).
- A compatible **Paper 1.20.6** server.
- The **OpenAudioMc** server plugin, compatible with the selected server/API build.
- Players must connect to the OpenAudioMc **web audio session** (usually via its `/audio` flow). A Minecraft server-only plugin cannot emit arbitrary HTTP radio streams using native Minecraft audio packets.

## Build

From this `minecraft-paper/` folder run:

```sh
gradle build
```

The expected output, **if compilation succeeds**, is `build/libs/AuroraKitsuneRadio-0.1.0.jar`. Source declares an OpenAudioMc API dependency and has **not been compiled or verified** against its published Java API; you may need to correct API imports/dependency coordinates and call signatures for your OpenAudioMc version.

## Install and use

1. Install and configure OpenAudioMc on the Paper server first.
2. Put the built AuroraKitsuneRadio JAR into Paper's `plugins/` folder.
3. Restart, check the server console for dependency/enable errors.
4. Connect the player's OpenAudioMc audio session (see the plugin's own instructions).
5. In Minecraft, run `/radio` or `/radio <name>` to find stations; `/radio play 1` plays the first **displayed search result**, and `/radio stop` stops the player's stream.
6. Operators can run `/radio refresh` to request a fresh catalogue.

The command currently shows **up to 20 matching stations**, even though the underlying catalogue can hold thousands. This is not a paginated full-directory GUI yet.

## Troubleshooting / security

- No playback: confirm the OpenAudioMc session is connected and the chosen stream format is supported by its web player.
- Build error: reconcile the OpenAudioMc API version and repository before distribution; no successful JAR build is asserted here.
- No results: test API connectivity from the **Paper server**, not just the player's browser.
- The plugin limits playback to server-retrieved, validated relative stream paths. Do not accept arbitrary client URLs for audio playback.
- See [OPENAUDIOMC.md](OPENAUDIOMC.md) for integration detail.

**Fabric / Forge / NeoForge are not dependencies of this server-only plugin.** For native client mods, see [../minecraft-client-mods/README.md](../minecraft-client-mods/README.md).
