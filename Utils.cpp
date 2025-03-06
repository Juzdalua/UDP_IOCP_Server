#include "Utils.h"
#include <iomanip>
#include <ctime>
#include <sstream>
#include <chrono>
#include <windows.h>


std::map<std::string, std::string> Utils::_envVariables;

bool Utils::EnvInit(const std::string& fileName)
{
	TCHAR szCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurrentDir);

	// TCHAR -> std::wstring 변환
	std::wstring wideCurrentDir(szCurrentDir);
	std::string currentDir(wideCurrentDir.begin(), wideCurrentDir.end());

	// 실행 파일 디렉터리와 같은 위치에 있는 .env 파일 경로 생성
	std::string fullFilePath = currentDir + "\\" + fileName;

	std::ifstream envFile(fullFilePath);
	if (!envFile.is_open()) return false;

	std::string line;
	while (std::getline(envFile, line))
	{
		if (line.empty() || line[0] == '#') continue;

		size_t delimiterPos = line.find('=');
		if (delimiterPos != std::string::npos)
		{
			std::string key = line.substr(0, delimiterPos);
			std::string value = line.substr(delimiterPos + 1);
			_envVariables[key] = value;
		}
	}

	envFile.close();
	return true;
}

std::string Utils::getEnv(const std::string& key)
{
	auto it = _envVariables.find(key);
	if (it != _envVariables.end()) return it->second;
	return "";
}

void Utils::LogError(const std::string& errorMsg, const std::string& functionName, std::string fileName)
{
	TCHAR szCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurrentDir);
	std::wstring wideCurrentDir(szCurrentDir);
	std::string currentDir(wideCurrentDir.begin(), wideCurrentDir.end());

	auto t = std::time(nullptr);
	std::tm tm = {};
	localtime_s(&tm, &t);
	std::ostringstream dateStream;
	dateStream << std::put_time(&tm, "%Y_%m_%d");

	std::string fullFilePath = currentDir + "\\log\\" + fileName + dateStream.str() + ".txt";
	std::ofstream logFile(fullFilePath, std::ios::app);
	if (!logFile.is_open()) return;

	std::ostringstream timeStream;
	timeStream << std::put_time(&tm, "%H:%M:%S");

	logFile << std::endl;
	logFile << "[" << timeStream.str() << "] -> [" << functionName << "]" << std::endl;
	logFile << errorMsg << std::endl;
	logFile.close();
}

void Utils::TestLogError(const std::string& msg)
{
	try
	{
		throw std::runtime_error(msg);
	}
	catch (const std::exception& e)
	{
		Utils::LogError(e.what(), "TestLogError");
	}
}

long long Utils::GetNowTimeMs()
{
	auto now = std::chrono::system_clock::now();
	auto nowTimeT = std::chrono::system_clock::to_time_t(now);
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()
	).count();
}

std::string Utils::GetNowTimeUtc9()
{
	auto now = std::chrono::system_clock::now();
	auto nowTimeT = std::chrono::system_clock::to_time_t(now);
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
	std::tm localTime;
	localtime_s(&localTime, &nowTimeT);
	std::ostringstream utc9TimeStream;
	utc9TimeStream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << millis;
	auto utc9Time = utc9TimeStream.str();

	return utc9Time;
}
