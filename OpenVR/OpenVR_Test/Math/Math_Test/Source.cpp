#include "Matrix.h"
#include <iostream>

int main() {
	ang_rate angle_vector(0, 0, 0);

	omega omega_op(angle_vector);

	double mag = angle_vector.get_magnitude();
	double scale_cos = cos(mag / 2.0);
	double scale_sin = (1 / mag) * sin(mag / 2.0);

	double iMatrixScaled[16];

	for (int i = 0; i < 16; i++) {
		iMatrixScaled
	}
}