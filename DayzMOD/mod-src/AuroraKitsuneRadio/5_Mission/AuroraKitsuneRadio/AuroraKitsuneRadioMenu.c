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

	void Refresh()
	{
		if (!m_Root) return;
		int total = 0;
		if (LootRadioState.s_SlotIds) total = LootRadioState.s_SlotIds.Count();
		if (m_Status)
		{
			string kind = "Handheld: tune the physical radio to a matching FM frequency";
			if (m_InVehicle) kind = "Vehicle: select a stream (cassette has priority)";
			m_Status.SetText(kind + " | Page " + (m_Page + 1).ToString());
		}
		for (int i = 0; i < 8; i++)
		{
			ButtonWidget btn = m_Rows[i];
			if (!btn) continue;
			int id = m_Page * 8 + i + 1;
			if (id > total)
			{
				btn.Show(false);
				continue;
			}
			btn.Show(true);
			string name = "Unavailable";
			if (LootRadioState.s_SlotIds)
			{
				for (int j = 0; j < LootRadioState.s_SlotIds.Count(); j++)
				{
					int freq = LootRadioState.s_SlotIds.GetKey(j);
					if (LootRadioState.s_SlotIds.Get(freq) == id && LootRadioState.s_Names && LootRadioState.s_Names.Contains(freq))
					{
						name = LootRadioState.s_Names.Get(freq);
						break;
					}
				}
			}
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
				int id = m_Page * 8 + i + 1;
				if (m_InVehicle) LootRadioState.SetCarStation(id.ToString());
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
