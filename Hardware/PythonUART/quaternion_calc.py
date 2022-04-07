# This is what's in the driver code. Handling it in the python server might be faster.
from math_helpers import quaternion
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

def updateRotation(q_t0, angle_vector, verbose=False):
    if (type(angle_vector) == list):
        angle_vector = np.asarray(angle_vector, dtype=np.float32)

    ident_m = np.identity(4)
    omega_m = return_omega_matrix(angle_vector)

    if (verbose == True): 
        print("Updating rotation!")
        print("Identity matrix:\n", ident_m)
        print("Omega operator:\n", omega_m)

    mag = np.linalg.norm(angle_vector)

    if (np.isclose(mag, 0, rtol=1e-5)):
        mag = 0.0001

    elapsed_time_ms = 1

    scale_Ident = np.cos(mag * elapsed_time_ms / 2.0)
    scale_Omega = (1.0 / mag) * np.sin(mag * elapsed_time_ms / 2.0)

    ident_m = ident_m * scale_Ident
    omega_m = omega_m * scale_Omega

    result = ident_m + omega_m

    q_t1 = np.matmul(result, q_t0)
    q_t1 = q_t1 / np.linalg.norm(q_t1)

    if (verbose == True):
        print("qt0:\n", q_t0)
        print("Gyroscopic rotation:\n", angle_vector)
        print("Identity scale:\n", scale_Ident)
        print("Omega scale:\n", scale_Omega)
        print("Scaled identity matrix:\n", ident_m)
        print("Scaled omega matrix:\n", omega_m)
        print("Result matrix:\n", result)
        print("qt1:\n", q_t1)

    return quaternion(q_t1[0], q_t1[1], q_t1[2], q_t1[3])