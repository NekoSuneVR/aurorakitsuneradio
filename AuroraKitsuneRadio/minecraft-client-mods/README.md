# Minecraft client mods: Fabric, Forge and NeoForge

**Status: planned, not implemented.** This folder currently contains **documentation only**. It has no Gradle build, no loader entrypoints and no client JARs. Do not use `gradle build` here expecting a mod.

## What is required for native in-game audio

The [Paper implementation](../minecraft-paper/README.md) uses OpenAudioMc's external web-audio session. To provide audio directly inside the Minecraft Java client without an external browser session, we need to develop an actual client-side mod for each loader:

| Loader | Build status | Intended installation |
| --- | --- | --- |
| Fabric | Not implemented | Eventually copy client JAR to `.minecraft/mods/` with matching Fabric Loader/API |
| Forge | Not implemented | Eventually copy a Forge-targeted client JAR to `.minecraft/mods/` |
| NeoForge | Not implemented | Eventually copy a NeoForge-targeted client JAR to `.minecraft/mods/` |

Version-specific builds will be necessary: do not install a Fabric JAR as a Forge or NeoForge JAR.

## Proposed development plan

1. Choose supported Minecraft and loader versions.
2. Implement common Java API catalogue and station-name search models.
3. Implement asynchronous HTTPS loading and a search/selection GUI.
4. Provide a streaming decoder for supported radio codecs (MP3 and AAC where licensed and technically available).
5. Integrate decoded PCM with the Minecraft/OpenAL sound system on the audio thread, with volume/stop controls.
6. Implement separate Fabric, Forge and NeoForge loader entrypoints and distribution/build targets.
7. Test each version in an actual Minecraft client; do not treat successful Gradle compilation as playback validation.

No native mod is being claimed as complete by this README. Until these exist, use [Paper + OpenAudioMc](../minecraft-paper/README.md) or the [browser companion player](../web-player/README.md).
