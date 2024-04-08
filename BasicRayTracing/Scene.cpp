#include "Scene.h"

HitResult Triangle::intersect( Ray ray ) {
    HitResult res;

    vec3 S = ray.startPoint;
    vec3 d = ray.direction;
    vec3 N = material.normal;
    if ( dot( N, d ) > 0.0f )
        N = -N;

    if ( fabs( dot( N, d ) ) < 1e-5f )
        return res;

    float t = ( dot( N, p1 ) - dot( S, N ) ) / dot( d, N );
    if ( t < 0.5e-3 )
        return res;

    vec3 P = S + d * t;

    vec3 c1 = cross( p2 - p1, P - p1 );
    vec3 c2 = cross( p3 - p2, P - p2 );
    vec3 c3 = cross( p1 - p3, P - p3 );
    vec3 n  = material.normal;
    if ( dot( c1, n ) < 0 || dot( c2, n ) < 0 || dot( c3, n ) < 0 )
        return res;

    res.isHit           = true;
    res.distance        = t;
    res.hitPoint        = P;
    res.material        = material;
    res.material.normal = N;
    return res;
};

HitResult Sphere::intersect( Ray ray ) {
    HitResult res;

    vec3 S = ray.startPoint;
    vec3 d = ray.direction;

    float OS = length( center - S );
    float SH = dot( center - S, d );
    float OH = sqrt( pow( OS, 2 ) - pow( SH, 2 ) );

    if ( OH > radius )
        return res;

    float PH = sqrt( pow( radius, 2 ) - pow( OH, 2 ) );

    float t1 = length( SH ) - PH;
    float t2 = length( SH ) + PH;
    float t  = ( t1 < 0 ) ? ( t2 ) : ( t1 );
    vec3  P  = S + t * d;

    if ( fabs( t1 ) < 0.0005f || fabs( t2 ) < 0.0005f )
        return res;

    res.isHit           = true;
    res.distance        = t;
    res.hitPoint        = P;
    res.material        = material;
    res.material.normal = normalize( P - center );
    return res;
}