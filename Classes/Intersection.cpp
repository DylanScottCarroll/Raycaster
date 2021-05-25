#include "header.h"


Intersection::Intersection(){
	real = false;
	distance = INFINITY;
}

Intersection::Intersection(bool real, Vector3 point, Vector3 normal, float distance, Shader shader, Vector3 color){
	this->real = real;
	this->point = point;
	this->normal = normal;
	this->distance = distance;
	this->shader = shader;
	this->color = color;
}

Intersection::Intersection(bool real, Vector3 point, Vector3 normal, float distance, Shader shader){
	this->real = real;
	this->point = point;
	this->normal = normal;
	this->distance = distance;
	this->shader = shader;
}

Intersection::Intersection(Vector3 color){
	real = false;
	distance = INFINITY;
	this->color = color;
}