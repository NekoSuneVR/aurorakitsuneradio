# Aurora Kitsune FiveM Radio (experimental)
Original resource following the NUI approach used by the owner's earlier `car-internet-radio-fivem` (`fivemscript` branch). This version uses **client-local NUI HTML5 audio** rather than the legacy xSound dependency. Audio is 2D/private, NOT synchronized vehicle speaker audio.
Copy `fivem/` to `resources/[local]/auroraradio/`, add `ensure auroraradio` to server.cfg.
Use `/auroraradio` or F7, type a station name and click to listen; volume and stop are on the HTML5 audio player.
Server loads enabled stations from /api/stations and validates selection IDs; the client receives only server-approved URLs. Requires access to the external HTTPS streams through FiveM's CEF/NUI audio engine. No now-playing feature yet. Untested in FiveM.
