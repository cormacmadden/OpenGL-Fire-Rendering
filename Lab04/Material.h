#pragma once
#include "Texture.h"
#include "maths_funcs.h"

class Material {
public:
    
    Material::Material()
    {

    }
    vec3 AmbientColor = vec3(0.0f, 0.0f, 0.0f);
    vec3 DiffuseColor = vec3(0.0f, 0.0f, 0.0f);
    vec3 SpecularColor = vec3(0.0f, 0.0f, 0.0f);
    float shininess = 32.0f;
    // TODO: need to deallocate  these
    Texture pDiffuse; // base color of the material
    Texture pSpecularExponent;
};
