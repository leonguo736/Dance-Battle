dofile("uart.lua")

ws = websocket.createClient()
ws:connect("ws://192.168.137.23:8080")

ws:on("connection", function(ws)
    print("connected")
    ws:send("hello")

    gpio.write(led, gpio.LOW)
    uart.write(0, "start\n")
end)

ws:on("receive", function(_, msg, opcode)
    print("received", msg, opcode)
    
    gpio.write(led, gpio.HIGH)
    uart.write(0, msg .. "\n")
    gpio.write(led, gpio.LOW)
end)

ws:on("close", function(_, status)
    print("connection closed", status)
    uart.write(0, "close\n")
    
    gpio.write(led, gpio.HIGH)
end)