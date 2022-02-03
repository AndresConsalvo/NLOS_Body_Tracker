#include <ControllerDriver.h>

vr::EVRInitError ControllerDriver::Activate(uint32_t unObjectID) {
	driverID = unObjectID;

	vr::PropertyContainerHandle_t props = vr::VRProperties()->TrackedDeviceToPropertyContainer(driverID);

	vr::VRProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String, "{example}/input/controller_profile.json");
	vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_Treadmill);

	vr::VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, vr::EVRScalarType::VRScalarType_Absolute,
		vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send joystick commands to OpenVR with, in the Y direction (forward/backward)

	vr::VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &trackpadYHandle, vr::EVRScalarType::VRScalarType_Absolute,
		vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send trackpad commands to OpenVR with, in the Y direction

	vr::VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, vr::EVRScalarType::VRScalarType_Absolute,
		vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarType_Absolute? Take a look at the comments on EVRScalarType.

	vr::VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &trackpadXHandle, vr::EVRScalarType::VRScalarType_Absolute,
		vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarUnits_NormalizedTwoSided? Take a look at the comments on EVRScalarUnits.

	return vr::VRInitError_None;
}

vr::DriverPose_t ControllerDriver::GetPose() {
	vr::DriverPose_t pose = { 0 };
	pose.poseIsValid = false;
	pose.result = vr::TrackingResult_Calibrating_OutOfRange;
	pose.deviceIsConnected = true;

	vr::HmdQuaternion_t quat;
	quat.w = 1;
	quat.x = 0;
	quat.y = 0;
	quat.z = 0;

	pose.qWorldFromDriverRotation = quat;
	pose.qDriverFromHeadRotation = quat;

	return pose;
}

void ControllerDriver::RunFrame() {
	vr::VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.95f, 0);
	vr::VRDriverInput()->UpdateScalarComponent(trackpadYHandle, 0.95f, 0);
	vr::VRDriverInput()->UpdateScalarComponent(joystickXHandle, 0.0f, 0);
	vr::VRDriverInput()->UpdateScalarComponent(trackpadXHandle, 0.0f, 0);
}

void ControllerDriver::Deactivate() {
	driverID = vr::k_unTrackedDeviceIndexInvalid;
}

void* ControllerDriver::GetComponent(const char* pchComponentNameandVersion) {
	if (strcmp(vr::IVRDriverInput_Version, pchComponentNameandVersion) == 0) {
		return this;
	}

	return NULL;
}

void ControllerDriver::EnterStandby() {}

void ControllerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
	if (unResponseBufferSize >= 1) {
		pchResponseBuffer[0] = 0;
	}
}