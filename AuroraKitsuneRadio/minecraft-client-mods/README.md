# Client mod targets: Fabric, Forge and NeoForge
The Paper + OpenAudioMc implementation is available now as the supported server-side playback route, although it relies on the OpenAudioMc audio session outside Minecraft's native audio engine.

For truly native game audio, a separate client-side streaming audio decoder and OpenAL playback implementation is required per Minecraft loader. Fabric, Forge and NeoForge build targets are **not implemented yet**; this directory is an explicit roadmap, not compiled mod source.

Recommended shared architecture:
- `common`: API data models, name search, selection state, HTTP fetch, playback abstraction.
- `fabric`: loader entrypoint / client UI / audio engine binding.
- `forge`: Forge entrypoint / client UI / audio binding.
- `neoforge`: NeoForge entrypoint / client UI / audio binding.
- Provide streamed MP3/AAC decoder support before promising all stations will play.
- Never block the Minecraft render thread or game tick while reading streams.
- Keep native client audio separate from OpenAudioMc browser fallback.
