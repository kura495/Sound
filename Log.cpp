#include<string>
void Log(const std::string& message) { 
	OutputDebugStringA(message.c_str());
}