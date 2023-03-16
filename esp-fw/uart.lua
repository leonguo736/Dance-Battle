count = 0

uart.on("data", "\r", function(data) 
    -- print("Receive from UART", data)
    if (string.find(data, "start")) then
        -- ws:send("start")
        gpio.write(led, gpio.LOW)
        uart.write(0, "start\n")
        gpio.write(led, gpio.HIGH)
    elseif (string.find(data, "increment")) then
        count = count + 1;
        gpio.write(led, gpio.LOW)
        uart.write(0, count .. "\n")
        gpio.write(led, gpio.HIGH)
    elseif (string.find(data, "decrement")) then
        count = count - 1;
        gpio.write(led, gpio.LOW)
        uart.write(0, count .. "\n")
        gpio.write(led, gpio.HIGH)
    elseif (string.find(data, "config")) then
        gpio.write(led, gpio.LOW)
        local baud, databits, parity, stopbits = uart.getconfig(0)
        uart.write(0,  baud .. " | " .. databits .. " | " .. parity .. " | " .. stopbits .. "\n")
        gpio.write(led, gpio.HIGH)
    elseif (string.find(data, "buffer")) then
        gpio.write(led, gpio.LOW)
        uart.write(0, uart.fifodepth(0, uart.DIR_RX) .. " | " .. uart.fifodepth(0, uart.DIR_TX) .. "\n")
        gpio.write(led, gpio.HIGH)
    elseif (string.find(data, "test")) then
        gpio.write(led, gpio.LOW)
        http.get("http://worldtimeapi.org/api/timezone/America/Vancouver", nil, function(code, data)
            if (code < 0) then
                uart.write(0, "HTTP request failed")
            else
                print(code, data)
                uart.write(0, code .. "")
                uart.write(0, data)
                uart.write(0, "\n")
            end
            gpio.write(led, gpio.HIGH)
        end)
    elseif (string.find(data, "ws")) then
        gpio.write(led, gpio.LOW)
        ws:send(data)
        gpio.write(led, gpio.HIGH)
    else 
        gpio.write(led, gpio.LOW)
        uart.write(0, data .. "\n")
        gpio.write(led, gpio.HIGH)
    end
end, 0)

