local open=false
local current=nil
local function close()
    open=false
    SetNuiFocus(false,false)
    SendNUIMessage({action='close'})
end
RegisterCommand('auroraradio',function()
    if open then close();return end
    open=true
    SetNuiFocus(true,true)
    SendNUIMessage({action='open'})
    TriggerServerEvent('auroraradio:catalogue')
end,false)
RegisterKeyMapping('auroraradio','Open Aurora Kitsune radio','keyboard','F7')
RegisterNetEvent('auroraradio:catalogueResult',function(stations)
    SendNUIMessage({action='stations', stations=stations})
end)
RegisterNetEvent('auroraradio:play',function(st)
    if type(st)~='table' then return end
    current=st
    SendNUIMessage({action='play',station=st})
end)
RegisterNetEvent('auroraradio:stopPlayback',function()
    current=nil
    SendNUIMessage({action='stop'})
end)
RegisterNUICallback('select',function(data,cb)
    if type(data.id)=='string' then TriggerServerEvent('auroraradio:select',data.id) end
    cb({ok=true})
end)
RegisterNUICallback('stop',function(_,cb)
    TriggerServerEvent('auroraradio:stop')
    cb({ok=true})
end)
RegisterNUICallback('close',function(_,cb) close();cb({ok=true}) end)
AddEventHandler('onResourceStop',function(name)
    if name==GetCurrentResourceName() then close() end
end)
