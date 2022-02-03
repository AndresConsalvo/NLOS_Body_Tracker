#include <DeviceProvider.h>

vr::EVRInitError DeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
	vr::EVRInitError initError = vr::InitServerDriverContext(pDriverContext);

	if (initError != vr::EVRInitError::VRInitError_None) {
		return initError;
	}

	vr::VRDriverLog()->Log("Initializing example controller");

	controllerDriver = new ControllerDriver();
	vr::VRServerDriverHost()->TrackedDeviceAdded("example_controller", vr::TrackedDeviceClass_Controller, controllerDriver);

	return vr::VRInitError_None;
}

void DeviceProvider::Cleanup() {
	delete controllerDriver;
	controllerDriver = NULL;
}

const char* const* DeviceProvider::GetInterfaceVersions() {
	return vr::k_InterfaceVersions;
}

void DeviceProvider::RunFrame() {
	controllerDriver->RunFrame();
}

bool DeviceProvider::ShouldBlockStandbyMode() {
	return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}