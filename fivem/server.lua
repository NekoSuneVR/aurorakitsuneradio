local ORIGIN = 'https://aurora-kitsune.fumikoecho.ca'
local stations = {}
local byId = {}
local function reload()
    PerformHttpRequest(ORIGIN .. '/api/stations', function(status, body)
        if status ~= 200 then print('[AuroraRadio] API HTTP '..tostring(status)); return end
        local data = json.decode(body or '{}')
        if type(data) ~= 'table' or type(data.stations) ~= 'table' then return end
        local nextList, nextIndex = {}, {}
        for _, st in ipairs(data.stations) do
            if st.enabled == true and type(st.id)=='string' and type(st.name)=='string' and type(st.stream_path)=='string'
                and st.stream_path:sub(1,8)=='/stream/' and not st.stream_path:find('..',1,true)
                and not st.stream_path:find('//',1,true) and not st.stream_path:find('[?#\\]') then
                local e = {id=st.id, name=st.name, country=st.country or '', genre=st.genre or '', url=ORIGIN..st.stream_path}
                nextList[#nextList+1]=e;nextIndex[st.id]=e
            end
        end
        stations=nextList;byId=nextIndex
        print('[AuroraRadio] '..#stations..' stations loaded')
    end, 'GET')
end
RegisterNetEvent('auroraradio:catalogue', function()
    TriggerClientEvent('auroraradio:catalogueResult', source, stations)
end)
RegisterNetEvent('auroraradio:select', function(id)
    local src=source
    if type(id)~='string' then return end
    local st=byId[id]
    if not st then return end
    -- Only send trusted URLs sourced from the server-side catalogue.
    TriggerClientEvent('auroraradio:play', src, st)
end)
RegisterNetEvent('auroraradio:stop', function()
    TriggerClientEvent('auroraradio:stopPlayback',source)
end)
RegisterCommand('auroraradio_reload', function(source)
    if source~=0 and not IsPlayerAceAllowed(source,'auroraradio.admin') then return end
    reload()
end,true)
CreateThread(function()
    reload()
    while true do
        Wait(30*60*1000)
        reload()
    end
end)
