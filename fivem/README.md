# AuroraKitsuneRadio for FiveM

A standalone FiveM GTA V radio **resource** inspired by the NUI design of the author's earlier [car-internet-radio-fivem](https://github.com/NekoSuneProjects/car-internet-radio-fivem/tree/fivemscript). This is new code, not a repackaged legacy resource.

## Installation (no compilation)

1. Copy **the contents of this `fivem/` folder** into `resources/[local]/auroraradio/` on your FiveM server. The `fxmanifest.lua` file must be at the root of that resource folder.
2. Add to `server.cfg`:
   ```cfg
   ensure auroraradio
   ```
3. Restart the FiveM server/resource.
4. Join and enter `/auroraradio`, or use **F7** (registered key mapping).
5. Type a station name, click a result, and adjust volume with the player controls. Use **Stop** to end playback.

Folder contents: `fxmanifest.lua`, `server.lua`, `client.lua` and `html/index.html`.

## How it works

- The server retrieves `https://aurora-kitsune.fumikoecho.ca/api/stations` on startup and periodically refreshes.
- Only valid enabled streams are indexed; clients request the catalogue and select a **station ID**.
- The server resolves that ID to an allowed stream URL and sends it back to the requesting player.
- The player's NUI `<audio>` element plays the selected stream privately.

To manually refresh the catalogue, use `auroraradio_reload` in the server console. For in-game administration, assign the ACE permission `auroraradio.admin` before using the command.

## Not included yet

This version **does not** use xSound, synchronize station choice between passengers, provide vehicle speaker position/attenuation, or display now-playing metadata. It is an individual 2D player. NUI codec support and external HTTPS stream restrictions can affect stations. **Not yet runtime-tested in FiveM**.

## Troubleshooting

- Menu doesn't open: verify `ensure auroraradio`, resource start errors, and F7 key conflicts.
- No stations: verify server outbound HTTPS/DNS connectivity and the API response.
- Station fails to play: inspect FiveM NUI DevTools; check station availability and HTML5 codec support.
- No sound: check NUI/audio mixer volume. This resource does not require the old xSound dependency.
