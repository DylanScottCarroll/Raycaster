#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <chrono>

#include "bitmap.c"

using namespace std;
using namespace std::chrono;

const int MAX_TRIANGLES = 600;

const float PI = 3.1415926;

const int MAX_BOUNCE = 3;
const float DIFFUSE_RAY_DENSITY = 100;
const float DIFFUSE_RAY_SPREAD = (4*PI)/5;

const int width = 500;
const int height = 500;




class Timers{
public:

	time_point<high_resolution_clock> starts[100];
	int totalTimes[100];

	int i = 0;

	Timers(){}

	void Start_Timer(int id){
		starts[id] =  high_resolution_clock::now();
	}

	void Pause_Timer(int id){
		auto duration = duration_cast<microseconds>( high_resolution_clock::now() - starts[id] );
		totalTimes[id] += duration.count();
	}

	void Print_Timer(int id){
		cout << id << ": " << totalTimes[id] << endl;
	}

	void Print_As_Percent(int id1, int id2){
		cout << id1 << ": " << 100*totalTimes[id1]/totalTimes[id2] << "%" << endl;
	}


};


class Vector3{
public:
	float x;
	float y;
	float z;

	Vector3(){ }

	Vector3(float x_, float y_, float z_ = 0){
		x = x_;
		y = y_;
		z = z_;

	}

	Vector3(float magnitude_, Vector3 angle_){
		x = cos(angle_.y) * cos(angle_.x) * magnitude_;
		y = cos(angle_.y) * sin(angle_.x) * magnitude_;
		z = sin(angle_.y) * magnitude_;
	}


	float Magnitude(){
		return sqrt(x*x + y*y + z*z);
	}

	Vector3 Angle(){
		float h = atan2(y, x);
		float v = atan2(z, Vector3(x, y, 0).Magnitude() );
		return Vector3(h, v, 0);
	}

	void Set_Magnitude(float new_magnitude){
		Vector3 new_Vector = Vector3(new_magnitude, this->Angle());
		x = new_Vector.x;
		y = new_Vector.y;
		z = new_Vector.z;
	}

	Vector3 Adjusted_Magnitude(float new_magnitude){
		return Vector3(new_magnitude, this->Angle());
	}

	float Dot(Vector3 other){
		return ((other.x * this->x) + (other.y * this->y) + (other.z * this->z));
	}

	Vector3 Cross(Vector3 other){
		Vector3 a = *this;
		Vector3 b = other;


		Vector3 new_vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);

		return new_vector3;
	}

	Vector3 operator+(Vector3 other){
		Vector3 new_vector3(x + other.x, y + other.y, z + other.z);
		return new_vector3;
	}
	Vector3 operator+(float f){
		Vector3 new_vector3(x + f, y + f, z + f);
		return new_vector3;
	}


	Vector3 operator-(Vector3 other){
		Vector3 new_vector3(x - other.x, y - other.y, z - other.z);
		return new_vector3;
	}

	Vector3 operator-(float f){
		Vector3 new_vector3(x - f, y - f, z - f);
		return new_vector3;
	}

	Vector3 operator*(float f){
		Vector3 new_vector3(x * f, y * f, z * f);
		return new_vector3;
	}

	bool operator!=(Vector3 other){
		return (x!=other.x || y!=other.y || z!=other.z);
	}



};

struct Shader{
	float reflective;
	float flat;
	float diffuse;
	Vector3 color;
}; 

Shader Make_Shader(float reflective, float flat, float diffuse, Vector3 color){
	Shader shader = {reflective, flat, diffuse, color};
	return shader;
}

struct Intersection_Data
{
	//Intersection data
	bool real; //is the intersection actually inside the face
	Vector3 point;
	Vector3 normal;
	float distance;
	struct Shader shader;
	Vector3 color;
};



float Area_In_Vectors(Vector3 point1, Vector3 point2, Vector3 point3){
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

class Triangle{
public:

	Vector3 point1;
	Vector3 point2;
	Vector3 point3;

	Shader shader;

	Vector3 normal;

	float area;

	Triangle(){ }

	Triangle(Vector3 point1_, Vector3 point2_, Vector3 point3_){
		point1 = point1_;
		point2 = point2_;
		point3 = point3_;

		normal = this->Get_Normal();
		area = this->Get_Area();
	}

	Triangle(Vector3 point1_, Vector3 point2_, Vector3 point3_, Shader shader_){
		point1 = point1_;
		point2 = point2_;
		point3 = point3_;
		
		shader = shader_;

		normal = this->Get_Normal();
		area = this->Get_Area();
	}

	Vector3 Get_Normal(){
		Vector3 a = point2 - point1;
		Vector3 b = point3 - point1;

		return a.Cross(b).Adjusted_Magnitude(1);
	}

	float Get_Area(){
		Vector3 a = point2 - point1;
		Vector3 b = point3 - point1;
		return ( a.Cross(b).Magnitude() / 2.0);
	}

	Vector3 Intersection_Point(Vector3 origin, Vector3 vec, Vector3 parent_pos){
		float lambda = (normal.x*(origin.x - parent_pos.x - point1.x) + 
						normal.y*(origin.y - parent_pos.y - point1.y) +
						normal.z*(origin.z - parent_pos.z - point1.z))  
						/ (-normal.Dot(vec));
		Vector3 intx_point = (vec * lambda) + (origin - parent_pos);

		return intx_point;
	}

	Intersection_Data Check_Intersection(Vector3 origin, Vector3 vec, Vector3 parent_pos){

		Vector3 point = Intersection_Point(origin, vec, parent_pos);

		float sum_area = Area_In_Vectors(point1, point2, point) 
						+Area_In_Vectors(point2, point3, point) 
						+Area_In_Vectors(point3, point1, point);
						
		point = point + parent_pos - origin;
		float distance = (vec.x!=0) ? (point.x/vec.x) : (point.y/vec.y);
		
		bool real = (abs(sum_area - area) < 0.0001) && (distance > 0.001); 

		Intersection_Data data = {real, point+origin, normal, distance, shader};
		
		return data;
	}


};

class Object{
public:
	Triangle triangles[MAX_TRIANGLES]; //maximum number of triangles is used so the length of the array can be known at compile time
	int triangle_count; //the actual number of triangles in the objecy
	Vector3 pos; // the offset all triangles will have when renhered. Hence, the triangles store "local" coordinates
	
	Shader shader;

	Object(){ }

	Object(string filename, Vector3 pos_, Shader shader_){ 
		pos = pos_;
		
		shader = shader_;

		ifstream infile;
		infile.open(filename, ios::in);
		string data;

		int lines = count(istreambuf_iterator<char>(infile), istreambuf_iterator<char>(), 'v');

		infile.clear();
		infile.seekg (0, ios::beg);

		lines += count(istreambuf_iterator<char>(infile), istreambuf_iterator<char>(), 'f');

		infile.clear();
		infile.seekg (0, ios::beg); //return the stream to the beginning to read the file

		Vector3 temp_verts[MAX_TRIANGLES * 3]; //hypotherically each triangle can have its own 3 vertecies
		int vrt_i = 1; //next index of the temp_verts array to be used

		//the three ints are together a triangle, which is built using index references to vertecies
		int temp_triangles[3][MAX_TRIANGLES];
		int tri_i = 0; //next index of the temp_triangles array to be used


		// loops through the file and populates temp_triangles and temp_verts
		for(int l = 0; l < lines; l++){
			infile >> data;


			if(data[0] == 'v' || data[0] == 'V'){
				size_t sz; 
				
				string s1;
				string s2;
				string s3;
				infile >> s1;
				infile >> s2;
				infile >> s3;	

				float f1 = stof(s1, &sz);
				float f2 = stof(s2, &sz);
				float f3 = stof(s3, &sz);

				//cout << f1 << endl;
				temp_verts[vrt_i] = Vector3(f1, f2, f3);
				vrt_i++;

				
			}
			if(data[0] == 'f' || data[0] == 'F'){
				size_t sz; 

				string s1;
				string s2;
				string s3;
				infile >> s1;
				infile >> s2;
				infile >> s3;	
				
				int i1 = stoi(s1, &sz);
				int i2 = stoi(s2, &sz);
				int i3 = stoi(s3, &sz);

				//cout << i1 << " " << i2 << " " << i3 << endl;

				temp_triangles[0][tri_i] = i1;
				temp_triangles[1][tri_i] = i2;
				temp_triangles[2][tri_i] = i3;

				tri_i++;

			}
		}	


		/*vrt_i is now used as the count of how many trianlges are in the temp_triangles array
		if it is higher than the maximum allowed triangles, then it is shortened
		*/

		if(tri_i > MAX_TRIANGLES)
			tri_i = MAX_TRIANGLES;


		triangle_count = tri_i;

		//cout << triangle_count << endl;

		for(int i = 0; i < tri_i; i++){
			//creates a new triangle from the temp_verts array using the indecies from the three ints in the temp_triangles array


			//cout << temp_verts[temp_triangles[0][i]].z << " ";
			//cout << temp_verts[temp_triangles[0][i]].z << " ";
			//cout << temp_verts[temp_triangles[0][i]].z << endl;

			Triangle new_triangle(temp_verts[temp_triangles[0][i]], temp_verts[temp_triangles[1][i]], temp_verts[temp_triangles[2][i]], shader);

			triangles[i] = new_triangle; //adds the new triangle to the object's triangle list


		}


	}

	//returns the closes real intersection otherwise return a dummy nonreal intersection
	Intersection_Data Closest_Intersection(Vector3 camera_pos, Vector3 ray){
		//the starting closest is an unreal intersection infinite distance away. (1.0/0.0) is +infinity. The other values are dummy
		Intersection_Data closest = {false, Vector3(0, 0, 0), Vector3(0, 0, 0), (1.0f/0.0f)};


		for(int i = 0; i < triangle_count; i++){
			Intersection_Data cur_intx = triangles[i].Check_Intersection(camera_pos, ray, this->pos);

			//if the current intersection is real and closer then it replaces the previous closest
			if(cur_intx.real  && cur_intx.distance < closest.distance){

				closest = cur_intx; 
			}

		}

		//if none are real then the initial dummy is returned, which has a false reality, which should be checked upon return
		return closest;

	}


};

Intersection_Data Cast(Vector3 ray, Vector3 camera_pos, Object objects[], int object_count, int max_bounce, int bounce){
	
	Vector3 sky_color = Vector3(64, 64, 64);
	Intersection_Data intx = Intersection_Data{false, Vector3(0, 0, 0), Vector3(0, 0, 0), ((float)1/(float)0), Shader{},  sky_color};

	//Cast ray to each object. Overwrite inx if the new intersection is closer
	for(int o = 0; o < object_count; o++){

		Intersection_Data new_intx = objects[o].Closest_Intersection(camera_pos, ray);
		if(new_intx.distance < intx.distance){ intx = new_intx;}
	}

	//-1 bounce means that we don't care about the color, we just want the other intersection data
	if(bounce == -1){
		return intx;
	}

	Shader shader = intx.shader;
	if(intx.real){
		
		
		bool shaded = Cast(Vector3(-1, 0, 1), intx.point, objects, object_count, max_bounce, -1).real;
		Vector3 flat_color;
		if(!shaded)
			flat_color = shader.color;
		else{
			float r = (sky_color.x*shader.color.x)/255.0;
			float g = (sky_color.y*shader.color.y)/255.0;
			float b = (sky_color.z*shader.color.z)/255.0;

			flat_color = Vector3(r, g, b);
		}

		Vector3 reflective_color;
		if(bounce < max_bounce && shader.reflective > 0){
			//Formula for a reflection ray
			//https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
			Vector3 normal = intx.normal;
			Vector3 reflection_ray = (ray - (normal * ray.Dot(normal) * 2.0)).Adjusted_Magnitude(1);
			
			reflective_color = Cast(reflection_ray, intx.point, objects, object_count, max_bounce, bounce + 1).color;
			
			reflective_color.x = (shader.color.x/255) * reflective_color.x;
			reflective_color.y = (shader.color.y/255) * reflective_color.y;
			reflective_color.z = (shader.color.z/255) * reflective_color.z;
		}
		else{
			reflective_color = Vector3(0, 0, 0);
		}
		
		Vector3 diffuse_color = Vector3(0, 0, 0);
		if(bounce < max_bounce && shader.diffuse > 0){
			
			
			for(int i = 0; i < DIFFUSE_RAY_DENSITY; i++){
				
				
				Vector3 normal = Vector3(intx.normal.x, intx.normal.y, intx.normal.z);
				if(ray.Dot(intx.normal) > 0){
					normal = normal * -1;
				}

				//Get a random vector, bur change it so it can only point away from the plane
				//Vector3 randomVector = Vector3(0.99, Vector3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX)) + normal;
				//randomVector.Set_Magnitude(1);
				
				Vector3 angle = normal.Angle();
				float d_angle_x =  DIFFUSE_RAY_SPREAD * (( rand()/(float)RAND_MAX) - 0.5);
				float d_angle_y =  DIFFUSE_RAY_SPREAD * (( rand()/(float)RAND_MAX) - 0.5);
				Vector3 random = Vector3(1, Vector3(angle.x+d_angle_x, angle.y+d_angle_y));

				diffuse_color = diffuse_color + Cast(random, intx.point, objects, object_count, max_bounce, bounce + 1).color;
			}


			//Find the average color
			diffuse_color = diffuse_color * (1.0f / DIFFUSE_RAY_DENSITY);

			//Dampen the reflection color based on the color of the object
			diffuse_color.x = (shader.color.x/255) * diffuse_color.x;
			diffuse_color.y = (shader.color.y/255) * diffuse_color.y;
			diffuse_color.z = (shader.color.z/255) * diffuse_color.z;
		}

		intx.color = reflective_color*shader.reflective + flat_color*shader.flat + diffuse_color*shader.diffuse;


		return intx;

	}
	
	return intx;


}


Vector3 camera_pos = Vector3(0, 0, 0);
float FOV = (70*PI)/180;
Vector3 camera_angle(0, -(PI/8), 0);
unsigned char pixels[width * height * 3]; 

int main(){
	
	
	Vector3 empty(0, 0, 0);

	int object_count = 4;
	Object objects[object_count];	
	objects[0] = Object("objects\\floor.obj", Vector3(0, 0, 0), 
		Shader{0.5, 0.5, 0, Vector3(200, 200, 200)} );
	objects[1] = Object("objects\\cube.obj", Vector3(1, 0, 0.1), 
		Shader{0, 0, 1, Vector3(255, 255, 255)} );
	objects[2] = Object("objects\\cube.obj", Vector3(-2, -2.5, 0.1),
		Shader{0.5, 0.5, 0, Vector3(0, 255, 0)});
	objects[3] = Object("objects\\sphere2.obj", Vector3(-2, 2.5, 0.1), 
		Shader{0.8, 0.2, 0, Vector3(0, 0, 255)});
	

 	for(int z = 0; z <= 40; z ++) //z is the number of steps in the multi-frame animation
	{
		float theta = z/20.0 * PI;
		Vector3 camera_angle = Vector3(-theta, 0); 
		Vector3 camera_pos = Vector3(-15.0 * cos(theta), 15.0 * sin(theta), 0.5);



		//Vectors pointing to the corners of the viewport 
		Vector3 viewport_left = Vector3(-tan(FOV/2.0), Vector3(camera_angle.x + (PI/2.0), 0, 0))+Vector3(1, camera_angle);
		Vector3 viewport_right = Vector3(tan(FOV/2.0), Vector3(camera_angle.x + (PI/2.0), 0, 0))+Vector3(1, camera_angle);
		
		Vector3 viewport_top = Vector3(-tan(FOV/2.0), Vector3(0, camera_angle.y + (PI/2.0), 0))+Vector3(1, camera_angle);
		Vector3 viewport_bottom = Vector3(tan(FOV/2.0), Vector3(0, camera_angle.y + (PI/2.0), 0))+Vector3(1, camera_angle);

		for(int y = 0; y < height; y++){
			cout << y << endl;

			
			for(int x = 0; x < width; x++){
				//cout << "\t" << x << endl;
				

				float local_y_pos = ( (float)y/height);
				float local_x_pos = ( (float)x/width);

				//Interpolate between the left and the right
				Vector3 top_x = (viewport_left + viewport_top)*(1.0-local_x_pos) + (viewport_right + viewport_top)*local_x_pos;
				Vector3 bottom_x = (viewport_left + viewport_bottom)*(1.0-local_x_pos) + (viewport_right + viewport_bottom)*local_x_pos;

				//interpolate between the top and the bottom
				Vector3 ray = top_x*(1-local_y_pos) + bottom_x*local_y_pos;
				ray.Set_Magnitude(1);

				Vector3 color = Cast(ray, camera_pos, objects, object_count, MAX_BOUNCE, 0).color;

				pixels[(y*width*3) + (x*3)] = (unsigned char) (color.x);
				pixels[(y*width*3) + (x*3) + 1] = (unsigned char) (color.y);
				pixels[(y*width*3) + (x*3) + 2] = (unsigned char) (color.z);

				

			}

			//cout << y << endl;
			

		}

		string saveDirS = "output\\" + to_string(z) + ".bmp";
		int stringLen = saveDirS.length();

		char saveDir[stringLen + 1];
		saveDir[stringLen] = '\0';
		for(int i = 0; i < stringLen; i++){
			saveDir[i] = saveDirS[i];
		}
		cout << saveDir << endl;


		Save(pixels, width, height, saveDir);
	}


	return 0;
}