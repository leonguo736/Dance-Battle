uart.on("data", "\r", function(data) 
    print("Receive from UART", data)
    uart.write(0, data)

    if (string.find(data, "start")) then
        -- ws:send("start")
        uart.write(0, "start")
    elseif (string.find(data, "test")) then
        http.get("http://worldtimeapi.org/api/timezone/America/Vancouver", nil, function(code, data)
            if (code < 0) then
                print("HTTP request failed")
            else
                print(code, data)
            end
        end)
    end
end, 0)

