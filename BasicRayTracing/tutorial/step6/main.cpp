#include "Scene.h"
#include "geometry.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#define M_PI 3.14159265358979323846

void render( const std::vector< Sphere >& spheres, const std::vector< Light >& lights )
{
    const int            width  = 1024;
    const int            height = 768;
    const int            fov    = M_PI / 2.;
    std::vector< Vec3f > framebuffer( width * height );

#pragma omp parallel for
    for ( size_t j = 0; j < height; j++ )
    {
        for ( size_t i = 0; i < width; i++ )
        {
            float x                      = ( 2 * ( i + 0.5 ) / ( float )width - 1 ) * tan( fov / 2. ) * width / ( float )height;
            float y                      = -( 2 * ( j + 0.5 ) / ( float )height - 1 ) * tan( fov / 2. );
            Vec3f dir                    = Vec3f( x, y, -1 ).normalize();
            framebuffer[ i + j * width ] = cast_ray( Vec3f( 0, 0, 0 ), dir, spheres, lights );
        }
    }

    std::ofstream ofs;
    ofs.open( "./out.ppm", std::ofstream::out | std::ofstream::binary );
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for ( size_t i = 0; i < height * width; ++i )
    {
        Vec3f& c   = framebuffer[ i ];
        float  max = std::max( c[ 0 ], std::max( c[ 1 ], c[ 2 ] ) );
        if ( max > 1 )
            c = c * ( 1. / max );
        for ( size_t j = 0; j < 3; j++ )
        {
            ofs << ( char )( 255 * std::max( 0.f, std::min( 1.f, framebuffer[ i ][ j ] ) ) );
        }
    }
    ofs.close();
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    Material yuhong( 1.0, Vec4f( 0.6, 0.3, 0.1, 0.0 ), Vec3f( 0.75, 0.28, 0.32 ), 50. );
    Material xinlan( 1.5, Vec4f( 0.5, 0.4, 0.0, 0.0 ), Vec3f( 0.58, 0.71, 0.81 ), 10. );
    Material cuilv( 1.0, Vec4f( 0.5, 0.1, 0.2, 0.0 ), Vec3f( 0.13, 0.49, 0.38 ), 20. );
    Material rubai( 1.0, Vec4f( 0.3, 0.1, 0.1, 0.0 ), Vec3f( 0.98, 0.96, 0.86 ), 30. );
    Material mirror( 1.0, Vec4f( 0.0, 10.0, 0.8, 0.0 ), Vec3f( 1.0, 1.0, 1.0 ), 1425. );
    Material glass( 1.5, Vec4f( 0.0, 0.5, 0.1, 0.8 ), Vec3f( 0.6, 0.7, 0.8 ), 125. );

    std::vector< Sphere > spheres;
    spheres.push_back( Sphere( Vec3f( -3, 0, -16 ), 2, yuhong ) );
    spheres.push_back( Sphere( Vec3f( -1.0, -1.5, -12 ), 2, glass ) );
    spheres.push_back( Sphere( Vec3f( 1.5, -0.5, -18 ), 3, cuilv ) );
    spheres.push_back( Sphere( Vec3f( 7, 5, -18 ), 4, mirror ) );

    std::vector< Light > lights;
    lights.push_back( Light( Vec3f( -20, 20, 20 ), 1.5 ) );
    lights.push_back( Light( Vec3f( 30, 50, -25 ), 1.8 ) );
    lights.push_back( Light( Vec3f( 30, 20, 30 ), 1.7 ) );

    render( spheres, lights );

    auto                            end  = std::chrono::high_resolution_clock::now();
    std::chrono::duration< double > diff = end - start;
    std::cout << "take: " << diff.count() << " s\n";

    return 0;
}
