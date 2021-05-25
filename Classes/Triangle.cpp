#include "header.h"

Triangle::Triangle(Vector3 point1_, Vector3 point2_, Vector3 point3_){
	point1 = point1_;
	point2 = point2_;
	point3 = point3_;

	normal = this->Get_Normal();
	area = this->Get_Area();
}

Triangle::Triangle(Vector3 point1_, Vector3 point2_, Vector3 point3_, Shader shader_){
	point1 = point1_;
	point2 = point2_;
	point3 = point3_;
	
	shader = shader_;

	normal = this->Get_Normal();
	area = this->Get_Area();
}

Vector3 Triangle::Get_Normal(){
	Vector3 a = point2 - point1;
	Vector3 b = point3 - point1;

	return a.Cross(b).Adjusted_Magnitude(1);
}

float Triangle::Get_Area(){
	Vector3 a = point2 - point1;
	Vector3 b = point3 - point1;
	return ( a.Cross(b).Magnitude() / 2.0);
}

Vector3 Triangle::Intersection_Point(Vector3 ray_origin, Vector3 ray, Vector3 parent_pos){
	float lambda = (normal.x*(ray_origin.x - parent_pos.x - point1.x) + 
					normal.y*(ray_origin.y - parent_pos.y - point1.y) +
					normal.z*(ray_origin.z - parent_pos.z - point1.z))  
					/ (-normal.Dot(ray));
	Vector3 intx_point = (ray * lambda) + (ray_origin - parent_pos);

	return intx_point;
}

Intersection Triangle::Check_Intersection(Vector3 ray_origin, Vector3 ray, Vector3 parent_pos){

	Vector3 point = Intersection_Point(ray_origin, ray, parent_pos);

	float sum_area = Vector3::Area_In_Vectors(point1, point2, point) 
					+Vector3::Area_In_Vectors(point2, point3, point) 
					+Vector3::Area_In_Vectors(point3, point1, point);
					
	point = point + parent_pos - ray_origin;
	float distance = (ray.x!=0) ? (point.x/ray.x) : (point.y/ray.y);
	
	bool real = (abs(sum_area - area) < 0.0001) && (distance > 0.001); 

	Intersection data = Intersection(real, point+ray_origin, normal, distance, shader);
	
	return data;
}


