// AuroraKitsuneRadio Radio — client mission hooks (Phase 3)
// OnUpdate: опрос Personal Radio в руках; RPC 777001 обрабатывается в DayZGame.

modded class MissionGameplay
{
	private int m_LootRadioTicks;
	private ref AuroraKitsuneRadioMenu m_AuroraKitsuneRadioMenu;
	private bool m_AuroraKitsuneRadioInputChecked;
	private int m_AnnouncedFrequency = -1;
	private bool m_WasWorking = false;
	private string m_AnnouncedName = "";
	private bool m_ConnectionChecked = false;
	private int m_LastConnectionCheck = 0;
	private int m_LastHelpTime = 0;
	private bool m_HasHelpShown = false;

	void MissionGameplay()
	{
		m_LootRadioTicks = 0;
		LootRadioState.BeginSession();
		AuroraKitsuneRadioRemoteStations.Fetch(true);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		// Poll before the radio's slower playback tick so the hotkey is responsive.
		UAInput menuInput = GetUApi().GetInputByName("UAAuroraKitsuneRadioToggle");
		if (!m_AuroraKitsuneRadioInputChecked)
		{
			m_AuroraKitsuneRadioInputChecked = true;
			if (!menuInput) Print("[AuroraKitsuneRadio] ERROR: UAAuroraKitsuneRadioToggle not registered. Check inputs.xml in client PBO and CfgMods.inputs.");
			else Print("[AuroraKitsuneRadio] UAAuroraKitsuneRadioToggle registered; default F7 or player custom binding.");
		}
		PlayerBase menuPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		bool inCar = false;
		bool holdingRadio = false;
		if (menuPlayer && menuPlayer.IsAlive())
		{
			inCar = menuPlayer.GetCommand_Vehicle() != null;
			EntityAI held = menuPlayer.GetHumanInventory().GetEntityInHands();
			holdingRadio = ItemTransmitter.Cast(held) != null;
		}
		if (m_AuroraKitsuneRadioMenu && m_AuroraKitsuneRadioMenu.IsOpen() && (!inCar && !holdingRadio))
		{
			m_AuroraKitsuneRadioMenu.Close();
			m_AuroraKitsuneRadioMenu = null;
		}
		// Never leave the radio cursor active under a native pause/inventory menu.
		if (m_AuroraKitsuneRadioMenu && m_AuroraKitsuneRadioMenu.IsOpen() && GetGame().GetUIManager().GetMenu())
		{
			m_AuroraKitsuneRadioMenu.Close();
			m_AuroraKitsuneRadioMenu = null;
		}
		// Escape/pause is an emergency exit even if the Close button loses focus.
		UAInput escapeInput = GetUApi().GetInputByName("UAMenu");
		if (m_AuroraKitsuneRadioMenu && m_AuroraKitsuneRadioMenu.IsOpen() && escapeInput && escapeInput.LocalPress())
		{
			m_AuroraKitsuneRadioMenu.Close();
			m_AuroraKitsuneRadioMenu = null;
		}
		if (menuInput && menuInput.LocalPress())
		{
			if (m_AuroraKitsuneRadioMenu && m_AuroraKitsuneRadioMenu.IsOpen())
			{
				m_AuroraKitsuneRadioMenu.Close();
				m_AuroraKitsuneRadioMenu = null;
			}
			else if ((inCar || holdingRadio) && !GetGame().GetUIManager().GetMenu())
				{
				m_AuroraKitsuneRadioMenu = new AuroraKitsuneRadioMenu(inCar);
				if (!m_AuroraKitsuneRadioMenu.IsOpen()) Print("[AuroraKitsuneRadio] ERROR: menu widget creation failed. Check AuroraKitsuneRadio/gui/layouts/aurorakitsuneradio.layout inside the client PBO.");
			}
		}

		if (m_AuroraKitsuneRadioMenu && m_AuroraKitsuneRadioMenu.IsOpen()) m_AuroraKitsuneRadioMenu.RefreshIfChanged();
		m_LootRadioTicks++;
		if (m_LootRadioTicks < 30)
			return;
		m_LootRadioTicks = 0;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.IsAlive())
		{
			LootRadioState.SetVolume0();
			m_WasWorking = false;
			m_ConnectionChecked = false;
			return;
		}

		// Car radio is private to each occupant and uses the same MPC-HC playlist.
		HumanCommandVehicle vehicleCommand = player.GetCommand_Vehicle();
		CarScript car;
		if (vehicleCommand) car = CarScript.Cast(vehicleCommand.GetTransport());
		if (car)
		{
			// Radio_Fungi car addon uses the "Cassette" vehicle attachment.
			// This is a conservative coexistence rule, not a playback-state API.
			// Prefer the in-game cassette whenever one is inserted, and never
			// change the other mod's actions, audio state, or attachment.
			EntityAI fungiCassette = car.FindAttachmentBySlotName("Cassette");
			if (fungiCassette)
			{
				LootRadioState.SetVolume0();
				m_WasWorking = false;
				return;
			}
			int carFrequencyKey = LootRadioState.GetCarFrequencyKey();
			if (carFrequencyKey > 0 && car.EngineIsOn())
			{
				int carNow = GetGame().GetTime();
				if (!m_ConnectionChecked || carNow - m_LastConnectionCheck >= 60000)
				{
					m_ConnectionChecked = true;
					m_LastConnectionCheck = carNow;
					LootRadioState.CheckPlayerConnection();
				}
				LootRadioState.PlayStation(carFrequencyKey);
				if (!m_WasWorking || carFrequencyKey != m_AnnouncedFrequency)
				{
					string stationName = "Channel available";
					if (LootRadioState.s_Names && LootRadioState.s_Names.Contains(carFrequencyKey)) stationName = LootRadioState.s_Names.Get(carFrequencyKey);
					NotificationSystem.AddNotificationExtended(5.0, "Car radio: preset " + LootRadioState.m_CarStationSlot.ToString(), stationName);
					m_AnnouncedFrequency = carFrequencyKey;
				}
				m_WasWorking = true;
			}
			else
			{
				LootRadioState.SetVolume0();
				m_WasWorking = false;
			}
			return;
		}
		EntityAI hands = player.GetHumanInventory().GetEntityInHands();
		ItemTransmitter tx = ItemTransmitter.Cast(hands);

		if (!tx)
		{
			if (!LootRadioState.m_DbgHands)
			{
				LootRadioState.m_DbgHands = true;
				Print("[LootRadio] no transmitter in hands");
			}
			LootRadioState.SetVolume0();
			m_WasWorking = false;
			m_ConnectionChecked = false;
			return;
		}

		if (!LootRadioState.m_DbgHands)
		{
			LootRadioState.m_DbgHands = true;
			Print("[LootRadio] transmitter in hands detected");
		}

		if (!tx.GetCompEM().IsWorking())
		{
			LootRadioState.SetVolume0();
			m_WasWorking = false;
			m_ConnectionChecked = false;
			return;
		}

		LootRadioState.ShowFirstUseHelp();

		// Only probe while the radio is on: once initially, then once per minute.
		int now = GetGame().GetTime();
		if (!m_ConnectionChecked || now - m_LastConnectionCheck >= 60000)
		{
			m_ConnectionChecked = true;
			m_LastConnectionCheck = now;
			LootRadioState.CheckPlayerConnection();
			if (!LootRadioState.m_PlayerReachable && (!m_HasHelpShown || now - m_LastHelpTime >= 60000))
			{
				m_HasHelpShown = true;
				m_LastHelpTime = now;
				bool helpRussian = false;
				GameOptions helpOptions = new GameOptions();
				ListOptionsAccess helpLanguage = ListOptionsAccess.Cast(helpOptions.GetOptionByType(OptionAccessType.AT_OPTIONS_LANGUAGE));
				if (helpLanguage)
				{
					string helpLangName;
					helpLanguage.GetItemText(helpLanguage.GetIndex(), helpLangName);
					helpLangName.ToLower();
					helpRussian = helpLangName.Contains("russian") || helpLangName.Contains("русск");
				}
				string helpTitle = "Radio: start MPC-HC";
				if (helpRussian) helpTitle = "Радио: запустите MPC-HC";
				int newlineCode = 10;
				string lineBreak = newlineCode.AsciiToString();
				string helpText = "https://github.com/clsid2/mpc-hc/releases" + lineBreak + "View > Options > Player > Web Interface" + lineBreak + "Listen on port [x]: 13579";
				NotificationSystem.AddNotificationExtended(15.0, helpTitle, helpText);
			}
		}

		int tunedIdx = tx.GetTunedFrequencyIndex();
		float tunedFreq = tx.GetTunedFrequency();
		int frequencyKey = Math.Round(tunedFreq * 10.0);
		if (LootRadioState.m_LastFrequencyIndex != tunedIdx)
		{
			LootRadioState.m_LastFrequencyIndex = tunedIdx;
			Print("[LootRadio] tuned freq: " + tunedFreq.ToString() + " key: " + frequencyKey.ToString());
		}

		if (frequencyKey > 0)
		{
			if (LootRadioState.m_PlayerReachable)
				LootRadioState.PlayStation(frequencyKey);

			// UI follows the physical radio, not MPC-HC playback commands.
			// Announce once on tuning, turning on again, or station-config change.
			if (LootRadioState.s_SlotIds && LootRadioState.s_SlotIds.Contains(frequencyKey))
			{
				string name = "";
				if (LootRadioState.s_Names && LootRadioState.s_Names.Contains(frequencyKey))
					name = LootRadioState.s_Names.Get(frequencyKey);
				if (!m_WasWorking || frequencyKey != m_AnnouncedFrequency || name != m_AnnouncedName)
				{
					bool russian = false;
					GameOptions options = new GameOptions();
					ListOptionsAccess language = ListOptionsAccess.Cast(options.GetOptionByType(OptionAccessType.AT_OPTIONS_LANGUAGE));
					if (language)
					{
						string langName;
						language.GetItemText(language.GetIndex(), langName);
						langName.ToLower();
						russian = langName.Contains("russian") || langName.Contains("русск");
					}
					string title = "FM: " + tunedFreq.ToString() + " MHz";
					string detail = name;
					if (russian) title = "Смена FM: " + tunedFreq.ToString();
					if (name == "")
					{
						if (russian) detail = "Канал свободен";
						else detail = "Channel available";
					}
					NotificationSystem.AddNotificationExtended(5.0, title, detail);
					m_AnnouncedFrequency = frequencyKey;
					m_AnnouncedName = name;
				}
			}
			m_WasWorking = true;
		}
		else
		{
			LootRadioState.SetVolume0();
			m_WasWorking = false;
		}
	}

	void ~MissionGameplay()
	{
		if (m_AuroraKitsuneRadioMenu && m_AuroraKitsuneRadioMenu.IsOpen()) m_AuroraKitsuneRadioMenu.Close();
		LootRadioState.StopSession();
	}
}
