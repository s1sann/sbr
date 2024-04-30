#include "geometry.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

void render()
{
    const int            width  = 1024;
    const int            height = 768;
    std::vector< Vec3f > framebuffer( width * height );

    for ( size_t j = 0; j < height; j++ )
    {
        for ( size_t i = 0; i < width; i++ )
        {
            framebuffer[ i + j * width ] = Vec3f( j / float( height ), i / float( width ), 0 );
        }
    }

    std::ofstream ofs;
    ofs.open("./out.ppm", std::ofstream::out | std::ofstream::binary);
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

    render();

    auto                            end  = std::chrono::high_resolution_clock::now();
    std::chrono::duration< double > diff = end - start;
    std::cout << "take: " << diff.count() << " s\n";

    return 0;
}
