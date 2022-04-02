from tracker import *
from math_helpers import quaternion

HMD = 0
CHEST = 1
WAIST = 2
LKNEE = 3
RKNEE = 4
LFOOT = 5
RFOOT = 6

class Skeleton:
    def __init__(self) -> None:
        self.HMD_to_Head = 0.0
        self.Head_to_Neck = 0.18
        self.Neck_to_Waist = 0.62
        self.Hip_Width = 0.3
        self.Hip_to_Knee = 0.45
        self.Knee_to_Foot = 0.45
        self.Head_to_Floor_len_m = 1.7
        self.Head_to_Waist_len_m = 0.8
        self.Hip_to_Foot_len_m = 0.5
        self.ankle_to_ground = 0.2

        self.chest_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.waist_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lknee_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rknee_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lfoot_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rfoot_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)

        self.chest_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.waist_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lknee_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rknee_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lfoot_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rfoot_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)

# -z = forward
# x = right
# y = up
def update_body(kinematics:Skeleton, trackers:dict, hmd_pos, hmd_quat, verbose=False):
    hmd_pos = quaternion(0, hmd_pos[0], hmd_pos[1], hmd_pos[2])
    hmd_quat = quaternion(hmd_quat[0], hmd_quat[1], hmd_quat[2], hmd_quat[3])

    chest_imu = trackers.get(CHEST).quat_from_imu
    waist_imu = trackers.get(WAIST).quat_from_imu
    lknee_imu = trackers.get(LKNEE).quat_from_imu
    rknee_imu = trackers.get(RKNEE).quat_from_imu
    lfoot_imu = trackers.get(LFOOT).quat_from_imu
    rfoot_imu = trackers.get(RFOOT).quat_from_imu

    chest = kinematics.chest_headingOffset * chest_imu * kinematics.chest_levelOffset
    waist = kinematics.waist_headingOffset * waist_imu * kinematics.waist_levelOffset
    lknee = kinematics.lknee_headingOffset * lknee_imu * kinematics.lknee_levelOffset
    rknee = kinematics.rknee_headingOffset * rknee_imu * kinematics.rknee_levelOffset
    lfoot = kinematics.lfoot_headingOffset * lfoot_imu * kinematics.lfoot_levelOffset
    rfoot = kinematics.rfoot_headingOffset * rfoot_imu * kinematics.rfoot_levelOffset
    
    neck_pos = hmd_pos + (hmd_quat * quaternion(0, 0, -kinematics.Head_to_Neck, 0) * hmd_quat.inverse())
    waist_pos = neck_pos + (chest.quat * quaternion(0, 0, -kinematics.Neck_to_Waist, 0) * chest.quat.inverse())

    lhip_pos = waist_pos + (waist.quat * quaternion(0, -kinematics.Hip_Width / 2, 0, 0) * waist.quat.inverse())
    rhip_pos = waist_pos + (waist.quat * quaternion(0, kinematics.Hip_Width / 2, 0, 0) * waist.quat.inverse())

    lknee_pos = lhip_pos + (lknee.quat * quaternion(0, 0, -kinematics.Hip_to_Knee, 0) * lknee.quat.inverse())
    rknee_pos = rhip_pos + (rknee.quat * quaternion(0, 0, -kinematics.Hip_to_Knee, 0) * rknee.quat.inverse())

    lfoot_pos = lknee_pos + (lfoot.quat * quaternion(0, 0, -kinematics.Knee_to_Foot, 0) * lfoot.quat.inverse())
    rfoot_pos = rknee_pos + (rfoot.quat * quaternion(0, 0, -kinematics.Knee_to_Foot, 0) * rfoot.quat.inverse())

    if (lfoot_pos.y < 0):
        lfoot_pos.y = 0
    
    if (rfoot_pos.y < 0):
        rfoot_pos.y = 0

    if (lhip_pos.y < 0):
        lhip_pos.y = 0
    
    if (rhip_pos.y < 0):
        rhip_pos.y = 0

    if (waist_pos.y < 0):
        waist_pos.y = 0


    waist.pos = waist_pos
    lknee.pos = lknee_pos
    rknee.pos = rknee_pos
    lfoot.pos = lfoot_pos
    rfoot.pos = rfoot_pos

    if (verbose == True):
        print("Neck pos:", vars(neck_pos))
        print("Waist pos:", vars(waist_pos))
        print("Left hip pos:", vars(lhip_pos))
        print("Right hip pos:", vars(rhip_pos))
        print("Left knee pos:", vars(lknee_pos))
        print("Right knee pos:", vars(rknee_pos))
        print("Left foot pos:", vars(lfoot_pos))
        print("Right foot pos:", vars(rfoot_pos))

def set_offsets(kinematics:Skeleton, trackers:dict, hmd_pos, hmd_quat):
    direction = quaternion(hmd_quat[0], 0.0, hmd_quat[2], 0.0)
    direction.normalize()

    chest_quat = trackers.get(CHEST).quat
    waist_quat = trackers.get(WAIST).quat
    lknee_quat = trackers.get(LKNEE).quat
    rknee_quat = trackers.get(RKNEE).quat
    lfoot_quat = trackers.get(LFOOT).quat
    rfoot_quat = trackers.get(RFOOT).quat

    kinematics.chest_headingOffset = getHeadingOffset(chest_quat, direction)
    kinematics.waist_headingOffset = getHeadingOffset(waist_quat, direction)
    kinematics.lknee_headingOffset = getHeadingOffset(lknee_quat, direction)
    kinematics.rknee_headingOffset = getHeadingOffset(rknee_quat, direction)
    kinematics.lfoot_headingOffset = getHeadingOffset(lfoot_quat, direction)
    kinematics.rfoot_headingOffset = getHeadingOffset(rfoot_quat, direction)

    kinematics.chest_levelOffset = getLevelOffset(chest_quat)
    kinematics.waist_levelOffset = getLevelOffset(waist_quat)
    kinematics.lknee_levelOffset = getLevelOffset(lknee_quat)
    kinematics.rknee_levelOffset = getLevelOffset(rknee_quat)
    kinematics.lfoot_levelOffset = getLevelOffset(lfoot_quat)
    kinematics.rfoot_levelOffset = getLevelOffset(rfoot_quat)



def getHeadingOffset(imu_quat : quaternion, direction : quaternion):
    heading = quaternion(imu_quat.w, 0.0, imu_quat.y, 0.0)
    heading.normalize()

    return heading.inverse() * direction

def getLevelOffset(imu_quat : quaternion):
    leveled_quat = quaternion(imu_quat.w, 0.0, imu_quat.y, 0.0)
    leveled_quat.normalize()

    return imu_quat.inverse() * leveled_quat
    
    




