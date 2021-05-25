#include <math.h>

#include <stdlib.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>

#include <chrono>

using namespace std;
using namespace std::chrono;


class Timers{
private:
	time_point<high_resolution_clock> starts[100];
	int totalTimes[100];
public:
	Timers();
	void Start_Timer(int id);
	void Pause_Timer(int id);
	void Print_Timer(int id);
	void Print_As_Percent(int id1, int id2);
};

class Vector3{
public:
	float x;
	float y;
	float z;
	
    Vector3(){}
	Vector3(float x_, float y_, float z_ = 0);
	Vector3(float magnitude_, Vector3 angle_);
	
    float Magnitude();
	Vector3 Angle();
	void Set_Magnitude(float new_magnitude);
	Vector3 Adjusted_Magnitude(float new_magnitude);
	
    float Dot(Vector3 other);
	Vector3 Cross(Vector3 other);
	
    Vector3 operator+(Vector3 other);
	Vector3 operator+(float f);
	Vector3 operator-(Vector3 other);
	Vector3 operator-(float f);
	Vector3 operator*(float f);
	bool operator!=(Vector3 other);
    
    static float Area_In_Vectors(Vector3 point1, Vector3 point2, Vector3 point3);
};


class Shader{
public:
    float reflective;
	float flat;
	float diffuse;
	Vector3 color;
    Shader(){}
    Shader (float reflective, float flat, float diffuse, Vector3 color);
};


class Intersection{
public:
	bool real; //is the intersection actually inside the face
	Vector3 point; //What point in space did the intersection occur at
	Vector3 normal; //What is the normal to the intersected face
	float distance; //How far did the ray travel before hitting the face
	struct Shader shader; //What is the shader of the face that was hit
	Vector3 color; //Holds the color from previous bounces

    Intersection();
    Intersection(bool real, Vector3 point, Vector3 normal, float distance, Shader shader, Vector3 color);
	Intersection(bool real, Vector3 point, Vector3 normal, float distance, Shader shader);
    Intersection(Vector3 color);
};

class Triangle{
public:
	Vector3 point1;
	Vector3 point2;
	Vector3 point3;
	Shader shader;
	Vector3 normal;
	float area;
	Triangle(){}
	Triangle(Vector3 point1_, Vector3 point2_, Vector3 point3_);
	Triangle(Vector3 point1_, Vector3 point2_, Vector3 point3_, Shader shader_);
	Vector3 Get_Normal();
	float Get_Area();
	Vector3 Intersection_Point(Vector3 ray_origin, Vector3 ray, Vector3 parent_pos);
	Intersection Check_Intersection(Vector3 ray_origin, Vector3 ray, Vector3 parent_pos);
};

const int MAX_TRIANGLES = 600;

class Object{
public:
	Triangle triangles[MAX_TRIANGLES]; //maximum number of triangles is used so the length of the array can be known at compile time
	int triangle_count; //the actual number of triangles in the objecy
	Vector3 pos; // the offset all triangles will have when renhered. Hence, the triangles store "local" coordinates
	Shader shader;

	Object(){}
	Object(string filename, Vector3 pos_, Shader shader_);
	Intersection Closest_Intersection(Vector3 camera_pos, Vector3 ray);
};