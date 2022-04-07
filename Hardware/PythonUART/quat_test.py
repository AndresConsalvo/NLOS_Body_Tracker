from math_helpers import quaternion

q_hmd = quaternion(-0.063, -0.120, -0.952, 0.275)
q_imu = quaternion(-0.776, 0.249, -0.093, 0.572)

leveled_quat = quaternion(q_imu.w, 0.0, q_imu.y, 0.0)
leveled_quat.normalize()

levelOffset = q_imu.inverse() * leveled_quat

direction = quaternion(q_hmd.w, 0.0, q_hmd.y, 0.0)
direction.normalize()

heading = quaternion(q_imu.w, 0.0, q_imu.y, 0.0)
heading.normalize()

headingOffset = heading.inverse() * direction

imu_lfoot = headingOffset * q_imu * levelOffset
imu_lfoot.normalize()
print(vars(imu_lfoot))




