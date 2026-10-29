// Default catalogue mirrored from server/radio-stations.json. Used only until the server snapshot arrives.
class NekoWaveBundledStations
{
	static void LoadIfMissing()
	{
		if (LootRadioState.s_SlotIds && LootRadioState.s_SlotIds.Count() > 0) return;
		string data = "";
		data = data + "1|878|Loot.Team Radio|https://radio.loot.team/radio";
		data = data + ";2|895|TruckersFM|https://live.truckers.fm/";
		data = data + ";3|913|NekoSune Community Radio|https://radiocp.nekosunevr.co.uk/listen/nekosune_community_radio/radio.mp3";
		data = data + ";4|919|NCS Radio|https://radio.cliamp.stream/ncs/stream";
		data = data + ";5|946|NCS House|https://radio.cliamp.stream/ncs-house/stream";
		data = data + ";6|966|NCS Drum and Bass|https://radio.cliamp.stream/ncs-dnb/stream";
		data = data + ";7|997|NCS Chill|https://radio.cliamp.stream/ncs-chill/stream";
		data = data + ";8|1025|Capital FM UK|https://media-ice.musicradio.com/CapitalMP3";
		data = data + ";9|2009|SomaFM Groove Salad|https://ice1.somafm.com/groovesalad-128-mp3";
		data = data + ";10|2010|SomaFM Drone Zone|https://ice1.somafm.com/dronezone-128-mp3";
		data = data + ";11|2011|SomaFM Secret Agent|https://ice1.somafm.com/secretagent-128-mp3";
		data = data + ";12|2012|Radio Paradise Main Mix|https://stream.radioparadise.com/mp3-128";
		data = data + ";13|2013|UK - Heart 80s|https://media-ssl.musicradio.com/Heart80sMP3";
		data = data + ";14|2014|UK - Capital UK|https://media-ssl.musicradio.com/CapitalUK";
		data = data + ";15|2015|UK - BBC World Service|http://stream.live.vc.bbcmedia.co.uk/bbc_world_service";
		data = data + ";16|2016|UK - LBC UK|http://media-ice.musicradio.com/LBCUK";
		data = data + ";17|2017|Canada - CIUT Toronto|https://ice23.securenetsystems.net/CIUT";
		data = data + ";18|2018|Canada - Rock 101 Vancouver|https://live.leanstream.co/CFMIFM-MP3-2";
		data = data + ";19|2019|Canada - CFOX Vancouver|https://live.leanstream.co/CFOXFM-MP3";
		data = data + ";20|2020|Canada - CJAY 92 Calgary|https://18393.live.streamtheworld.com/CJAYFM.mp3";
		data = data + ";21|2021|Canada - 100.3 The Bear Edmonton|https://24963.live.streamtheworld.com/CFBRFM.mp3";
		data = data + ";22|2022|Canada - RdMix Classic Rock|https://cast1.torontocast.com:4610/stream";
		data = data + ";23|2023|USA - KEXP Seattle|https://kexp.streamguys1.com/kexp160.aac";
		data = data + ";24|2024|USA - WFMU Freeform|http://stream0.wfmu.org/freeform-128k.mp3";
		data = data + ";25|2025|USA - SomaFM Synphaera|https://ice5.somafm.com/synphaera-128-mp3";
		data = data + ";26|2026|USA - SomaFM Synphaera HiFi|https://ice5.somafm.com/synphaera-256-mp3";
		data = data + ";27|2027|Gaming - Simulator Radio|https://simulatorradio.stream/stream";
		data = data + ";28|2028|NCS Dubstep (cliamp)|https://radio.cliamp.stream/ncs-dubstep/stream";
		data = data + ";29|2029|NCS Trap (cliamp)|https://radio.cliamp.stream/ncs-trap/stream";
		data = data + ";30|2030|NCS Phonk (cliamp)|https://radio.cliamp.stream/ncs-phonk/stream";
		data = data + ";31|2031|NCS Pop (cliamp)|https://radio.cliamp.stream/ncs-pop/stream";
		data = data + ";32|2032|Lofi Hip Hop (cliamp)|https://radio.cliamp.stream/lofi/stream";
		data = data + ";33|2033|Synthwave (cliamp)|https://radio.cliamp.stream/synthwave/stream";
		data = data + ";34|2034|EDM (cliamp)|https://radio.cliamp.stream/edm/stream";
		data = data + ";35|2035|Chiptunes (cliamp)|https://radio.cliamp.stream/chiptune/stream";
		data = data + ";36|2036|Amiga Tracker (cliamp)|https://radio.cliamp.stream/amiga/stream";
		data = data + ";37|2037|SomaFM Cliqhop IDM|https://ice5.somafm.com/cliqhop-128-mp3";
		data = data + ";38|2038|Retro Lofi Chiptune Radio|https://a5.asurahosting.com:8150/radio.mp3";
		data = data + ";39|2039|Hardstyle - MixStream|https://radio.mixstream.nl/hardstyle.mp3";
		data = data + ";40|2040|Hardstyle - Platinum|https://relay.platinumradio.nl/listen/platinum_hardstyle/radio.mp3";
		data = data + ";41|2041|Hardstyle FM|https://hardstylefm.stream.laut.fm/hardstylefm";
		data = data + ";42|2042|Real Hardstyle Radio|https://stream.realhardstyle.nl/";
		data = data + ";43|2043|Frenchcore - Technolovers|https://0nlineradio.radioho.st/technolovers-frenchcore";
		data = data + ";44|2044|FrenchcoreRadio EU|https://212.84.160.3:8959/stream2";
		data = data + ";45|2045|Uptempo - FrenchcoreRadio EU|https://212.84.160.3:8959/stream";
		data = data + ";46|2046|FrenchcoreRadio NL|https://82.145.63.6:6178/stream";
		data = data + ";47|2047|Dubstep - SomaFM Dub Step Beyond|https://ice5.somafm.com/dubstep-128-mp3";
		data = data + ";48|2048|Dubstep - SomaFM High Quality|https://ice5.somafm.com/dubstep-256-mp3";
		LootRadioState.SetStationsFromString(data);
		Print("[NekoWave] Loaded bundled station list while waiting for server RPC.");
	}
}
