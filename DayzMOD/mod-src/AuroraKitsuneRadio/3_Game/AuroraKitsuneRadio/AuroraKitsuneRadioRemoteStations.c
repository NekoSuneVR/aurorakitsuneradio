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
    static ref array<ref AuroraKitsuneApiStation> s_Catalogue;
    static int s_SelectedIndex = -1;

    static int Count() { if (!s_Catalogue) return 0; return s_Catalogue.Count(); }
    static AuroraKitsuneApiStation GetAt(int i) { if (!s_Catalogue || i < 0 || i >= s_Catalogue.Count()) return null; return s_Catalogue[i]; }

    // Use the 48th legacy virtual preset as a single dynamic playback slot.
    // No 2,893-entry MPC-HC playlist or server RPC required.
    static bool Tune(int index)
    {
        AuroraKitsuneApiStation chosen = GetAt(index);
        if (!chosen || !chosen.enabled) return false;
        if (!LootRadioState.s_SlotIds || LootRadioState.s_SlotIds.Count() < 8) return false;
        string snapshot = "";
        for (int slot = 1; slot <= 48; slot++)
        {
            int freq = 2000 + slot;
            string name = "-";
            string url = "-";
            if (slot <= 8)
            {
                if (slot == 1) freq = 878;
                if (slot == 2) freq = 895;
                if (slot == 3) freq = 913;
                if (slot == 4) freq = 919;
                if (slot == 5) freq = 946;
                if (slot == 6) freq = 966;
                if (slot == 7) freq = 997;
                if (slot == 8) freq = 1025;
            }
            if (LootRadioState.s_SlotIds)
            {
                for (int k = 0; k < LootRadioState.s_SlotIds.Count(); k++)
                {
                    int existingFreq = LootRadioState.s_SlotIds.GetKey(k);
                    if (LootRadioState.s_SlotIds.Get(existingFreq) != slot) continue;
                    freq = existingFreq;
                    if (LootRadioState.s_Names && LootRadioState.s_Names.Contains(existingFreq)) name = LootRadioState.s_Names.Get(existingFreq);
                    if (LootRadioState.s_Stations && LootRadioState.s_Stations.Contains(existingFreq)) url = LootRadioState.s_Stations.Get(existingFreq);
                    break;
                }
            }
            if (slot == 48)
            {
                freq = 2048;
                name = chosen.name;
                url = BASE_URL.Substring(0, BASE_URL.Length() - 1) + chosen.stream_path;
            }
            if (slot > 1) snapshot = snapshot + ";";
            snapshot = snapshot + slot.ToString() + "|" + freq.ToString() + "|" + name + "|" + url;
        }
        LootRadioState.SetStationsFromString(snapshot);
        LootRadioState.SetCarStation("48");
        s_SelectedIndex = index;
        s_Revision++;
        return true;
    }

	static void MarkServerAuthoritative()
	{
		s_ServerAuthoritative = true;
	}

	static void Fetch(bool force = false)
	{
		if (s_Pending) return;
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

        ref array<ref AuroraKitsuneApiStation> accepted = new array<ref AuroraKitsuneApiStation>();
        for (int i = 0; i < cfg.stations.Count(); i++)
        {
            AuroraKitsuneApiStation st = cfg.stations[i];
            if (!st || !st.enabled || st.name == "" || st.stream_path == "") continue;
            if (st.name.Contains("|") || st.name.Contains(";") || st.stream_path.Contains("|") || st.stream_path.Contains(";")) continue;
            if (st.stream_path.IndexOf("/") != 0 || st.stream_path.IndexOf("//") == 0 || st.stream_path.Contains("..") || st.stream_path.Contains("?") || st.stream_path.Contains("#")) continue;
            accepted.Insert(st);
        }
        if (accepted.Count() == 0) { OnFailure("no playable API stations"); return; }
        s_Catalogue = accepted;
        s_HasRemote = true;
        s_Revision++;
        Print("[AuroraKitsuneRadio] API catalogue indexed: " + accepted.Count().ToString() + " stations, searchable without the 48-slot cap.");
	}
}
