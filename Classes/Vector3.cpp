#include <math.h>
#include "header.h"

Vector3::Vector3(float x_, float y_, float z_){
	x = x_;
	y = y_;
	z = z_;

}

Vector3::Vector3(float magnitude_, Vector3 angle_){
	x = cos(angle_.y) * cos(angle_.x) * magnitude_;
	y = cos(angle_.y) * sin(angle_.x) * magnitude_;
	z = sin(angle_.y) * magnitude_;
}


float Vector3::Magnitude(){
	return sqrt(x*x + y*y + z*z);
}

Vector3 Vector3::Angle(){
	float h = atan2(y, x);
	float v = atan2(z, Vector3(x, y, 0).Magnitude() );
	return Vector3(h, v, 0);
}

void Vector3::Set_Magnitude(float new_magnitude){
	Vector3 new_Vector = Vector3(new_magnitude, this->Angle());
	x = new_Vector.x;
	y = new_Vector.y;
	z = new_Vector.z;
}

Vector3 Vector3::Adjusted_Magnitude(float new_magnitude){
	return Vector3(new_magnitude, this->Angle());
}

float Vector3::Dot(Vector3 other){
	return ((other.x * this->x) + (other.y * this->y) + (other.z * this->z));
}

Vector3 Vector3::Cross(Vector3 other){
	Vector3 a = *this;
	Vector3 b = other;


	Vector3 new_vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);

	return new_vector3;
}

Vector3 Vector3::Copy(){
	return Vector3(this->x, this->y, this->z);
}

Vector3 Vector3::operator+(Vector3 other){
	Vector3 new_vector3(x + other.x, y + other.y, z + other.z);
	return new_vector3;
}
Vector3 Vector3::operator+(float f){
	Vector3 new_vector3(x + f, y + f, z + f);
	return new_vector3;
}


Vector3 Vector3::operator-(Vector3 other){
	Vector3 new_vector3(x - other.x, y - other.y, z - other.z);
	return new_vector3;
}

Vector3 Vector3::operator-(float f){
	Vector3 new_vector3(x - f, y - f, z - f);
	return new_vector3;
}

Vector3 Vector3::operator*(float f){
	Vector3 new_vector3(x * f, y * f, z * f);
	return new_vector3;
}

bool Vector3::operator!=(Vector3 other){
	return (x!=other.x || y!=other.y || z!=other.z);
}

float Vector3::Area_In_Vectors(Vector3 point1, Vector3 point2, Vector3 point3){
	float ax = point2.x - point1.x;
	float ay = point2.y - point1.y;
	float az = point2.z - point1.z;
	float bx = point3.x - point1.x;
	float by = point3.y - point1.y;
	float bz = point3.z - point1.z;

	float x = ay*bz - az*by;
	float y = az*bx - ax*bz;
	float z = ax*by - ay*bx;

	return sqrt(x*x + y*y + z*z) / 2.0;
}


