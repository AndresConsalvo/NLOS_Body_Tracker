#define _USE_MATH_DEFINES
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma pack(n)

#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "math_helpers.h"
#pragma comment(lib, "ws2_32.lib")
/*
int main() {
	char  textBuffer[] = "{\"type\": \"POSITION\", \"data\": {\"accel\": [";
	printf("%s%.2f%s%.2f%s%.2f%s%.4f%s%.4f%s%.4f%s%s%s", textBuffer, 0.05, ",", 0.05, ",", 0.05, "], \"gyro\": [", 0.05, ",", 0.05, ",", 0.05, "], \"id\": ", "5", "}}");
}
*/