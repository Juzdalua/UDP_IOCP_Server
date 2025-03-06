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

#include "ProcessPacket.h"
#include "PacketInfo.h"

#pragma comment(lib, "ws2_32.lib")

constexpr int BUFFER_SIZE = 512;
constexpr int MAX_CLIENTS = 5;
constexpr int RECV_THREAD_SIZE = 5;

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
		for (int i = 0; i < RECV_THREAD_SIZE; ++i) {
			_threads.push_back(std::thread(&ServerCore::recvWorkerThread, this));
		}
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

		int result = WSARecvFrom(_udpSocket, &perIoData->wsaBuf, 1, &recvBytes, &flags, (sockaddr*)&perIoData->clientAddr, &perIoData->addrLen, &perIoData->overlapped, nullptr);
		if (result == SOCKET_ERROR) {
			int errorCode = WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				std::cerr << "WSARecvFrom failed. Error: " << WSAGetLastError() << std::endl;
			}
		}
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

			std::cout << '\n';
			std::cout << "Received packet from client: " << perIoData->clientAddr.sin_addr.s_addr << std::endl;

			// ���ŵ� ������ ó��
			static std::vector<unsigned char> recvBuffer;
			recvBuffer.insert(recvBuffer.end(), perIoData->buffer, perIoData->buffer + bytesTransferred);

			// ù 5����Ʈ ����� �м�
			while (recvBuffer.size() >= 5) {
				unsigned short sNetVersion = *reinterpret_cast<const unsigned short*>(&recvBuffer[0]);
				short sMask = *reinterpret_cast<const short*>(&recvBuffer[2]);
				unsigned char bSize = recvBuffer[4];

				std::cout << "bytesTransferred: " << bytesTransferred << '\n';
				std::cout << "Received Header - NetVersion: " << sNetVersion
					<< ", Mask: " << sMask
					<< ", bSize: " << (int)bSize << std::endl;

				// ���� ������ ũ�� ���: bSize - 5
				size_t dataSize = bSize - 5;

				// ��ü ��Ŷ�� ������ ���ŵǾ����� Ȯ��
				if (recvBuffer.size() < bSize) {
					std::cout << "Packet not complete yet. Waiting for more data." << std::endl;
					break; // ��Ŷ�� ������ ���ŵ��� ������, ���� ������ ������ ��ٸ�
				}


				// ��Ŷ ó��
				ProcessPacket::handlePacket(recvBuffer.data());

				// ó���� ��Ŷ ����
				recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + bSize); // ��ü ��Ŷ ����
			}

			// ���� ���� �غ�
			ZeroMemory(&perIoData->overlapped, sizeof(WSAOVERLAPPED));
			postRecv();
			delete perIoData;
		}
	}

	void sendWorkerThread() {
		// �۽� �۾��� ó���ϴ� ������ ������ �ۼ��� �� �ֽ��ϴ�.
		// �� ���������� ���ŵ� �����͸� ó���ϴ� ���� �켱�̹Ƿ� �� ���·� �ξ����ϴ�.
		while (true) {
			// �۽� ��� ��
		}
	}

	std::string _ip;
	int _port;
	HANDLE _iocp;
	SOCKET _udpSocket;
	std::vector<std::thread> _threads;
};