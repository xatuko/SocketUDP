#include "libsockudp/csocketudp.hpp"

using namespace libsock;

int main(int, char**) {
    int type;
    std::cout << "1 - server\n2 - client\n> ";
    std::cin >> type;

    std::unique_ptr<ISocketUDP> m_sock;

    if (type == 1)
        m_sock = std::make_unique<CSocketUDP>("192.168.11.148", 6534, SocketType::Server);
    else if (type == 2)
        m_sock = std::make_unique<CSocketUDP>("192.168.11.148", 6534, SocketType::Client);
    else return -1;
    if (m_sock->init() == -1)
        return -1;

    std::string input = "";
    while(input != "exit")
    {
        
            std::cout << "$ ";
            std::cin >> input;

            ByteArray arr(input.size()), arr2;
            for (int i = 0; i < input.size(); i++)
                arr[i] = input[i];
            if (arr.size() > 0)
                m_sock->sendData(arr);

            m_sock->recvData(arr2);
            if (!arr2.empty())
            {
                std::cout << "Message: ";
                for (int i =  0; i < arr2.size(); i++)
                    std::cout << (char)arr2[i] << " ";
                std::cout << std::endl;
            }
    }
    return 0;
}
