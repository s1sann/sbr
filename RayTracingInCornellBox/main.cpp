#include "Scene.h"
#include "svpng.inc"
#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>
#include <chrono>

using namespace std;

const int WIDTH  = 256;
const int HEIGHT = 256;

const int SAMPLE = 4096;

const double BRIGHTNESS = ( 2.0f * 3.1415926f ) * ( 1.0f / double( SAMPLE ) );

const double SCREEN_Z = 1.1;
const vec3   EYE      = vec3( 0, 0, 4.0 );

const vec3 RED( 1, 0.0, 0.0 );
const vec3 GREEN( 0.0, 1, 0.0 );
const vec3 BLUE( 0.0, 0.0, 1 );

const vec3 YELLOW( 1.0, 1.0, 0.1 );
const vec3 CYAN( 0.1, 1.0, 1.0 );
const vec3 MAGENTA( 1.0, 0.1, 1.0 );
const vec3 GRAY( 0.5, 0.5, 0.5 );
const vec3 WHITE( 1, 1, 1 );

void draw( double* SRC ) {
    unsigned char* image = new unsigned char[ WIDTH * HEIGHT * 3 ];
    unsigned char* p     = image;
    double*        S     = SRC;

    FILE* fp;
    fopen_s( &fp, "image.png", "wb" );

#pragma omp parallel for
    for ( int i = 0; i < HEIGHT; i++ )
        for ( int j = 0; j < WIDTH; j++ ) {
            *p++ = ( unsigned char )clamp( pow( *S++, 1.0f / 2.2f ) * 255, 0.0, 255.0 );
            *p++ = ( unsigned char )clamp( pow( *S++, 1.0f / 2.2f ) * 255, 0.0, 255.0 );
            *p++ = ( unsigned char )clamp( pow( *S++, 1.0f / 2.2f ) * 255, 0.0, 255.0 );
        }

    svpng( fp, WIDTH, HEIGHT, image, 0 );
}

HitResult shoot( vector< Shape* >& shapes, Ray ray ) {
    HitResult res, r;
    res.distance = 1145141919.810f;

    for ( auto& shape : shapes ) {
        r = shape->intersect( ray );
        if ( r.isHit && r.distance < res.distance )
            res = r;
    }

    return res;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    vector< Shape* > shapes;
    shapes.push_back( new Triangle( vec3( -0.5, -0.5, 0 ), vec3( 0.0, 0.5, 0 ), vec3( 0.5, -0.5, 0 ), RED ) );

    double* image = new double[ WIDTH * HEIGHT * 3 ];
    memset( image, 0.0, sizeof( double ) * WIDTH * HEIGHT * 3 );
    double* p = image;
#pragma omp parallel for
    for ( int i = 0; i < HEIGHT; i++ )
        for ( int j = 0; j < WIDTH; j++ ) {
            double x = 2.0 * double( j ) / double( WIDTH ) - 1.0;
            double y = 2.0 * double( HEIGHT - i ) / double( HEIGHT ) - 1.0;

            vec3 coord     = vec3( x, y, SCREEN_Z );
            vec3 direction = normalize( coord - EYE );

            Ray ray;
            ray.startPoint = coord;
            ray.direction  = direction;

            HitResult res   = shoot( shapes, ray );
            vec3      color = res.material.color;

            *p = color.x;
            p++;
            *p = color.y;
            p++;
            *p = color.z;
            p++;
        }

    draw( image );

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;

    std::cout << "take: " << diff.count() << " s\n";

    return 0;
}
