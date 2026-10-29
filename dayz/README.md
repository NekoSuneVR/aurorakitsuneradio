# AuroraKitsuneRadio for DayZ

**Source status:** experimental, based on LootTeamRadio; not compiled or tested against DayZ in this PR. See [LICENSE](LICENSE).

## Requirements

- DayZ client and a server with the required mission integration.
- Windows computer running [MPC-HC](https://github.com/clsid2/mpc-hc/releases) with **View > Options > Player > Web Interface** enabled on port **13579**.
- Signed client PBO and server `.bikey` made from this fork.

Audio is played locally using MPC-HC, not streamed as spatial DayZ in-world audio.

## Build from source

1. Work from **this `DayzMOD/` directory** (after checking out the repository).
2. Install a suitable DayZ PBO packer (e.g. armake2) and DayZ Tools for signing.
3. Build and inspect the mod:
   ```sh
   armake2 build mod-src/AuroraKitsuneRadio AuroraKitsuneRadio.pbo
   armake2 inspect AuroraKitsuneRadio.pbo
   ```
4. Place the PBO in `Build/@AuroraKitsuneRadio/addons/`; use the template in `Build/@AuroraKitsuneRadio/` for release packaging.
5. Sign the PBO with DayZ Tools using a private key that stays **outside Git**; distribute its `.bisign` alongside the PBO and place the corresponding `.bikey` on the server.
6. Integrate the mission source before joining the server. Test game script logs and launcher keybindings on a separate client.

Detailed steps: [docs/BUILD.md](docs/BUILD.md) and [docs/SERVER.md](docs/SERVER.md). **Do not replace** an existing mission `init.c` wholesale with `server/mission-radio-snippet.c` — merge the required methods into it.

## How to use

- Hold an operational battery-powered Personal Radio and tune one of its supported physical FM frequencies, **or** enter a vehicle with its engine running.
- Press **F7** (remappable in DayZ Controls) to open the station menu while holding the radio or sitting in a vehicle.
- The vehicle menu can search names in the Aurora API catalogue and page through results; choose a station to listen through MPC-HC.
- `!radio car off` stops vehicle mode; `!radio 50` sets volume (0–100); `!radio help` shows controls. Other `!radio car` commands use server-side legacy presets and require mission integration.

## Station API and existing 48-slot protocol

The source fetches `https://aurora-kitsune.fumikoecho.ca/api/stations`. It keeps **all valid, enabled API entries** in a searchable client catalogue rather than truncating to 48. When you select an API station in a vehicle, the client temporarily maps that station onto **legacy virtual slot 48**, preserving the first eight handheld frequencies. Slot 48 is therefore unavailable as a distinct legacy preset while the API selection is active. An incoming server preset refresh may override the dynamic slot; reselect the desired API station.

If API fetching fails, existing bundled/server preset stations remain accessible. Server-provided presets take precedence for the legacy 48-slot configuration, but API catalogue search can still load. Note the current API is downloaded as a whole, and large payloads or international names remain untested in the DayZ client.

The Radio_Fungi coexistence logic checks whether a cassette is **inserted**, not whether it is actively playing; test both mods on a staging server.

## Known limitations

No build or client gameplay validation has been run; PBO layout/input registration, script syntax, RPC, huge-JSON handling, audio codec support, and UI behavior still require manual testing. GitHub commits do **not** update any Steam Workshop listing automatically. Publish only through your authorized Steam/DayZ Tools account.
