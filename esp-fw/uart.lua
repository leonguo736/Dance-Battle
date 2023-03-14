uart.on("data", "\r", function(data) 
    print("Receive from UART", data)
    uart.write(0, data)
end, 0)

