from tracker import *
from math_helpers import *

HMD = 0
CHEST = 1
WAIST = 2
LKNEE = 3
RKNEE = 4
LFOOT = 5
RFOOT = 6

# https://personal.utdallas.edu/~sxb027100/dock/quaternion.html - for more info on quaternions. Very useful concept, hard as heck to visualize.
#                                                               - think of the x, y, z as the vector and w representing the clockwise rotation of the vector.
#                                                               - quaternions are represented as a vector [w, x, y, z].

# Spooky
class Skeleton:
    def __init__(self) -> None:
        # body measurements
        # defaults are for yours truly, Ruo :)
        self.HMD_to_Head = 0.13
        self.Head_to_Neck = 0.18
        self.Neck_to_Chest = 0.19
        self.Chest_to_Waist = 0.32
        self.Hip_Width = 0.22
        self.Hip_to_Knee = 0.38
        self.Knee_to_Foot = 0.38
        self.Head_to_Floor_len_m = 1.7
        self.Head_to_Waist_len_m = 0.8
        self.Hip_to_Foot_len_m = 0.5
        self.ankle_to_ground = 0.2

        # heading offsets
        self.chest_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.waist_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lknee_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rknee_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lfoot_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rfoot_headingOffset = quaternion(1.0, 0.0, 0.0, 0.0)

        # level offsets
        self.chest_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.waist_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lknee_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rknee_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.lfoot_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)
        self.rfoot_levelOffset = quaternion(1.0, 0.0, 0.0, 0.0)

        # sensor transforms
        self.driver_transform_euler = [90.0, 0.0, 0.0]
        self.chest_transform_euler = [90.0, 0.0, 270.0] 
        self.waist_transform_euler = [90.0, 0.0, 270.0] # for back facing
        self.lknee_transform_euler = [90.0, 0.0, 90.0] # for front facing
        self.rknee_transform_euler = [90.0, 0.0, 90.0]
        self.lfoot_transform_euler = [90.0, 0.0, 90.0]
        self.rfoot_transform_euler = [90.0, 0.0, 90.0]

        self.driver_transform = get_quat_from_euler(self.driver_transform_euler)
        self.chest_transform = get_quat_from_euler(self.chest_transform_euler)
        self.waist_transform = get_quat_from_euler(self.waist_transform_euler)
        self.lknee_transform = get_quat_from_euler(self.lknee_transform_euler)
        self.rknee_transform = get_quat_from_euler(self.rknee_transform_euler)
        self.lfoot_transform = get_quat_from_euler(self.lfoot_transform_euler)
        self.rfoot_transform = get_quat_from_euler(self.rfoot_transform_euler)

def convertFrame(imu, transform, driver):
    T_imu = driver * transform.inverse()
    imu_zero = T_imu.inverse() * imu
    return driver.inverse() * imu_zero * transform

# -z = forward
# x = right
# y = up
def update_body(kinematics:Skeleton, trackers:dict, hmd_pos, hmd_quat, verbose=False, offset_front=False, static_pos=False):

    if (offset_front == True):
        hmd_pos.z = hmd_pos.z - 1.0

    chest_tracker = trackers.get(CHEST)
    waist_tracker = trackers.get(WAIST)
    lknee_tracker = trackers.get(LKNEE)
    rknee_tracker = trackers.get(RKNEE)
    lfoot_tracker = trackers.get(LFOOT)
    rfoot_tracker = trackers.get(RFOOT)

    chest_imu = chest_tracker.quat_from_imu
    waist_imu = waist_tracker.quat_from_imu
    lknee_imu = lknee_tracker.quat_from_imu
    rknee_imu = rknee_tracker.quat_from_imu
    lfoot_imu = lfoot_tracker.quat_from_imu
    rfoot_imu = rfoot_tracker.quat_from_imu

    chest = kinematics.chest_headingOffset * convertFrame(chest_imu, kinematics.chest_transform, kinematics.driver_transform) * kinematics.chest_levelOffset
    waist = kinematics.waist_headingOffset * convertFrame(waist_imu, kinematics.waist_transform, kinematics.driver_transform) * kinematics.waist_levelOffset
    lknee = kinematics.lknee_headingOffset * convertFrame(lknee_imu, kinematics.lknee_transform, kinematics.driver_transform) * kinematics.lknee_levelOffset
    rknee = kinematics.rknee_headingOffset * convertFrame(rknee_imu, kinematics.rknee_transform, kinematics.driver_transform) * kinematics.rknee_levelOffset
    lfoot = kinematics.lfoot_headingOffset * convertFrame(lfoot_imu, kinematics.lfoot_transform, kinematics.driver_transform) * kinematics.lfoot_levelOffset
    rfoot = kinematics.rfoot_headingOffset * convertFrame(rfoot_imu, kinematics.rfoot_transform, kinematics.driver_transform) * kinematics.rfoot_levelOffset

    #chest = kinematics.chest_headingOffset * chest_imu * kinematics.chest_levelOffset
    #waist = kinematics.waist_headingOffset * waist_imu * kinematics.waist_levelOffset
    #lknee = kinematics.lknee_headingOffset * lknee_imu * kinematics.lknee_levelOffset
    #rknee = kinematics.rknee_headingOffset * rknee_imu * kinematics.rknee_levelOffset
    #lfoot = kinematics.lfoot_headingOffset * lfoot_imu * kinematics.lfoot_levelOffset
    #rfoot = kinematics.rfoot_headingOffset * rfoot_imu * kinematics.rfoot_levelOffset

    chest_tracker.quat = chest
    waist_tracker.quat = waist
    lknee_tracker.quat = lknee
    rknee_tracker.quat = rknee
    lfoot_tracker.quat = lfoot
    rfoot_tracker.quat = rfoot
    
    # forward kinematics
    neck_pos = hmd_pos + (hmd_quat * quaternion(0, 0, -kinematics.Head_to_Neck, kinematics.HMD_to_Head) * hmd_quat.inverse())
    chest_pos = neck_pos + (chest * quaternion(0, 0, -kinematics.Neck_to_Chest, 0) * chest.inverse())
    waist_pos = chest_pos + (waist * quaternion(0, 0, -kinematics.Chest_to_Waist, 0) * waist.inverse())

    # pseudo-joint tracking; these aren't actually represented by trackers (maybe could remove them to save on calculations?)
    lhip_pos = waist_pos + (waist * quaternion(0, -kinematics.Hip_Width / 2, 0, 0) * waist.inverse())
    rhip_pos = waist_pos + (waist * quaternion(0, kinematics.Hip_Width / 2, 0, 0) * waist.inverse())

    lknee_pos = lhip_pos + (lknee * quaternion(0, 0, -kinematics.Hip_to_Knee, 0) * lknee.inverse())
    rknee_pos = rhip_pos + (rknee * quaternion(0, 0, -kinematics.Hip_to_Knee, 0) * rknee.inverse())

    lfoot_pos = lknee_pos + (lfoot * quaternion(0, 0, -kinematics.Knee_to_Foot, 0) * lfoot.inverse())
    rfoot_pos = rknee_pos + (rfoot * quaternion(0, 0, -kinematics.Knee_to_Foot, 0) * rfoot.inverse())

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



    #waist_tracker.pos = waist_pos
    chest_tracker.pos = chest_pos
    waist_tracker.pos = waist_pos
    lknee_tracker.pos = lknee_pos
    rknee_tracker.pos = rknee_pos
    lfoot_tracker.pos = lfoot_pos
    rfoot_tracker.pos = rfoot_pos



    if (static_pos == True):
        chest_tracker.pos = quaternion(0.0, 0.0, 1.8, -1.0)
        waist_tracker.pos = quaternion(0.0, 0.0, 1.5, -1.0)
        lknee_tracker.pos = quaternion(0.0, 0.0, 1.0, -1.0)
        rknee_tracker.pos = quaternion(0.0, 0.0, 1.0, -1.0)
        lfoot_tracker.pos = quaternion(0.0, 0.0, 0.5, -1.0)
        rfoot_tracker.pos = quaternion(0.0, 0.0, 0.5, -1.0)


    if (verbose == True):
        print("Neck pos:", vars(neck_pos))
        print("Chest pos:", vars(chest_pos))
        print("Waist pos:", vars(waist_pos))
        print("Left hip pos:", vars(lhip_pos))
        print("Right hip pos:", vars(rhip_pos))
        print("Left knee pos:", vars(lknee_pos))
        print("Right knee pos:", vars(rknee_pos))
        print("Left foot pos:", vars(lfoot_pos))
        print("Right foot pos:", vars(rfoot_pos))

        print("chest quat:", vars(chest))
        print("Waist quat:", vars(waist))
        print("Left knee quat:", vars(lknee))
        print("Right knee quat:", vars(rknee))
        print("Left foot quat:", vars(lfoot))
        print("Right foot quat:", vars(rfoot))




def set_offsets(kinematics:Skeleton, trackers:dict, hmd_quat, verbose=False):
    direction = hmd_quat
    direction.normalize()

    chest_quat = convertFrame(trackers.get(CHEST).quat_from_imu, kinematics.chest_transform, kinematics.driver_transform)
    waist_quat = convertFrame(trackers.get(WAIST).quat_from_imu, kinematics.waist_transform, kinematics.driver_transform)
    lknee_quat = convertFrame(trackers.get(LKNEE).quat_from_imu, kinematics.lknee_transform, kinematics.driver_transform)
    rknee_quat = convertFrame(trackers.get(RKNEE).quat_from_imu, kinematics.rknee_transform, kinematics.driver_transform)
    lfoot_quat = convertFrame(trackers.get(LFOOT).quat_from_imu, kinematics.lfoot_transform, kinematics.driver_transform)
    rfoot_quat = convertFrame(trackers.get(RFOOT).quat_from_imu, kinematics.rfoot_transform, kinematics.driver_transform)

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

    if (verbose == True):
        print(vars(chest_quat))
        print(vars(kinematics.chest_headingOffset))



def getHeadingOffset(imu_quat : quaternion, direction : quaternion):
    heading = quaternion(imu_quat.w, 0.0, imu_quat.y, 0.0)
    heading.normalize()

    test = heading.inverse() * direction
    print(vars(test))

    return heading.inverse() * direction

def getLevelOffset(imu_quat : quaternion):
    leveled_quat = quaternion(imu_quat.w, 0.0, imu_quat.y, 0.0)
    leveled_quat.normalize()

    return imu_quat.inverse() * leveled_quat