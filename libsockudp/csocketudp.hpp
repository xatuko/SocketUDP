#ifndef CSOCKETUDP_HPP
#define CSOCKETUDP_HPP

#include "isocketudp.hpp"

namespace libsock {

class CSocketUDP : public ISocketUDP
{
private: // members
    sockaddr_in m_sock_addr;   // Собственный адрес.
    sockaddr_in m_client_addr; // Адрес отправителя последний данных.

    socklen_t m_own_addr_len; // Размер структуры своего адреса.
    socklen_t m_cli_addr_len; // Размер структуры адреса отправителя.

    SocketType  m_sock_type; // Тип сокета (клиент/сервер).
    
    std::array<pollfd,2> m_pfd; // Пол для остлеживания состояний сокета.
    
    std::atomic_bool m_run {false}; // Флаг запуса потока чтения-записи.
    
    // Мьютекс для контроля буфера исходящих данных.
    std::mutex m_s_buf_control;
    
    // Мьютекс для контроля буфера входящих данных.
    std::mutex m_r_buf_control;
    
    std::queue<ByteArray> m_send_buffer;    // Буфер данных для отправки.
    std::queue<ByteArray> m_recv_buffer;    // Буфер входящих данных.

    std::unique_ptr<std::thread> m_thread;  // Поток чтения-записи данных.
    
    bool m_is_open;     // Флаг открытия сокета.
    bool m_output_flag; // Флаг вывода сообщений класса в консоль.
    int  m_fd;          // Дескриптор сокета.

private: // methods
    
    /**
     * @brief Вывод сообщения класса.
     */
    void print(const std::string & text);

    /**
     * @brief Вывод ошибки класса.
     */
    int error(const std::string & text);

    /**
     * @brief Инициализация клиента.
     */
    int initClient();

    /**
     * @brief Инициализация сервера.
     */
    int initServer();

    /**
     * @brief Прочитать данные из сокета.
     * @return Количество прочитанных байт (-1 в случае ошибки).
     */
    int readData(const int & sock, ByteArray & data);

    /**
     * @brief Записать данные в сокет.
     * @return Количество записанных байт (-1 в случае ошибки). 
     */
    int writeData(const int & sock, const ByteArray & data);

    /**
     * @brief Потоковый метод записи/чтения данных.
     */
    void bufproc(int sock);

public:  // methods
    CSocketUDP(const std::string & ip, const int & port,
               const SocketType & sock_type);
    ~CSocketUDP() override;

    /**
     * @brief Инициализация сокета.
     */
    int init() override;

    /**
     * @brief Отправить данные.
     */
    void sendData(const ByteArray & data) override;

    /**
     * @brief Получить данные.
     */
    void recvData(ByteArray & data) override;

    /**
     * @brief Закрыть сокет.
     */
    void closeSock() override;

    /**
     * @brief Включить/отключить вывод сообщений в консоль.
     * @param status false - отключить, true - включить.
     */
    void enableOutput(bool status) override
    { m_output_flag = status; }
};

} // namespace libsock

#endif // CSOCKETUDP_HPP