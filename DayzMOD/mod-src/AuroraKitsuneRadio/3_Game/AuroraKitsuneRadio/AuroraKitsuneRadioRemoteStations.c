// Aurora Kitsune catalogue API. Requests are asynchronous; retain bundled stations on errors.
class AuroraKitsuneApiStation
{
 string id;
 string name;
 string country;
 string stream_path;
 bool enabled;
}
class AuroraKitsuneApiResponse
{
 ref array<ref AuroraKitsuneApiStation> stations;
 void AuroraKitsuneApiResponse() { stations = new array<ref AuroraKitsuneApiStation>(); }
}
// API response supports names and stream_path, with up to 48 playable legacy slots.
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
	static const string BASE_URL = "https://aurora-kitsune.fumikoecho.ca/";
	static const string STATIONS_PATH = "api/stations";
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
		Print("[AuroraKitsuneRadio] Requesting Aurora Kitsune station catalogue...");
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
		if (data.Length() < 40 || data.Length() > 10000000)
		{
			OnFailure("invalid response size");
			return;
		}

        AuroraKitsuneApiResponse cfg = new AuroraKitsuneApiResponse();
        string error;
        if (!JsonFileLoader<AuroraKitsuneApiResponse>.LoadData(data, cfg, error) || !cfg || !cfg.stations)
        {
            OnFailure("invalid API JSON " + error);
            return;
        }
        // Legacy playlist and chat command protocol still supports 48 slots.
        // Keep the eight original handheld frequencies; imported streams are vehicle-only.
        string snapshot = "";
        int included = 0;
        for (int i = 0; i < cfg.stations.Count() && included < 48; i++)
        {
            AuroraKitsuneApiStation st = cfg.stations[i];
            if (!st || !st.enabled || st.name == "" || st.stream_path == "") continue;
            if (st.name.Contains("|") || st.name.Contains(";") || st.stream_path.Contains("|") || st.stream_path.Contains(";")) continue;
            if (st.stream_path.IndexOf("/") != 0 || st.stream_path.IndexOf("//") == 0 || st.stream_path.Contains("..") || st.stream_path.Contains("?") || st.stream_path.Contains("#")) continue;
            int slot = included + 1;
            int freq = 2000 + slot;
            if (slot == 1) freq = 878;
            if (slot == 2) freq = 895;
            if (slot == 3) freq = 913;
            if (slot == 4) freq = 919;
            if (slot == 5) freq = 946;
            if (slot == 6) freq = 966;
            if (slot == 7) freq = 997;
            if (slot == 8) freq = 1025;
            if (included > 0) snapshot = snapshot + ";";
            snapshot = snapshot + slot.ToString() + "|" + freq.ToString() + "|" + st.name + "|https://aurora-kitsune.fumikoecho.ca" + st.stream_path;
            included++;
        }
        if (included < 8)
        {
            OnFailure("API returned fewer than eight playable stations");
            return;
        }
        LootRadioState.SetStationsFromString(snapshot);
        s_HasRemote = true;
        s_Revision++;
        Print("[AuroraKitsuneRadio] API catalogue loaded: " + included.ToString() + " playable stations (legacy maximum 48).");
	}
}
