-- AuroraKitsuneRadio: original client-side Garry's Mod radio addon.
-- Does not depend on Hive365. Local 2D playback via BASS (sound.PlayURL).
if not CLIENT then return end
local ORIGIN = "https://aurora-kitsune.fumikoecho.ca"
local API = ORIGIN .. "/api/stations"
local stations, filtered, channel, frame = {}, {}, nil, nil
local selected, generation = nil, 0
local vol = CreateClientConVar("aurora_radio_volume", "35", true, false, "Radio volume 0-100", 0, 100)
local function log(msg) chat.AddText(Color(115,220,180), "[Aurora Radio] ", color_white, tostring(msg)) end
local function safePath(path)
    return isstring(path) and path:sub(1, 8) == "/stream/" and not path:find("%.%.", 1, false)
        and not path:find("[?#\\]", 1) and not path:find("//", 1, true)
end
local function stop()
    generation = generation + 1
    if IsValid(channel) then channel:Stop() end
    channel = nil
    selected = nil
end
local function play(st)
    if not st or not safePath(st.stream_path) then return log("Invalid station path") end
    stop()
    selected = st
    local ticket = generation
    sound.PlayURL(ORIGIN .. st.stream_path, "noblock", function(ch, code, reason)
        if ticket ~= generation then if IsValid(ch) then ch:Stop() end return end
        if not IsValid(ch) then selected = nil return log("Stream error " .. tostring(code) .. ": " .. tostring(reason)) end
        channel = ch
        ch:SetVolume(math.Clamp(vol:GetInt(), 0, 100) / 100)
        ch:Play()
        log("Playing " .. st.name)
    end)
end
cvars.AddChangeCallback("aurora_radio_volume", function(_, _, new)
    if IsValid(channel) then channel:SetVolume(math.Clamp(tonumber(new) or 35, 0, 100) / 100) end
end, "aurora_volume")
local function fetch(callback)
    http.Fetch(API, function(body, _, _, status)
        if status ~= 200 then log("API returned HTTP " .. tostring(status)); return end
        local parsed = util.JSONToTable(body)
        if not istable(parsed) or not istable(parsed.stations) then log("Unexpected station response"); return end
        local result = {}
        for _, st in ipairs(parsed.stations) do
            if istable(st) and st.enabled == true and isstring(st.name) and st.name ~= "" and safePath(st.stream_path) then
                result[#result + 1] = { id = tostring(st.id or ""), name = st.name, stream_path = st.stream_path, country = tostring(st.country or ""), genre = tostring(st.genre or "") }
            end
        end
        stations = result
        log("Loaded " .. #stations .. " stations")
        if callback then callback() end
    end, function(err) log("Cannot reach radio API: " .. tostring(err)) end)
end
local function openMenu()
    if IsValid(frame) then frame:Remove() end
    frame = vgui.Create("DFrame")
    frame:SetSize(620, 570)
    frame:Center()
    frame:SetTitle("AuroraKitsuneRadio - Live Stations")
    frame:MakePopup()
    local search = vgui.Create("DTextEntry", frame)
    search:Dock(TOP); search:DockMargin(8, 8, 8, 4); search:SetTall(28)
    search:SetPlaceholderText("Search name, country or genre...")
    local status = vgui.Create("DLabel", frame)
    status:Dock(TOP); status:DockMargin(8, 0, 8, 4); status:SetTall(22)
    local list = vgui.Create("DListView", frame)
    list:Dock(FILL); list:DockMargin(8, 2, 8, 4)
    list:AddColumn("Station"); list:AddColumn("Country"):SetFixedWidth(65); list:AddColumn("Genre"):SetFixedWidth(135)
    -- Limit rendered rows per page, not the station catalogue.
    local page, pageSize = 1, 40
    local footer = vgui.Create("DPanel", frame)
    footer:Dock(BOTTOM); footer:SetTall(38); footer.Paint = nil
    local previous = vgui.Create("DButton", footer)
    previous:Dock(LEFT); previous:SetWide(90); previous:SetText("Previous")
    local nextPage = vgui.Create("DButton", footer)
    nextPage:Dock(LEFT); nextPage:SetWide(90); nextPage:SetText("Next")
    local stopButton = vgui.Create("DButton", footer)
    stopButton:Dock(RIGHT); stopButton:SetWide(90); stopButton:SetText("Stop")
    stopButton.DoClick = function() stop(); log("Stopped") end
    local volume = vgui.Create("DNumSlider", footer)
    volume:Dock(FILL); volume:SetText("Volume"); volume:SetMin(0); volume:SetMax(100); volume:SetDecimals(0)
    volume:SetValue(vol:GetInt())
    volume.OnValueChanged = function(_, value) RunConsoleCommand("aurora_radio_volume", tostring(math.floor(value))) end
    local function render()
        if not IsValid(list) then return end
        list:Clear()
        local total = #filtered
        local pages = math.max(1, math.ceil(total / pageSize))
        page = math.Clamp(page, 1, pages)
        for idx = (page - 1) * pageSize + 1, math.min(page * pageSize, total) do
            local st = filtered[idx]
            local line = list:AddLine(st.name, st.country, st.genre)
            line.station = st
        end
        status:SetText(total .. " results / " .. #stations .. " stations | Page " .. page .. "/" .. pages)
        previous:SetEnabled(page > 1); nextPage:SetEnabled(page < pages)
    end
    local function filter()
        filtered = {}
        local q = string.lower(string.Trim(search:GetValue() or ""))
        for _, st in ipairs(stations) do
            local text = string.lower(st.name .. " " .. st.country .. " " .. st.genre)
            if q == "" or string.find(text, q, 1, true) then filtered[#filtered + 1] = st end
        end
        page = 1; render()
    end
    search.OnValueChange = filter
    previous.DoClick = function() page = page - 1; render() end
    nextPage.DoClick = function() page = page + 1; render() end
    list.DoDoubleClick = function(_, _, line) if line and line.station then play(line.station) end end
    filter()
    if #stations == 0 then status:SetText("Loading stations...") end
    fetch(function() if IsValid(frame) then filter() end end)
end
concommand.Add("aurora_radio", openMenu)
concommand.Add("aurora_radio_stop", stop)
concommand.Add("aurora_radio_refresh", function() fetch() end)
hook.Add("ShutDown", "AuroraRadioStop", stop)
