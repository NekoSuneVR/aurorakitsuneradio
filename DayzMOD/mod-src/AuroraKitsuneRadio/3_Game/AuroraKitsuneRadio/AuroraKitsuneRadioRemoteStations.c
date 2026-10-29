// Temporary public GitHub catalogue provider, replaceable by a future geo-radio API.
// Download client-side; never block the DayZ update thread.
class AuroraKitsuneRadioStationFetchCallback : RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		AuroraKitsuneRadioRemoteStations.OnSuccess(data);
	}
	override void OnError(int errorCode)
	{
		AuroraKitsuneRadioRemoteStations.OnFailure("HTTP callback error " + errorCode.ToString());
	}
	override void OnTimeout()
	{
		AuroraKitsuneRadioRemoteStations.OnFailure("request timed out");
	}
}
class AuroraKitsuneRadioRemoteStations
{
	static const string BASE_URL = "https://raw.githubusercontent.com/NekoSuneProjectsForks/NekoWave/feature/vehicle-radio-multi-stations/";
	static const string STATIONS_PATH = "server/radio-stations.json";
	static ref AuroraKitsuneRadioStationFetchCallback s_Callback;
	static bool s_Pending = false;
	static int s_LastRequestAt = -120000;
	static bool s_HasRemote = false;
	static bool s_ServerAuthoritative = false;
	static int s_Revision = 0;

	static void MarkServerAuthoritative()
	{
		s_ServerAuthoritative = true;
	}

	static void Fetch(bool force = false)
	{
		if (s_Pending || s_ServerAuthoritative) return;
		int now = GetGame().GetTime();
		if (!force && now - s_LastRequestAt < 60000) return;
		s_LastRequestAt = now;
		RestContext ctx = GetRestApi().GetRestContext(BASE_URL);
		if (!ctx)
		{
			Print("[AuroraKitsuneRadio] Remote catalogue REST context unavailable; using bundled stations.");
			return;
		}
		s_Pending = true;
		s_Callback = new AuroraKitsuneRadioStationFetchCallback();
		Print("[AuroraKitsuneRadio] Requesting public GitHub station catalogue...");
		ctx.GET(s_Callback, STATIONS_PATH);
	}

	static void OnFailure(string message)
	{
		s_Pending = false;
		s_Callback = null;
		Print("[AuroraKitsuneRadio] Catalogue request failed: " + message + "; retaining current stations.");
	}

	static void OnSuccess(string data)
	{
		s_Pending = false;
		s_Callback = null;
		if (s_ServerAuthoritative) return;
		if (data.Length() < 40 || data.Length() > 200000)
		{
			OnFailure("invalid response size");
			return;
		}
		LootRadioClientConfig cfg = new LootRadioClientConfig();
		string error;
		if (!JsonFileLoader<LootRadioClientConfig>.LoadData(data, cfg, error) || !cfg || !cfg.stations)
		{
			OnFailure("invalid JSON " + error);
			return;
		}
		int count = cfg.stations.Count();
		if (count < 8 || count > 48)
		{
			OnFailure("expected 8-48 stations, got " + count.ToString());
			return;
		}
		string snapshot = "";
		for (int i = 0; i < count; i++)
		{
			LootRadioClientStation st = cfg.stations[i];
			if (!st || st.slotId != i + 1 || st.name == "" || st.name.Contains("|") || st.name.Contains(";") || st.url.Contains("|") || st.url.Contains(";"))
			{
				OnFailure("invalid station entry at " + (i + 1).ToString());
				return;
			}
			int fk = Math.Round(st.frequency * 10.0);
			if (i >= 8 && fk != 2000 + st.slotId)
			{
				OnFailure("invalid extended station frequency");
				return;
			}
			if (i < 8 && fk <= 0)
			{
				OnFailure("invalid handheld frequency");
				return;
			}
			if (st.url.IndexOf("http://") != 0 && st.url.IndexOf("https://") != 0)
			{
				OnFailure("station URL must be HTTP(S)");
				return;
			}
			if (i > 0) snapshot = snapshot + ";";
			snapshot = snapshot + st.slotId.ToString() + "|" + fk.ToString() + "|" + st.name + "|" + st.url;
		}
		LootRadioState.SetStationsFromString(snapshot);
		s_HasRemote = true;
		s_Revision++;
		Print("[AuroraKitsuneRadio] GitHub catalogue loaded: " + count.ToString() + " stations.");
	}
}
