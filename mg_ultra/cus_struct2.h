/*Better definition of a point*/
#ifndef __CUS_STRUCT_2__
#define __CUS_STRUCT_2__

#include <cmath>
#include "constants.h"

#if __has_include(<glm/glm.hpp>)
	#include <glm/glm.hpp>
	#define GLM_ADDED
#endif

/*Use for both rectangular points and polar representations in 2d
x,y will always be accurate, but mag, angle are private and only accurate when called with mag(), ang() and related
When the mag is 0, angle is still considered unique*/
class Point2 {
	float magnitude;
	float angle;

	void setRectComponent() {
		x = magnitude * sin(angle * PI / 180.0f);
		y = magnitude * cos(angle * PI / 180.0f);
	}

	void setMagComponent() {
		magnitude = sqrt(pow(x, 2) + pow(y, 2));
		angle = (atan2(y, x) * 180.0f / PI);
	}

public:
	float x;
	float y;

	Point2(float x, float y) {
		this->x = x;
		this->y = y;
	}

	float getMag() {
		magnitude = sqrt(pow(x, 2) + pow(y, 2));
		return magnitude;
	}

	float getAng() {
		angle = (atan2(y, x) * 180 / PI);
		return angle;
	}

	//sets magnitude
	void setMag(float magnitude) {
		this->magnitude = magnitude;
		setRectComponent();
	}

	//sets angle
	void setAng(float angle) {
		this->angle = angle;
		setRectComponent();
	}

	//sets polar part
	void setPol(float magnitude, float angle) {
		this->magnitude = magnitude;
		this->angle = angle;
		setRectComponent();
	}

	//generates a point2 from mag angle
	static Point2 generateFromMagAng(float mag, float ang) {
		Point2 a(0, 0);
		a.setMag(mag);
		a.setAng(ang);
		return a;
	}

	Point2 operator+(const Point2& b) {
		return Point2(this->x + b.x, this->y + b.y);
	}

	void operator+=(const Point2& b) {
		this->x += b.x;
		this->y += b.y;
	}

	Point2 operator-(const Point2& b) {
		return Point2(this->x - b.x, this->y - b.y);
	}

	void operator-=(const Point2& b) {
		this->x -= b.x;
		this->y -= b.y;
	}

	Point2 operator*(const float& b) {
		return Point2(b*this->x, b*this->y);
	}

	Point2 getUnitVector() {
		setMagComponent();
		return Point2(this->x / magnitude, this->y / magnitude);
	}

};

/*Use for rectangular points in 3d*/
class Point3 {
public:
	float x;
	float y;
	float z;

	Point3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Point3(Point2 xy, float z) {
		Point3(xy.x, xy.y, z);
	}

	Point3(float val) {
		this->x = val;
		this->y = val;
		this->z = val;
	}

	Point3 operator+(const Point3& b) {
		return Point3(this->x + b.x, this->y + b.y, this->z + b.z);
	}


#ifdef GLM_ADDED
	//as glm is avalible use the following conversion
	glm::vec3 getVec3() {
		return glm::vec3(this->x, this->y, this->z);
	}
#endif
};

#endif