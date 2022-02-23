#include <DeviceProvider.h>

#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )

DeviceProvider deviceProvider; //global, single instance, of the class that provides OpenVR with all of your devices.

HMD_DLL_EXPORT

void* HmdDriverFactory(const char* interfaceName, int* returnCode)
{
	if (strcmp(interfaceName, vr::IServerTrackedDeviceProvider_Version) == 0)
	{
		return &deviceProvider;
	}

	if (returnCode)
	{
		*returnCode = vr::VRInitError_Init_InterfaceNotFound;
	}

	return NULL;
}