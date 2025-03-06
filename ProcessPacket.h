#pragma once
class ProcessPacket
{
public:
	static void handlePacket(const unsigned char* buffer);

public:
	// sMask = 1
	static void handleSteerPacket(const unsigned char* buffer);

	// sMask = 2
	static void handleCabinControlPacket(const unsigned char* buffer);

	// sMask = 3
	static void handleCabinSwitchPacket(const unsigned char* buffer);
};

