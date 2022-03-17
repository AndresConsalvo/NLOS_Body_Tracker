# This is what's in the driver code. Handling it in the python server might be faster.
import numpy as np

def return_omega_matrix(angle_vector):
    omega_m = np.zeros([4, 4])

    omega_m[0][0] = 0
    omega_m[0][1] = -angle_vector[0]
    omega_m[0][2] = -angle_vector[1]
    omega_m[0][3] = -angle_vector[2]

    omega_m[1][0] = angle_vector[0]
    omega_m[1][1] = 0
    omega_m[1][2] = angle_vector[2]
    omega_m[1][3] = angle_vector[1]

    omega_m[2][0] = angle_vector[1]
    omega_m[2][1] = -angle_vector[2]
    omega_m[2][2] = 0
    omega_m[2][3] = angle_vector[0]

    omega_m[3][0] = angle_vector[2]
    omega_m[3][1] = angle_vector[1]
    omega_m[3][2] = -angle_vector[0]
    omega_m[3][3] = 0

    return omega_m

# x, y, z
angle_vector = np.array([0, 3.14, 0])
# w, x, y, z
q_t0 = np.array([1, 0, 0, 0]).transpose()


ident_m = np.identity(4)
omega_m = return_omega_matrix(angle_vector)

print(ident_m)

mag = np.linalg.norm(angle_vector)

if (np.isclose(mag, 0, rtol=1e-5)):
    mag = 0.0001

elapsed_time_ms = 1

scale_Ident = np.cos(mag * elapsed_time_ms / 2.0)

scale_Omega = (1.0 / mag) * np.sin(mag * elapsed_time_ms / 2.0)
ident_m = ident_m * scale_Ident

omega_m = omega_m * scale_Omega

print(scale_Ident)
print(scale_Omega)

result = ident_m + omega_m
print(result)

q_t1 = result * q_t0

q_t1 = q_t1[:,0]

#q_t1 = q_t1 / np.linalg.norm(angle_vector)

print(q_t1)