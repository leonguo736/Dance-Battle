-- print("Application.lua")

switch = require("switch")
-- dofile("request.lua")
-- dofile("server.lua")
dofile("ws.lua")
-- dofile("uart.lua")

DEBUG = true

stateMachine = switch()
    :case("c", function(command, ip)
        ws_connect(ip)
    end)
    :case("r", function(command)
        print("Resetting ESP...")
        node.restart()
    end)
    :case("q", function(command) 
        print("Closing UART")
        uart.on("data")
    end)
    :case("d", function(command)
        DEBUG = not DEBUG
        print("Debug: " .. tostring(DEBUG))
    end)
    :case("j", function(command, json)
        print("Sending JSON")
        ws:send(json)
    end)
    :default(function(command, ...)
        ws:send(command .. table.concat({...} or {}, " "))
    end)

uart.on("data", "\r", function(data)
    local command = string.sub(data, 1, 1)
    local args = {}
    local argc = 0
    for i in string.gmatch(string.sub(data, 2, -1), "[^ ]+") do
        table.insert(args, i)
        argc = argc + 1
    end

    if (DEBUG) then
        uart.write(0, "----- Echo -----\n");
        uart.write(0, "Command: " .. command .. "\n")
        
        uart.write(0, "Args: [" .. argc .. "]: ")
        if (argc > 0) then
            uart.write(0, table.concat(args, ","))
        end
        uart.write(0, "\n----- End -----\n")
    end

    if (command) then
        stateMachine(command, unpack(args))
    end
end, 0)

gpio.write(led, gpio.HIGH)
uart.write(0, "i\n");


