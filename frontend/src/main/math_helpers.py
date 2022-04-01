import numpy as np

class quaternion:
    def __init__(self, w, x, y, z) -> None:
        self.w = w
        self.x = x
        self.y = y
        self.z = z

    def get_inverse(self):
        return quaternion(self.w, -self.x, -self.y, -self.z)

    def __mul__(self, other):
        w = (self.w * other.w) - (self.x * other.x) - (self.y * other.y) - (self.z * other.z)
        x = (self.w * other.x) + (self.x * other.w) + (self.y * other.z) - (self.z * other.y)
        y = (self.w * other.y) - (self.x * other.z) + (self.y * other.w) + (self.z * other.x)
        z = (self.w * other.z) + (self.x * other.y) - (self.y * other.x) + (self.z * other.w)

        return quaternion(w, x, y, z)

    def __add__(self, other):
        w = self.w + other.w
        x = self.x + other.x
        y = self.y + other.y
        z = self.z + other.z

        return quaternion(w, x, y, z)

    def get_np_array(self):
        return np.array([self.w, self.x, self.y, self.z])


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

def updateRotation(q_t0:quaternion, angle_vector, verbose=False):
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
        return q_t0
    else:
        scale_Ident = np.cos(mag / 2.0)
        scale_Omega = (1.0 / mag) * np.sin(mag / 2.0)


    ident_m = ident_m * scale_Ident
    omega_m = omega_m * scale_Omega

    result = ident_m + omega_m

    q_t1 = np.matmul(result, q_t0.get_np_array())
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