#pragma once
#pragma pack(n)

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _USE_MATH_DEFINES


#include <chrono>
#include <Globals.h>
#include <math_helpers.h>
#include <thread>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define tracker_data 0
#define set_config 1
#define calibrate 2

#pragma pack(push, 1)
struct data_pkg {
	uint8_t header = 'T';
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float qw = 1.0;
	float qx = 0.0;
	float qy = 0.0;
	float qz = 0.0;
	uint8_t tracker_id = 0;
	uint8_t footer = 't';
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ping_pkg {
	uint8_t header = 'P';
	uint8_t msg = 45;
	uint8_t footer = 'p';
};
#pragma pack(pop)

// adapted from https://github.com/m9cd0n9ld/IMU-VR-Full-Body-Tracker/blob/main/steamvr_openvr_driver/driver_imuFBT/headers/UDP.h
class UDP {
public:
	void init();
	void deinit();

private:
	void start();
	void setValue(data_pkg* payload);

	SOCKET sock;
	sockaddr_in local;
	int localAddrSize = sizeof(local);

	const uint16_t serverPort = 4242;

	int bytes_read;
	int BufLen = 255;
	char RecvBuf[255];

	std::thread* receiveThread = nullptr;

	ping_pkg* server_response;
	data_pkg* payload;
	bool broadcast_en = true;
	bool server_connected = false;

	// Keeping track of time
	std::chrono::high_resolution_clock::time_point t_waist_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_lfoot_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rfoot_last = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_recv_end = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point last_received;
	std::chrono::high_resolution_clock::time_point current_time;
	double elapsed_time_s;

};


