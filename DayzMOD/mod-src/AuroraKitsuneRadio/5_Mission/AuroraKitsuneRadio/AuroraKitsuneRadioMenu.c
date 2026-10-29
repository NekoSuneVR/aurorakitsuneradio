// Client-only paginated catalogue. No calls into Radio_Fungi's scripts.
class AuroraKitsuneRadioMenu : ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ref array<ButtonWidget> m_Rows;
	protected ButtonWidget m_Prev;
	protected ButtonWidget m_Next;
	protected ButtonWidget m_Off;
	protected ButtonWidget m_Close;
	protected TextWidget m_Status;
    protected EditBoxWidget m_Search;
    protected ref array<int> m_FilteredSlots;
    protected bool m_ApiMode;
	protected int m_Page;
	protected bool m_InVehicle;
	protected bool m_CursorOwned;
	protected int m_LastCatalogueRevision = -1;

	void AuroraKitsuneRadioMenu(bool inVehicle)
	{
		m_InVehicle = inVehicle;
		m_Page = 0;
		AuroraKitsuneRadioBundledStations.LoadIfMissing();
		AuroraKitsuneRadioRemoteStations.Fetch();
		m_Root = GetGame().GetWorkspace().CreateWidgets("AuroraKitsuneRadio/gui/layouts/aurorakitsuneradio.layout");
		if (!m_Root) return;
		m_Root.SetHandler(this);
		m_Status = TextWidget.Cast(m_Root.FindAnyWidget("NWStatus"));
        m_Search = EditBoxWidget.Cast(m_Root.FindAnyWidget("NWSearch"));
        if (m_Search) m_Search.SetHandler(this);
        m_FilteredSlots = new array<int>();
        m_ApiMode = inVehicle;
		m_Rows = new array<ButtonWidget>();
		for (int i = 1; i <= 8; i++)
			{
			ButtonWidget row = ButtonWidget.Cast(m_Root.FindAnyWidget("NW" + i.ToString()));
			if (row) row.SetHandler(this);
			m_Rows.Insert(row);
		}
		m_Prev = ButtonWidget.Cast(m_Root.FindAnyWidget("NWPrev"));
		m_Next = ButtonWidget.Cast(m_Root.FindAnyWidget("NWNext"));
		m_Off = ButtonWidget.Cast(m_Root.FindAnyWidget("NWOff"));
		m_Close = ButtonWidget.Cast(m_Root.FindAnyWidget("NWClose"));
		if (m_Prev) m_Prev.SetHandler(this);
		if (m_Next) m_Next.SetHandler(this);
		if (m_Off) m_Off.SetHandler(this);
		if (m_Close) m_Close.SetHandler(this);
		GetGame().GetUIManager().ShowCursor(true);
		// Do not change the global game-focus reference counter: it can trap
		// input after Alt-Tab or after another mod opens/closes a menu.
		m_CursorOwned = true;
		Refresh();
	}

	void RefreshIfChanged()
	{
		if (m_LastCatalogueRevision != AuroraKitsuneRadioRemoteStations.s_Revision)
		{
			m_LastCatalogueRevision = AuroraKitsuneRadioRemoteStations.s_Revision;
			Refresh();
		}
	}


    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        if (w == m_Search) { m_Page = 0; Refresh(); return true; }
        return false;
    }
    void Refresh()
    {
        if (!m_Root) return;
        m_FilteredSlots.Clear();
        string query = "";
        if (m_Search) { query = m_Search.GetText(); query.ToLower(); }
        if (m_ApiMode && AuroraKitsuneRadioRemoteStations.Count() > 0)
        {
            for (int apiIndex = 0; apiIndex < AuroraKitsuneRadioRemoteStations.Count(); apiIndex++)
            {
                AuroraKitsuneApiStation entry = AuroraKitsuneRadioRemoteStations.GetAt(apiIndex);
                if (!entry) continue;
                string displayName = entry.name;
                displayName.ToLower();
                if (query == "" || displayName.Contains(query)) m_FilteredSlots.Insert(apiIndex);
            }
        }
        else if (LootRadioState.s_SlotIds)
        {
            for (int slot = 1; slot <= 48; slot++)
            {
                for (int j = 0; j < LootRadioState.s_SlotIds.Count(); j++)
                {
                    int freq = LootRadioState.s_SlotIds.GetKey(j);
                    if (LootRadioState.s_SlotIds.Get(freq) != slot) continue;
                    string name = "";
                    if (LootRadioState.s_Names && LootRadioState.s_Names.Contains(freq)) name = LootRadioState.s_Names.Get(freq);
                    string comparison = name;
                    comparison.ToLower();
                    if (query == "" || comparison.Contains(query)) m_FilteredSlots.Insert(slot);
                    break;
                }
            }
        }
        int total = m_FilteredSlots.Count();
        if (m_Page * 8 >= total) m_Page = 0;
        if (m_Status)
        {
            string kind = "Handheld: tune the physical FM frequency";
            if (m_InVehicle) kind = "Vehicle: choose stream (cassette priority)";
            m_Status.SetText(kind + " | " + total.ToString() + " matches | Page " + (m_Page + 1).ToString());
        }
        for (int i = 0; i < 8; i++)
        {
            ButtonWidget btn = m_Rows[i];
            if (!btn) continue;
            int index = m_Page * 8 + i;
            if (index >= total) { btn.Show(false); continue; }
            int id = m_FilteredSlots[index];
            string name = "Unavailable";
            if (m_ApiMode && AuroraKitsuneRadioRemoteStations.Count() > 0)
            {
                AuroraKitsuneApiStation station = AuroraKitsuneRadioRemoteStations.GetAt(id);
                if (station) name = station.name;
                btn.Show(true);
                btn.SetText(name);
                continue;
            }
            for (int j = 0; j < LootRadioState.s_SlotIds.Count(); j++)
            {
                int freq = LootRadioState.s_SlotIds.GetKey(j);
                if (LootRadioState.s_SlotIds.Get(freq) == id && LootRadioState.s_Names && LootRadioState.s_Names.Contains(freq))
                {
                    name = LootRadioState.s_Names.Get(freq);
                    break;
                }
            }
            btn.Show(true);
            btn.SetText(id.ToString() + " - " + name);
        }
        if (m_Prev) m_Prev.Show(m_Page > 0);
        if (m_Next) m_Next.Show((m_Page + 1) * 8 < total);
        if (m_Off) m_Off.Show(m_InVehicle);
    }

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Close) { Close(); return true; }
		if (w == m_Prev) { if (m_Page > 0) m_Page--; Refresh(); return true; }
		if (w == m_Next) { m_Page++; Refresh(); return true; }
		if (w == m_Off) { LootRadioState.SetCarStation("off"); Close(); return true; }
		for (int i = 0; i < 8; i++)
		{
			if (w == m_Rows[i])
			{
				int index = m_Page * 8 + i;
                if (!m_FilteredSlots || index >= m_FilteredSlots.Count()) return true;
                int id = m_FilteredSlots[index];
				if (m_InVehicle && AuroraKitsuneRadioRemoteStations.Count() > 0) AuroraKitsuneRadioRemoteStations.Tune(id);
                else if (m_InVehicle) LootRadioState.SetCarStation(id.ToString());
				else NotificationSystem.AddNotificationExtended(5.0, "AuroraKitsuneRadio", "Handheld: tune to the matching physical frequency; stations 9+ require a vehicle.");
				Close();
				return true;
			}
		}
		return false;
	}

	void Close()
	{
		if (m_Root) { m_Root.Unlink(); m_Root = null; }
		if (m_CursorOwned)
		{
			m_CursorOwned = false;
			GetGame().GetUIManager().ShowCursor(false);
		}
	}
	bool IsOpen() { return m_Root != null; }
}
