#include "Scene.h"

Vec3f reflect( const Vec3f& I, const Vec3f& N )
{
    return I - N * 2.f * ( I * N );
}

Vec3f refract( const Vec3f& I, const Vec3f& N, const float& refractive_index )
{
    float cosi = -std::max( -1.f, std::min( 1.f, I * N ) );
    float etai = 1, etat = refractive_index;
    Vec3f n = N;
    if ( cosi < 0 )
    {
        cosi = -cosi;
        std::swap( etai, etat );
        n = -N;
    }
    float eta = etai / etat;
    float k   = 1 - eta * eta * ( 1 - cosi * cosi );
    return k < 0 ? Vec3f( 0, 0, 0 ) : I * eta + n * ( eta * cosi - sqrtf( k ) );
}

bool scene_intersect( const Vec3f& orig, const Vec3f& dir, const std::vector< Sphere >& spheres, Vec3f& hit, Vec3f& N, Material& material )
{
    float spheres_dist = std::numeric_limits< float >::max();
    for ( size_t i = 0; i < spheres.size(); i++ )
    {
        float dist_i;
        if ( spheres[ i ].ray_intersect( orig, dir, dist_i ) && dist_i < spheres_dist )
        {
            spheres_dist = dist_i;
            hit          = orig + dir * dist_i;
            N            = ( hit - spheres[ i ].center ).normalize();
            material     = spheres[ i ].material;
        }
    }
    return spheres_dist < 1000;
}

Vec3f cast_ray( const Vec3f& orig, const Vec3f& dir, const std::vector< Sphere >& spheres, const std::vector< Light >& lights, size_t depth )
{
    Vec3f    point, N;
    Material material;

    if ( depth > 4 || !scene_intersect( orig, dir, spheres, point, N, material ) )
    {
        return Vec3f( 0.98, 0.96, 0.86 );
    }

    Vec3f reflect_dir = reflect( dir, N ).normalize();
    Vec3f refract_dir = refract( dir, N, material.refractive_index ).normalize();

    Vec3f reflect_orig = reflect_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
    Vec3f refract_orig = refract_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;

    Vec3f reflect_color = cast_ray( reflect_orig, reflect_dir, spheres, lights, depth + 1 );
    Vec3f refract_color = cast_ray( refract_orig, refract_dir, spheres, lights, depth + 1 );

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for ( size_t i = 0; i < lights.size(); i++ )
    {
        Vec3f light_dir = ( lights[ i ].position - point ).normalize();

        float light_distance = ( lights[ i ].position - point ).norm();

        Vec3f    shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
        Vec3f    shadow_pt, shadow_N;
        Material tmpmaterial;
        if ( scene_intersect( shadow_orig, light_dir, spheres, shadow_pt, shadow_N, tmpmaterial ) && ( shadow_pt - shadow_orig ).norm() < light_distance )
            continue;

        diffuse_light_intensity += lights[ i ].intensity * std::max( 0.f, light_dir * N );
        specular_light_intensity += powf( std::max( 0.f, -reflect( -light_dir, N ) * dir ), material.specular_exponent ) * lights[ i ].intensity;
    }

    return material.diffuse_color * diffuse_light_intensity * material.albedo[ 0 ] + Vec3f( 1., 1., 1. ) * specular_light_intensity * material.albedo[ 1 ] + reflect_color * material.albedo[ 2 ]
           + refract_color * material.albedo[ 3 ];
}