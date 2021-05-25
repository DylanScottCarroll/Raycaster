#include "header.h"

Shader::Shader (float reflective, float flat, float diffuse, Vector3 color){
    this->reflective = reflective;
    this->flat = flat;
    this->diffuse = diffuse;
    this->color = color;
}

