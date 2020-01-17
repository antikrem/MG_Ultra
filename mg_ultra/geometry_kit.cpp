#include "geometry_kit.h"

void geom_kit::populate_sphere(vector<float>& vertices, vector<int>& indices, int stacks, int sectors, float radius) {

	float xy, z;

	int k1, k2;

	float sectorStep = 2 * PI / sectors;
	float stackStep = PI / stacks;

	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; i++) {
		stackAngle = PI / 2 - i * stackStep;
		xy = radius * cos(stackAngle);
		z = radius * sin(stackAngle);

		k1 = i * (sectors + 1);
		k2 = k1 + sectors + 1;

		for (int j = 0; j <= sectors; j++, k1++, k2++) {
			sectorAngle = j * sectorStep;

			vertices.push_back(xy * cosf(sectorAngle));
			vertices.push_back(xy * sinf(sectorAngle));
			vertices.push_back(z);

			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
};