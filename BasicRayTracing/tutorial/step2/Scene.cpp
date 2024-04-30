#include "Scene.h"

Vec3f cast_ray( const Vec3f& orig, const Vec3f& dir, const Sphere& sphere )
{
    float sphere_dist = std::numeric_limits< float >::max();
    if ( !sphere.ray_intersect( orig, dir, sphere_dist ) )
    {
        return Vec3f( 0.2, 0.2, 0.2 );
    }

    return Vec3f( 0.8, 0.8, 0.8 );
}