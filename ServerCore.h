#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

constexpr int BUFFER_SIZE = 512;
constexpr int MAX_CLIENTS = 5;

struct PerIoData {
	WSAOVERLAPPED overlapped = {};
	WSABUF wsaBuf;
	char buffer[BUFFER_SIZE];
	sockaddr_in clientAddr;
	int addrLen = sizeof(sockaddr_in);
};

class ServerCore {
public:
	ServerCore(std::string ip, int port) : _ip(ip), _port(port), _iocp(nullptr), _udpSocket(INVALID_SOCKET) {}

	bool init() {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cerr << "WSAStartup failed." << std::endl;
			return false;
		}

		_udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if (_udpSocket == INVALID_SOCKET) {
			std::cerr << "Socket creation failed." << std::endl;
			return false;
		}

		sockaddr_in serverAddr = {};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
		serverAddr.sin_port = htons(_port);

		if (bind(_udpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cerr << "Bind failed." << std::endl;
			return false;
		}

		_iocp = CreateIoCompletionPort((HANDLE)_udpSocket, nullptr, (ULONG_PTR)_udpSocket, 0);
		if (!_iocp) {
			std::cerr << "IOCP creation failed." << std::endl;
			return false;
		}

		postRecv();

		return true;
	}

	void runThread() {
		_threads.push_back(std::thread(&ServerCore::recvWorkerThread, this));
		_threads.push_back(std::thread(&ServerCore::sendWorkerThread, this));
	}

	void stopThread() {
		for (auto& t : _threads) {
			t.join();
		}
	}

	void startServer() {
		runThread();
	}

	void stopServer() {
		stopThread();

		if (_udpSocket != INVALID_SOCKET) {
			closesocket(_udpSocket);
		}

		if (_iocp) {
			CloseHandle(_iocp);
		}

		WSACleanup();
	}

	void send(const sockaddr_in& clientAddr, const char* data, int len) {
		auto* perIoData = new PerIoData;
		perIoData->wsaBuf.buf = perIoData->buffer;
		memcpy(perIoData->buffer, data, len);
		perIoData->wsaBuf.len = len;
		perIoData->clientAddr = clientAddr;

		DWORD bytesSent = 0;
		WSASendTo(_udpSocket, &perIoData->wsaBuf, 1, &bytesSent, 0, (sockaddr*)&perIoData->clientAddr, perIoData->addrLen, nullptr, nullptr);
		delete perIoData;
	}

private:
	void postRecv() {
		auto* perIoData = new PerIoData;
		perIoData->wsaBuf.buf = perIoData->buffer;
		perIoData->wsaBuf.len = BUFFER_SIZE;
		DWORD flags = 0;
		DWORD recvBytes = 0;

		WSARecvFrom(_udpSocket, &perIoData->wsaBuf, 1, &recvBytes, &flags, (sockaddr*)&perIoData->clientAddr, &perIoData->addrLen, &perIoData->overlapped, nullptr);
	}

	void recvWorkerThread() {
		DWORD bytesTransferred;
		ULONG_PTR completionKey;
		PerIoData* perIoData;

		while (true) {
			BOOL result = GetQueuedCompletionStatus(_iocp, &bytesTransferred, &completionKey, (LPOVERLAPPED*)&perIoData, INFINITE);
			if (!result || bytesTransferred == 0) {
				std::cerr << "Client disconnected or error occurred." << std::endl;
				delete perIoData;
				continue;
			}

			// 받은 데이터의 끝에 NULL을 추가하여 문자열로 처리
			if (bytesTransferred < BUFFER_SIZE) {
				perIoData->buffer[bytesTransferred] = '\0'; // 널 종료 문자 추가
			}
			else {
				perIoData->buffer[BUFFER_SIZE - 1] = '\0'; // 만약 크기가 버퍼를 초과하면 마지막에 NULL 추가
			}

			std::cout << "Received from client: " << perIoData->buffer << std::endl;

			// 받은 데이터를 에코로 송신 (예시)
			// send(perIoData->clientAddr, perIoData->buffer, bytesTransferred);

			ZeroMemory(&perIoData->overlapped, sizeof(WSAOVERLAPPED));
			postRecv();
			delete perIoData;
		}
	}


	void sendWorkerThread() {
		// 송신 작업을 처리하는 스레드 로직을 작성할 수 있습니다.
		// 이 예제에서는 수신된 데이터를 처리하는 것이 우선이므로 빈 상태로 두었습니다.
		while (true) {
			// 송신 대기 중
		}
	}

	std::string _ip;
	int _port;
	HANDLE _iocp;
	SOCKET _udpSocket;
	std::vector<std::thread> _threads;
};