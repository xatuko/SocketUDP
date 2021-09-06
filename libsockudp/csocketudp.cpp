#include "csocketudp.hpp"

CSocketUDP::CSocketUDP(const std::string & ip, const int & port, const SocketType & sock_type) : ISocketUDP()
{
    m_sock_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_sock_addr.sin_port        = htons(port);
    m_sock_addr.sin_family      = AF_INET;
    m_sock_len                  = sizeof(m_sock_addr);
    m_sock_type                 = sock_type;
}

CSocketUDP::~CSocketUDP()
{
    closeSock();
}

int CSocketUDP::initClient()
{
    m_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_fd < 0)
        return error("Ошибка создания сокета.");

    print("Соединение установлено.");
    
    m_is_open = true;
    m_run     = true;

    m_thread = std::make_unique<std::thread>(&CSocketUDP::bufproc, this, m_fd); 

    return 1;
}

int CSocketUDP::initServer()
{
    m_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_fd < 0)
        return error("Ошибка создания сокета.");

    if (bind(m_fd, (sockaddr*)&m_sock_addr, m_sock_len) == -1)
        return error("Ошибка привязки сокета.");
    
    m_is_open = true;
    m_run     = true;
    
    m_thread = std::make_unique<std::thread>(&CSocketUDP::bufproc, this, m_fd); 
    return 1;
}

int CSocketUDP::init()
{
    return m_sock_type == SocketType::Client ? initClient() : initServer();
}

int CSocketUDP::readData(const int & sock, ByteArray & data)
{
    int size = recvfrom(sock, data.data(), data.size(), MSG_DONTWAIT, (sockaddr*)&m_sock_addr, &m_sock_len);
    if (size > 0)
        data.resize(size);
    else if (size < 0) return error("Ошибка чтения.");
    return size;
}

int CSocketUDP::writeData(const int & sock, const ByteArray & data)
{

    int size = sendto(sock, data.data(), data.size(), MSG_DONTWAIT, (sockaddr*)&m_sock_addr, m_sock_len);
    return size > 0 ? size : error("Ошибка записи.");
}

void CSocketUDP::sendData(const ByteArray & data)
{
    std::unique_lock<std::mutex> lock(m_s_buf_control);
    m_send_buffer.push(data);
    lock.unlock();
}

void CSocketUDP::recvData(ByteArray & data)
{
    data.clear();
    std::unique_lock<std::mutex> lock(m_r_buf_control);
    if (m_recv_buffer.size() > 0)
    {
        data = m_recv_buffer.front();
        m_recv_buffer.pop();
    }
    lock.unlock();
}

void CSocketUDP::bufproc(int sock)
{
    m_pfd = new pollfd[2];

    m_pfd[0].fd      = sock;
    m_pfd[0].events  = POLLIN;
    m_pfd[0].revents = 0;

    m_pfd[1].fd      = sock;
    m_pfd[1].events  = POLLOUT;
    m_pfd[1].revents = 0;

    while (m_run)
    {
        // std::unique_lock<std::mutex> lock1(m_s_buf_control);
        // if (m_send_buffer.size() > 0)
        // {
        //     ByteArray barray = m_send_buffer.front();
        //     if (writeData(sock, barray) > 0)
        //         m_send_buffer.pop();
        // }
        // lock1.unlock();

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // std::unique_lock<std::mutex> lock2(m_r_buf_control);
        // ByteArray barray(BUFFER_SIZE);
        // if (readData(sock, barray) > 0)
        //     m_recv_buffer.push(barray);
        // lock2.unlock();

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        int res = poll(m_pfd, 2, 1000);
        if (res > 0)
        {
            if (m_send_buffer.size() > 0 && m_pfd[1].revents & POLLOUT)
            {
                std::unique_lock<std::mutex> lock(m_s_buf_control);
                if (writeData(sock, m_send_buffer.front()) > 0)
                    m_send_buffer.pop();
                lock.unlock();
            }

            if (m_pfd[0].revents & POLLIN)
            {
                std::unique_lock<std::mutex> lock(m_r_buf_control);
                ByteArray barray(BUFFER_SIZE);
                if (readData(sock, barray) > 0)
                    m_recv_buffer.push(barray);
                lock.unlock();
            }

            m_pfd[0].revents = 0;
            m_pfd[1].revents = 0;
        }
        
        if (res < 0)
        {
            m_run = false;
            error("Ошибка пола.");
        }
    }
}

void CSocketUDP::closeSock()
{
    if (!m_is_open)
        return;
    
    m_is_open = false;
    m_run     = false;
    if (m_thread->joinable())
        m_thread->join();
    
    close(m_fd);
}

void CSocketUDP::print(const std::string & text)
{
    std::cout << "[CSocketUDP] " << text << std::endl;
}

int CSocketUDP::error(const std::string & text)
{
    print(text);
    print("Error: " + std::to_string(errno));
    return -1;
}
