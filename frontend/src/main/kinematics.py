from tracker import *
from math_helpers import quaternion

HMD = 0
CHEST = -1
WAIST = 1
LKNEE = 2
RKNEE = 3
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


# -z = forward
# x = right
# y = up
def update_body(kinematics:Skeleton, trackers:dict, hmd_pos, hmd_quat, verbose=False):
    chest = trackers.get(CHEST)
    waist = trackers.get(WAIST)
    lknee = trackers.get(LKNEE)
    rknee = trackers.get(RKNEE)
    lfoot = trackers.get(LFOOT)
    rfoot = trackers.get(RFOOT)

    neck_pos = hmd_pos + (hmd_quat * quaternion(0, 0, -kinematics.Head_to_Neck, 0) * hmd_quat.get_inverse())
    waist_pos = neck_pos + (waist.quat * quaternion(0, 0, -kinematics.Neck_to_Waist, 0) * waist.quat.get_inverse())

    lhip_pos = waist_pos + (waist.quat * quaternion(0, -kinematics.Hip_Width / 2, 0, 0) * waist.quat.get_inverse())
    rhip_pos = waist_pos + (waist.quat * quaternion(0, kinematics.Hip_Width / 2, 0, 0) * waist.quat.get_inverse())

    lknee_pos = lhip_pos + (lknee.quat * quaternion(0, 0, -kinematics.Hip_to_Knee, 0) * lknee.quat.get_inverse())
    rknee_pos = rhip_pos + (rknee.quat * quaternion(0, 0, -kinematics.Hip_to_Knee, 0) * rknee.quat.get_inverse())

    lfoot_pos = lknee_pos + (lfoot.quat * quaternion(0, 0, -kinematics.Knee_to_Foot, 0) * lfoot.quat.get_inverse())
    rfoot_pos = rknee_pos + (rfoot.quat * quaternion(0, 0, -kinematics.Knee_to_Foot, 0) * rfoot.quat.get_inverse())

    if (lfoot_pos.y < 0):
        lfoot_pos.y = 0
    
    if (rfoot_pos.y < 0):
        rfoot_pos.y = 0

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
    
    


""" hmd_pos = [0.0, 1.8, 0.0]
hmd_quat = [1.0, 0.0, 0.0, 0.0]

trackers = {}
trackers[1] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 1)
trackers[2] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 2)
trackers[3] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 3)
trackers[4] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 4)
trackers[5] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 5)
trackers[6] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 6)

kinematics = Skeleton()

update_body(kinematics, trackers, hmd_pos, hmd_quat, verbose=True)

print(vars(trackers[1].pos))
print(vars(trackers[2].pos))
 """



