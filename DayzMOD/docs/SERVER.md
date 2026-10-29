# NekoWave Radio server integration / Встраивание на сервер

**Do not replace your entire `mpmissions/<mission>/init.c`.**
Back it up first. `server/mission-radio-snippet.c` is a **reference snippet**;
it is not a standalone mod script or ready-to-replace mission file. The
client mod alone cannot configure a server mission.

1. Inside your existing `CustomMission: MissionServer`, merge the fields and
   methods from `server/mission-radio-snippet.c`. Integrate the shown `OnEvent`
   and `OnInit` logic into existing overrides, calling `super` only once.
   Keep unrelated mission hooks (spawn equipment, character creation, etc.).
   Add the marked station RPC block to your existing `InvokeOnConnect` after
   `super.InvokeOnConnect`; merge into an existing override if one exists.
   Place `RTStation` and `RTStations` classes at file scope.
2. Replace `REPLACE_WITH_ADMIN_STEAM_ID` with the **server admin's own** Steam
   ID. Keep operator-specific IDs out of public source control. Chat commands
   resolve player identity from the server-side player list; duplicate names
   are rejected. Administrators must not grant admin commands to arbitrary users.
3. Put `server/radio-stations.json` into `$mission:radio-stations.json`.
   On the first successful start it is copied to
   `$profile:DayZRadio/radio-stations.json` — **later edit that profile copy**
   (the mission file is only an initial default). Server validates 8–48 station entries, preserving the first
   eight fixed handheld frequencies; only slots with both a name and URL play. The
   supplied presets include TruckersFM, NekoSune Community Radio, NCS-themed
   channels and Capital FM UK. Verify the streams from client PCs, including
   licensing, geo-availability and capacity, before enabling on a live server.
4. Install the matching signed client mod on server and distribute its `.bikey`
   into the server `keys/` directory. Copy [`server/meta.cpp`](../server/meta.cpp)
   to the root of the server's `@NekoWave/` directory, next to `mod.cpp`
   and `addons/`. Its `publishedid = 0` intentionally does not advertise the original project's Workshop item. Once this fork has its own published Workshop item, replace `0` with that new ID. Keep upstream author credit in the README.
   Do **not** place it inside the PBO or under `server/` on the DayZ host.
   With all required mods signed, use `verifySignatures=2`; do not disable
   verification for public servers.
5. Restart gracefully; check server RPT for `[RADIO] stations loaded: 8` and
   script errors. Test with a separate Windows client. Back up profile
   configuration and binaries before any release migration; rollback if
   script compilation fails. The example mod does **not** start eight Icecast
   processes: the seven empty slots remain intentionally silent.

### Running alongside another car-radio or cassette mod

NekoWave's car mode is a separate **opt-in player-side stream**. It does
not register tape items, change the other mod's tuning actions, or patch the
other mod's classes. This makes co-installation possible in principle, but it
is not proof of compatibility with any particular Workshop release.

The optional second system referenced by the community is:
https://steamcommunity.com/sharedfiles/filedetails/?id=3433858227

The other mod's scripts were not available for review, so **compatibility
has not been verified**. Install both on a staging server and check DayZ
script errors and action menus before deploying publicly.

For players: use `!radio car off` before using the cassette radio. Otherwise
the two unrelated audio players can be audible simultaneously. The livestream
is local to MPC-HC, and the cassette mod retains its own playback/controls.
Switching to a cassette does not automatically stop NekoWave, because
there is no verified API to detect cassette playback.

For administrators: keep both mods installed as separate signed client mods,
include their declared dependencies, and retain the NekoWave mission RPC
integration. Do **not** rename or overwrite the other mod's classes, car
actions, or JSON configuration. If duplicate input actions, class collisions,
or UI conflicts occur, inspect both mods' actual scripts and add a dedicated
integration adapter rather than replacing either mod's internals.

### Car radio controls (per player)

Inside a vehicle with its engine running, `!radio car fungi` disables the livestream before using Radio_Fungi cassette controls. `!radio car` selects preset 1,
`!radio car 1` through `!radio car 8` select any of the eight configured
stations, and `!radio car off` disables car radio playback. The car radio
uses RPC 777004 and requires both the updated mission integration and client
PBO. It does not broadcast spatially to other occupants; each player
controls their own local MPC-HC playback. Leaving the car or stopping the
engine pauses playback. The handheld radio continues to work independently.

### Admin chat commands

- `!radio set 2,https://example.org/live,Station name` — updates slot 2.
- `!radio clear 2` — clears slot 2.
- `!radio reload` — validates and broadcasts current profile configuration.

Each update broadcasts all eight slots; active clients rebuild local playlist
and reconnect the stream if tuned to the updated station. The server keeps a
backup at `$profile:DayZRadio/radio-stations.backup.json`.
Players use `!radio 50` / `!radio volume 50` and `!radio help`.

**Caveat:** standalone mission snippets cannot be compiled directly; test the
merged mission in DayZ on a non-production server before changing the live
mission. Obtain content rights and check stream capacity before advertising.


#### Radio_Fungi cassette coexistence

In vehicles with a `Cassette` attachment slot, NekoWave now checks
whether a cassette is inserted. If so, the external livestream automatically
pauses and Radio_Fungi can use its original controls. Removing the cassette
allows the selected livestream preset to resume if the car engine is running.
`!radio car fungi` still turns off the stream manually.

This is **attachment-based detection**, not genuine cassette play/pause
detection: a cassette left inserted but stopped will keep livestreams paused.
It does not alter or stop Radio_Fungi's sounds, and it is not a guarantee of
compatibility with all versions, boats, or third-party vehicle packs. Test the
vehicle slot name, both mods and playback on a staging DayZ server. The
livestream remains local MPC-HC audio, not shared vehicle sound.


### Extended station catalogue

The first eight entries keep their original fixed frequencies. Add consecutive `slotId` values from 9 through 48 for car-only streams; set `frequency` to `(2000 + slotId) / 10` (for example, slot 9 = `200.9`). The vehicle controls accept two-digit preset numbers, e.g. `!radio car 12`. The first eight still work on handheld radios. Edit the active `$profile:DayZRadio/radio-stations.json`, then use `!radio reload`. Do not assume that a new station stream is licensed or globally available. The GUI remains planned; it is not part of this change.
