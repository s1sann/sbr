#include "Scene.h"
#include "geometry.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#define M_PI 3.14159265358979323846

void render(const std::vector<Sphere>& spheres, const std::vector< Light >& lights )
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

    Material yuhong( Vec3f( 0.75, 0.28, 0.32 ) );
    Material xinlan( Vec3f( 0.58, 0.71, 0.81 ) );
    Material cuilv( Vec3f( 0.13, 0.49, 0.38 ) );
    Material rubai( Vec3f( 0.98, 0.96, 0.86 ) );

    std::vector< Sphere > spheres;
    spheres.push_back( Sphere( Vec3f( -3, 0, -16 ), 2, yuhong) );
    spheres.push_back( Sphere( Vec3f( -1.0, -1.5, -12 ), 2, xinlan) );
    spheres.push_back( Sphere( Vec3f( 1.5, -0.5, -18 ), 3, cuilv) );
    spheres.push_back( Sphere( Vec3f( 7, 5, -18 ), 4, rubai) );

    std::vector< Light > lights;
    lights.push_back( Light( Vec3f( -20, 20, 20 ), 1.5 ) );

    render( spheres, lights );

    auto                            end  = std::chrono::high_resolution_clock::now();
    std::chrono::duration< double > diff = end - start;
    std::cout << "take: " << diff.count() << " s\n";

    return 0;
}
