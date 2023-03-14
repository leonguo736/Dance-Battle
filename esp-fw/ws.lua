dofile("uart.lua")

ws = websocket.createClient()
ws:connect("ws://192.168.137.23:8080")

ws:on("connection", function(ws)
    print("connected")
    ws:send("hello")

    uart.write(0, "start\n")
end)

ws:on("receive", function(_, msg, opcode)
    print("received", msg, opcode)

    uart.write(0, msg .. "\n")
end)

ws:on("close", function(_, status)
    print("connection closed", status)
    uart.write(0, "close\n")
end)