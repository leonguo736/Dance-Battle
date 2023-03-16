print("Application.lua")

dofile("request.lua")
dofile("server.lua")
dofile("ws.lua")
-- dofile("uart.lua")

gpio.write(led, gpio.HIGH)

uart.on("data", "\r", function(data)
    ws:send(data)
end, 0)



