#include <iostream>
#include "Utils.h"
#include "ServerCore.h"
#include "PacketInfo.h"

int main()
{

    Utils::EnvInit();
    std::string SERVER_IP = Utils::getEnv("SERVER_IP");
    int SERVER_PORT = stoi(Utils::getEnv("SERVER_PORT"));

    ServerCore server(SERVER_IP, SERVER_PORT);
    if (!server.init()) {
        std::cerr << "Server initialization failed." << std::endl;
        return 1;
    }

    std::cout << "UDP IOCP Server running on";
    std::cout << " IP: " << SERVER_IP;
    std::cout << " PORT: " << SERVER_PORT;
    std::cout << '\n';

    server.startServer();

    server.stopServer();
    return 0;
}
