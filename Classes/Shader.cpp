#include "header.h"

Shader::Shader (float reflective, float flat, float diffuse, float refractive, float IOF, Vector3 color){
    this->reflective = reflective;
    this->flat = flat;
    this->diffuse = diffuse;
    this->refractive = refractive;
    this->IOF = IOF;
    this->color = color;
}

