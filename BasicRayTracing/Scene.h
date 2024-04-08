#ifndef SBR_SCENE_H
#define SBR_SCENE_H

#include <glm/glm.hpp>

using namespace glm;

typedef struct Ray {
    vec3 startPoint = vec3( 0, 0, 0 );
    vec3 direction  = vec3( 0, 0, 0 );
} Ray;

typedef struct Material {
    bool   isEmissive       = false;
    vec3   normal           = vec3( 0, 0, 0 );
    vec3   color            = vec3( 0, 0, 0 );
    double specularRate     = 0.0f;
    double roughness        = 1.0f;
    double refractRate      = 0.0f;
    double refractAngle     = 1.0f;
    double refractRoughness = 0.0f;
} Material;

typedef struct HitResult {
    bool     isHit    = false;
    double   distance = 0.0f;
    vec3     hitPoint = vec3( 0, 0, 0 );
    Material material;
} HitResult;

class Shape {
public:
    Shape() {}
    virtual HitResult intersect( Ray ray ) {
        return HitResult();
    }
};

class Triangle : public Shape {
public:
    vec3     p1, p2, p3;
    Material material;

public:
    Triangle() {}
    Triangle( vec3 _p1, vec3 _p2, vec3 _p3, vec3 c ) {
        p1 = _p1, p2 = _p2, p3 = _p3;
        material.normal = normalize( cross( p2 - p1, p3 - p1 ) );
        material.color  = c;
    }

    HitResult intersect( Ray ray );
};

class Sphere : public Shape {
public:
    vec3     center;
    double   radius;
    Material material;

public:
    Sphere() {}
    Sphere( vec3 _center, double _radius, vec3 c ) {
        center         = _center;
        radius         = _radius;
        material.color = c;
    }

    HitResult intersect( Ray ray );
};

#endif  // !SBR_SCENE_H