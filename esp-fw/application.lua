print("Application.lua")

dofile("request.lua")
dofile("server.lua")
dofile("uart.lua")

gpio.write(led, gpio.HIGH)

local function test()
    print("test")
end

function non_local()
    print("non local")
end

function arg_test(...)
    print("arg_test")
    for k,v in pairs(arg) do
        print(k,v)
    end
end

function create(url, port) 
    local client = net.createConnection()
    client:on("receive", function(response) 
        print(response)
    end)
    client:connect(port, url)
    return client
end

function send(client, data)
    client:send(data)
end

function create_udp()
    local udpSocket = net.createUDPSocket()
    udpSocket:on("receive", function(s, data, port, ip)
        print(string.format("received '%s' from %s:%d", data, ip, port))
    end)

    return udpSocket
end

function udp_send(udp, data, ip, port)
    udp:send(port, ip, data)
end

function ping(url) 
    net.ping(url, function (b, ip, sq, tm) 
        if ip then 
            print(("%d bytes from %s, icmp_seq=%d time=%dms"):format(b, ip, sq, tm)) 
        else 
            print("Invalid IP address") 
        end 
    end)
end


