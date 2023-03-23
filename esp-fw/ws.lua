-- dofile("uart.lua")
-- function WS = {}

-- function WS:create(connectCallback, receiveCallback, closeCallback) 
--     local ws = {
--         ws = websocket.createClient()
--     }
--     setmetatable(ws, self)
--     self.__index = self;

--     ws:connect("ws://192.168.137.23:8080")

--     ws:on("connection", function(ws)
--         gpio.write(led, gpio.LOW)
--         uart.write(0, "connected\n")
--     end)

--     ws:on("receive", function(_, msg, opcode)    
--         gpio.write(led, gpio.HIGH)
--         uart.write(0, msg .. "\n")
--         gpio.write(led, gpio.LOW)
--     end)

--     ws:on("close", function(_, status)
--         uart.write(0, "close\n")
--         gpio.write(led, gpio.HIGH)
--     end)
    
--     return ws;
-- end
 
ws = websocket.createClient()

function ws_connect(ip) 
    if (DEBUG) then
        print("Connecting to IP: " .. ip)
    end
    ws:connect("ws://" .. ip)

    ws:on("connection", function(ws)
        gpio.write(led, gpio.LOW)
        uart.write(0, "r\n")
    end)

    ws:on("receive", function(_, msg, opcode)    
        gpio.write(led, gpio.HIGH)
        uart.write(0, msg .. "\n")
        gpio.write(led, gpio.LOW)
    end)

    ws:on("close", function(_, status)
        uart.write(0, "c\n")
        gpio.write(led, gpio.HIGH)
    end)
end
