#pragma once

#pragma pack(push,1)
struct PacketHeader
{
	unsigned short sNetVersion;
	short sMask;
	unsigned char bSize;
};
#pragma pack(pop)

struct CabinControlPacket {
	unsigned __int16 status;
	float carHeight;
	float carWidth;
	float seatWidth;
};

struct SteerPacket {
	unsigned __int32 status;
	float steerAngle;
	float steerAngleRate;
};

struct CabinSwitchPacket {
	unsigned char GearTriger;                // 1: 아래로 두칸, 2: 아래로 한칸, 3: 중간, 4: 위로 한칸, 5: 위로 두칸
	unsigned char GearP;                     // 2: Off, 1: On
	unsigned char Left_Paddle_Shift;         // 0: Off, 1: On
	unsigned char Right_Paddle_Shift;        // 0: Off, 1: On
	unsigned char Crs;                       // 1: 속도 +, 2: 속도 -, 3: 앞차 거리, 4: 속도 버튼 클릭, 8: 크루즈
	unsigned char voice;                     // 0: Off, 1: On
	unsigned char phone;                     // 0: Off, 1: On
	unsigned char mode;                      // 0: Off, 1: On
	unsigned char modeUp;                    // 0: Off, 1: On
	unsigned char modeDown;                  // 0: Off, 1: On
	unsigned char volumeMute;                // 0: Off, 1: On
	unsigned char volumeWheel;               // 0: Off, 1: Down, 2: Up
	unsigned char Menu;                      // 0: Off, 1: On
	unsigned char MenuWheelbtn;              // 0: Off, 1: On
	unsigned char Menuwheel;                 // 0: Off, 1: Down, 2: Up
	unsigned char bookmark;                  // 0: Off, 1: On
	unsigned char Lamp_TrnSigLftSwSta;      // 0: Off, 1: On
	unsigned char Lamp_TrnSigRtSwSta;       // 0: Off, 1: On
	unsigned char Light;                     // 0: Off, 1: Auto, 2: 라이트1, 3: 라이트2
	unsigned char Lamp_HdLmpHiSwSta1;       // 0: Off, 1: On (push)
	unsigned char Lamp_HdLmpHiSwSta2;       // 0: Off, 1: On (pull)
	unsigned char Wiper_FrWiperMist;        // 0: Off, 1: On
	unsigned char Wiper_FrWiperWshSwSta;    // 0: Off, 1: Auto, 2: Lo, 3: Hi
	unsigned char Wiper_FrWiperWshSwSta2;   // 0~4 (Level)
	unsigned char Wiper_RrWiperWshSwSta;    // 0: Off, 1: Lo, 2: Hi
	unsigned char NGB;                       // 0: Off, 1: On
	unsigned char DriveModeSw;               // 0: Off, 1: On
	unsigned char LeftN;                     // 0: Off, 1: On
	unsigned char RightN;                    // 0: Off, 1: On
	unsigned char HOD_Dir_Status;            // 0: 미감지, 1~10 (스티어링 감지)
	unsigned char FWasher;                   // 0: Off, 1: On
	unsigned char Parking;                   // 0: Off, 1: pull, 2: push
	unsigned char SeatBelt1;                 // 0: Off, 1: On
	unsigned char SeatBelt2;                 // 0: Off, 1: On
	unsigned char EMG;                       // 0: Off, 1: On
	unsigned char Key;                       // 0: Off, 1: On
	unsigned char Trunk;                     // 0: Off, 1: On
	unsigned char VDC;                       // 0: Off, 1: On
	unsigned char Booster;                   // 0: Off, 1: On
	unsigned char Plus;                      // 0: Off, 1: On
	unsigned char Right;                     // 0: Off, 1: On
	unsigned char Minus;                     // 0: Off, 1: On
	unsigned char Voice;                     // 0: Off, 1: On
	unsigned char OK;                        // 0: Off, 1: On
	unsigned char Left;                       // 0: Off, 1: On
	unsigned char Phone;                      // 0: Off, 1: On
	unsigned char PlusSet;                   // 0: Off, 1: On
	unsigned char Distance;                  // 0: Off, 1: On
	unsigned char MinusSet;                  // 0: Off, 1: On
	unsigned char LFA;                       // 0: Off, 1: On
	unsigned char SCC;                       // 0: Off, 1: On
	unsigned char CC;                        // 0: Off, 1: On
	unsigned char DriveMode;                 // 0: Off, 1: On
	unsigned char LightHeight;               // 0~3 (Level)
	unsigned char ACCpedal;                  // 0~255
	unsigned char Brakepedal;                // 0~255
	unsigned char bMask;                     // (unsigned char)0xef
};

