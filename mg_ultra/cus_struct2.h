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
		x = magnitude * cos(angle * PI / 180.0f);
		y = magnitude * sin(angle * PI / 180.0f);
	}

	void setMagComponent() {
		magnitude = sqrt(pow(x, 2) + pow(y, 2));
		angle = (atan2(y, x) * 180.0f / PI);
	}

public:
	float x;
	float y;

	Point2() {
		this->x = 0;
		this->y = 0;
	}

	Point2(float v) {
		this->x = v;
		this->y = v;
	}

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
	static Point2 generateFromPolar(float mag, float ang) {
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
		return Point2(b * this->x, b * this->y);
	}

	Point2 getUnitVector() {
		setMagComponent();
		return Point2(this->x / magnitude, this->y / magnitude);
	}

	//computes distance to another point
	float distanceTo(const Point2& point) {
		return sqrt(pow(point.x - this->x, 2) + pow(point.y - this->y, 2));
	}

#ifdef GLM_ADDED
	//as glm is avalible use the following conversion
	glm::vec2 getVec2() {
		return glm::vec2(this->x, this->y);
	}
#endif
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
		this->x = xy.x;
		this->y = xy.y;
		this->z = z;
	}

	Point3(float val) {
		this->x = val;
		this->y = val;
		this->z = val;
	}

	float magnitude() {
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	Point3 getUnitVector() {
		float mag = magnitude();
		return Point3(x / mag, y / mag, z / mag);
	}

	Point3 operator+(const Point3& b) {
		return Point3(this->x + b.x, this->y + b.y, this->z + b.z);
	}

	Point3 operator-(const Point3& b) {
		return Point3(this->x - b.x, this->y - b.y, this->z - b.z);
	}

	void operator+=(const Point3& b) {
		this->x += b.x;
		this->y += b.y;
		this->z += b.z;
	}

	Point3 operator*(float b) const {
		return Point3(b * this->x, b* this->y, b * this->z);
	}

	//sets x, y, does not effect z
	void setXY(const Point2& val) {
		x = val.x;
		y = val.y;
	}

	//returns x, y as point2
	Point2 getXY() const {
		return Point2(x, y);
	}

	//calculate distance from one vector to another
	float distanceTo(Point3& b) {
		return sqrt(pow((x - b.x), 2) + pow((y - b.y), 2) + pow((z - b.z), 2));
	}

	//returns a normalised vector of direction to another point
	Point3 directionTo(Point3& b) {
		Point3 temp = b - *this;
		return temp * (1.0f / temp.magnitude());
	}

	//returns two vectors crossed
	static Point3 cross(const Point3& a, const Point3& b) {
		return Point3(
			a.y * b.z - a.z  * b.y,
			a.z * b.x - a.x  * b.z,
			a.x * b.y - a.y  * b.x
		);
	}

	//returns two vectors dot
	static float dot(const Point3& a, const Point3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	//rotates this vector about an axis of rotation defind by normal vector k by theta degrees
	Point3 rotate(const Point3& k, float degrees) {
		return
			*this * cos(DEG2RAD(degrees))
			+ Point3::cross(k, *this) * sin(DEG2RAD(degrees))
			+ k * (Point3::dot(k, *this) * (1 - cos(DEG2RAD(degrees))));
	}

	//returns true if in given box
	bool inBox(Point3& center, Point3& size) {
		return (abs(x - center.x) < size.x) && (abs(y - center.y) < size.y) && (abs(z - center.z) < size.z);
	}
	
#ifdef GLM_ADDED
	//as glm is avalible use the following conversion
	glm::vec3 getVec3() {
		return glm::vec3(this->x, this->y, this->z);
	}
#endif
};

#endif