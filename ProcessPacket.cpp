#include "ProcessPacket.h"
#include <vector>
#include <iostream>
#include "PacketInfo.h"

void ProcessPacket::handlePacket(const unsigned char* buffer, int bytesTransferred)
{
	// 헤더 파싱
	unsigned short sNetVersion = *reinterpret_cast<const unsigned short*>(buffer); // 2바이트
	short sMask = *reinterpret_cast<const short*>(buffer + sizeof(unsigned short)); // 2바이트
	unsigned char bSize = *(buffer + sizeof(unsigned short) + sizeof(short)); // 1바이트

	std::cout << "sNetVersion: " << sNetVersion << '\n';
	std::cout << "sMask: " << sMask << '\n';
	std::cout << "bSize: " << (int)bSize << '\n';

	// 데이터 버퍼 체크
	if (bSize <= 0) return;
	std::vector<unsigned char> dataBuffer((int)bSize - sizeof(PacketHeader));
	std::memcpy(dataBuffer.data(), buffer+5, (int)bSize - sizeof(PacketHeader));

	switch (sMask)
	{
	case 1: {
		handleSteerPacket(dataBuffer.data());
		break;
	}
	case 2: {
		handleCabinControlPacket(dataBuffer.data());
		break;
	}
	case 3: {
		handleCabinSwitchPacket(dataBuffer.data());
		break;
	}
	}
}

void ProcessPacket::handleSteerPacket(const unsigned char* buffer)
{
	unsigned __int32 status = *reinterpret_cast<const unsigned __int32*>(buffer);
	float steerAngle = *reinterpret_cast<const float*>(buffer + sizeof(unsigned __int32));
	float steerAngleRate = *reinterpret_cast<const float*>(buffer + sizeof(unsigned __int32) + sizeof(float));

	std::cout << " status: " << status;
	std::cout << " steerAngle: " << steerAngle;
	std::cout << " steerAngleRate: " << steerAngleRate;
}

void ProcessPacket::handleCabinControlPacket(const unsigned char* buffer)
{
	unsigned __int16 status = *reinterpret_cast<const unsigned __int16*>(buffer);  // 2바이트
	float carHeight = *reinterpret_cast<const float*>(buffer + sizeof(unsigned __int16));  // 4바이트
	float carWidth = *reinterpret_cast<const float*>(buffer + sizeof(unsigned __int16) + sizeof(float));  // 4바이트
	float seatWidth = *reinterpret_cast<const float*>(buffer + sizeof(unsigned __int16) + sizeof(float) * 2);  // 4바이트
}

void ProcessPacket::handleCabinSwitchPacket(const unsigned char* buffer)
{
	// 1바이트씩 읽어오기
	unsigned char GearTriger = buffer[0];  // 1: 아래로 두칸, 2: 아래로 한칸, 3: 중간, 4: 위로 한칸, 5: 위로 두칸
	unsigned char GearP = buffer[1];        // 2: Off, 1: On
	unsigned char Left_Paddle_Shift = buffer[2];  // 0: Off, 1: On
	unsigned char Right_Paddle_Shift = buffer[3]; // 0: Off, 1: On
	unsigned char Crs = buffer[4];          // 1: 속도 +, 2: 속도 -, 3: 앞차 거리, 4: 속도 버튼 클릭, 8: 크루즈
	unsigned char voice = buffer[5];        // 0: Off, 1: On
	unsigned char phone = buffer[6];        // 0: Off, 1: On
	unsigned char mode = buffer[7];         // 0: Off, 1: On
	unsigned char modeUp = buffer[8];       // 0: Off, 1: On
	unsigned char modeDown = buffer[9];     // 0: Off, 1: On
	unsigned char volumeMute = buffer[10];  // 0: Off, 1: On
	unsigned char volumeWheel = buffer[11]; // 0: Off, 1: Down, 2: Up
	unsigned char Menu = buffer[12];        // 0: Off, 1: On
	unsigned char MenuWheelbtn = buffer[13]; // 0: Off, 1: On
	unsigned char Menuwheel = buffer[14];   // 0: Off, 1: Down, 2: Up
	unsigned char bookmark = buffer[15];    // 0: Off, 1: On
	unsigned char Lamp_TrnSigLftSwSta = buffer[16];  // 0: Off, 1: On
	unsigned char Lamp_TrnSigRtSwSta = buffer[17];   // 0: Off, 1: On
	unsigned char Light = buffer[18];       // 0: Off, 1: Auto, 2: 라이트1, 3: 라이트2
	unsigned char Lamp_HdLmpHiSwSta1 = buffer[19];  // 0: Off, 1: On (push)
	unsigned char Lamp_HdLmpHiSwSta2 = buffer[20];  // 0: Off, 1: On (pull)
	unsigned char Wiper_FrWiperMist = buffer[21];   // 0: Off, 1: On
	unsigned char Wiper_FrWiperWshSwSta = buffer[22]; // 0: Off, 1: Auto, 2: Lo, 3: Hi
	unsigned char Wiper_FrWiperWshSwSta2 = buffer[23]; // 0~4 (Level)
	unsigned char Wiper_RrWiperWshSwSta = buffer[24]; // 0: Off, 1: Lo, 2: Hi
	unsigned char NGB = buffer[25];          // 0: Off, 1: On
	unsigned char DriveModeSw = buffer[26];  // 0: Off, 1: On
	unsigned char LeftN = buffer[27];        // 0: Off, 1: On
	unsigned char RightN = buffer[28];       // 0: Off, 1: On
	unsigned char HOD_Dir_Status = buffer[29];  // 0: 미감지, 1~10 (스티어링 감지)
	unsigned char FWasher = buffer[30];      // 0: Off, 1: On
	unsigned char Parking = buffer[31];      // 0: Off, 1: pull, 2: push
	unsigned char SeatBelt1 = buffer[32];    // 0: Off, 1: On
	unsigned char SeatBelt2 = buffer[33];    // 0: Off, 1: On
	unsigned char EMG = buffer[34];          // 0: Off, 1: On
	unsigned char Key = buffer[35];          // 0: Off, 1: On
	unsigned char Trunk = buffer[36];        // 0: Off, 1: On
	unsigned char VDC = buffer[37];          // 0: Off, 1: On
	unsigned char Booster = buffer[38];      // 0: Off, 1: On
	unsigned char Plus = buffer[39];         // 0: Off, 1: On
	unsigned char Right = buffer[40];        // 0: Off, 1: On
	unsigned char Minus = buffer[41];        // 0: Off, 1: On
	unsigned char Voice = buffer[42];        // 0: Off, 1: On
	unsigned char OK = buffer[43];           // 0: Off, 1: On
	unsigned char Left = buffer[44];         // 0: Off, 1: On
	unsigned char Phone = buffer[45];        // 0: Off, 1: On
	unsigned char PlusSet = buffer[46];      // 0: Off, 1: On
	unsigned char Distance = buffer[47];     // 0: Off, 1: On
	unsigned char MinusSet = buffer[48];     // 0: Off, 1: On
	unsigned char LFA = buffer[49];          // 0: Off, 1: On
	unsigned char SCC = buffer[50];          // 0: Off, 1: On
	unsigned char CC = buffer[51];           // 0: Off, 1: On
	unsigned char DriveMode = buffer[52];    // 0: Off, 1: On
	unsigned char LightHeight = buffer[53];  // 0~3 (Level)
	unsigned char ACCpedal = buffer[54];     // 0~255
	unsigned char Brakepedal = buffer[55];   // 0~255
	unsigned char bMask = buffer[56];        // (unsigned char)0xef
}
