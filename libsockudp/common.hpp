
#ifndef COMMON_HPP
#define COMMON_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <sys/poll.h>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <chrono>

using ByteArray = std::vector<uint8_t>;
const constexpr uint BUFFER_SIZE = 1400;
const constexpr uint SEND_DELAY  = 1400;
const constexpr uint RECV_DELAY  = 1400;

enum class SocketType { Client, Server };


#endif // COMMON_HPP