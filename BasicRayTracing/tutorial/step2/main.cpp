#include "Scene.h"
#include "geometry.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#define M_PI 3.14159265358979323846

void render( const Sphere& sphere )
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
            framebuffer[ i + j * width ] = cast_ray( Vec3f( 0, 0, 0 ), dir, sphere );
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

    Sphere sphere( Vec3f( 1, 2, -16 ), 2 );
    render( sphere );

    auto                            end  = std::chrono::high_resolution_clock::now();
    std::chrono::duration< double > diff = end - start;
    std::cout << "take: " << diff.count() << " s\n";

    return 0;
}
