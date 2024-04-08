#include "Scene.h"
#include "svpng.inc"
#include <chrono>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>
#include <omp.h>

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

void draw( double* SRC )
{
    unsigned char* image = new unsigned char[ WIDTH * HEIGHT * 3 ];
    unsigned char* p     = image;
    double*        S     = SRC;

    FILE* fp;
    fopen_s( &fp, "image.png", "wb" );

#pragma omp parallel for
    for ( int i = 0; i < HEIGHT; i++ )
        for ( int j = 0; j < WIDTH; j++ )
        {
            *p++ = ( unsigned char )clamp( pow( *S++, 1.0f / 2.2f ) * 255, 0.0, 255.0 );
            *p++ = ( unsigned char )clamp( pow( *S++, 1.0f / 2.2f ) * 255, 0.0, 255.0 );
            *p++ = ( unsigned char )clamp( pow( *S++, 1.0f / 2.2f ) * 255, 0.0, 255.0 );
        }

    svpng( fp, WIDTH, HEIGHT, image, 0 );
}

HitResult shoot( vector< Shape* >& shapes, Ray ray )
{
    HitResult res, r;
    res.distance = 1145141919.810f;

    for ( auto& shape : shapes )
    {
        r = shape->intersect( ray );
        if ( r.isHit && r.distance < res.distance )
            res = r;
    }

    return res;
}

std::uniform_real_distribution<> dis( 0.0, 1.0 );
random_device                    rd;
mt19937                          gen( rd() );
double                           randf()
{
    return dis( gen );
}

vec3 randomVec3()
{
    vec3 d;
    do
    {
        d = 2.0f * vec3( randf(), randf(), randf() ) - vec3( 1, 1, 1 );
    } while ( dot( d, d ) > 1.0 );
    return normalize( d );
}

// 法向半球随机向量
vec3 randomDirection( vec3 n )
{
    /*
    // 法向半球
    vec3 d;
    do
    {
        d = randomVec3();
    } while (dot(d, n) < 0.0f);
    return d;
    */
    // 法向球
    return normalize( randomVec3() + n );
}

// 路径追踪
vec3 pathTracing( vector< Shape* >& shapes, Ray ray, int depth )
{
    if ( depth > 8 )
        return vec3( 0 );
    HitResult res = shoot( shapes, ray );

    if ( !res.isHit )
        return vec3( 0 );  // 未命中

    // 如果发光则返回颜色
    if ( res.material.isEmissive )
        return res.material.color;

    // 有 P 的概率终止
    double r = randf();
    float  P = 0.8;
    if ( r > P )
        return vec3( 0 );

    // 否则继续
    Ray randomRay;
    randomRay.startPoint = res.hitPoint;
    randomRay.direction  = randomDirection( res.material.normal );

    vec3  color  = vec3( 0 );
    float cosine = fabs( dot( -ray.direction, res.material.normal ) );

    // 根据反射率决定光线最终的方向
    r = randf();
    if ( r < res.material.specularRate )  // 镜面反射
    {
        vec3 ref            = normalize( reflect( ray.direction, res.material.normal ) );
        randomRay.direction = mix( ref, randomRay.direction, res.material.roughness );
        color               = pathTracing( shapes, randomRay, depth + 1 ) * cosine;
    }
    else if ( res.material.specularRate <= r && r <= res.material.refractRate )  // 折射
    {
        vec3 ref            = normalize( refract( ray.direction, res.material.normal, float( res.material.refractAngle ) ) );
        randomRay.direction = mix( ref, -randomRay.direction, res.material.refractRoughness );
        color               = pathTracing( shapes, randomRay, depth + 1 ) * cosine;
    }
    else  // 漫反射
    {
        vec3 srcColor = res.material.color;
        vec3 ptColor  = pathTracing( shapes, randomRay, depth + 1 ) * cosine;
        color         = ptColor * srcColor;  // 和原颜色混合
    }

    return color / P;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    vector< Shape* > shapes;  // 几何物体的集合

    Sphere s1                = Sphere( vec3( -0.65, -0.7, 0.0 ), 0.3, GREEN );
    Sphere s2                = Sphere( vec3( 0.0, -0.3, 0.0 ), 0.4, WHITE );
    Sphere s3                = Sphere( vec3( 0.65, 0.1, 0.0 ), 0.3, BLUE );
    s1.material.specularRate = 0.3;
    s1.material.roughness    = 0.1;

    s2.material.specularRate = 0.3;
    s2.material.refractRate  = 0.95;
    s2.material.refractAngle = 0.1;
    // s2.material.refractRoughness = 0.05;

    s3.material.specularRate = 0.3;

    shapes.push_back( &s1 );
    shapes.push_back( &s2 );
    shapes.push_back( &s3 );

    shapes.push_back( new Triangle( vec3( -0.15, 0.4, -0.6 ), vec3( -0.15, -0.95, -0.6 ), vec3( 0.15, 0.4, -0.6 ), YELLOW ) );
    shapes.push_back( new Triangle( vec3( 0.15, 0.4, -0.6 ), vec3( -0.15, -0.95, -0.6 ), vec3( 0.15, -0.95, -0.6 ), YELLOW ) );

    Triangle tt = Triangle( vec3( -0.2, -0.2, -0.95 ), vec3( 0.2, -0.2, -0.95 ), vec3( -0.0, -0.9, 0.4 ), YELLOW );
    // tt.material.specularRate = 0.1;
    // tt.material.refractRate = 0.85;
    // tt.material.refractRoughness = 0.3;
    // shapes.push_back(&tt);

    // 发光物
    Triangle l1            = Triangle( vec3( 0.4, 0.99, 0.4 ), vec3( -0.4, 0.99, -0.4 ), vec3( -0.4, 0.99, 0.4 ), WHITE );
    Triangle l2            = Triangle( vec3( 0.4, 0.99, 0.4 ), vec3( 0.4, 0.99, -0.4 ), vec3( -0.4, 0.99, -0.4 ), WHITE );
    l1.material.isEmissive = true;
    l2.material.isEmissive = true;
    shapes.push_back( &l1 );
    shapes.push_back( &l2 );

    // 背景盒子
    // bottom
    shapes.push_back( new Triangle( vec3( 1, -1, 1 ), vec3( -1, -1, -1 ), vec3( -1, -1, 1 ), WHITE ) );
    shapes.push_back( new Triangle( vec3( 1, -1, 1 ), vec3( 1, -1, -1 ), vec3( -1, -1, -1 ), WHITE ) );
    // top
    shapes.push_back( new Triangle( vec3( 1, 1, 1 ), vec3( -1, 1, 1 ), vec3( -1, 1, -1 ), WHITE ) );
    shapes.push_back( new Triangle( vec3( 1, 1, 1 ), vec3( -1, 1, -1 ), vec3( 1, 1, -1 ), WHITE ) );
    // back
    shapes.push_back( new Triangle( vec3( 1, -1, -1 ), vec3( -1, 1, -1 ), vec3( -1, -1, -1 ), CYAN ) );
    shapes.push_back( new Triangle( vec3( 1, -1, -1 ), vec3( 1, 1, -1 ), vec3( -1, 1, -1 ), CYAN ) );
    // left
    shapes.push_back( new Triangle( vec3( -1, -1, -1 ), vec3( -1, 1, 1 ), vec3( -1, -1, 1 ), BLUE ) );
    shapes.push_back( new Triangle( vec3( -1, -1, -1 ), vec3( -1, 1, -1 ), vec3( -1, 1, 1 ), BLUE ) );
    // right
    shapes.push_back( new Triangle( vec3( 1, 1, 1 ), vec3( 1, -1, -1 ), vec3( 1, -1, 1 ), RED ) );
    shapes.push_back( new Triangle( vec3( 1, -1, -1 ), vec3( 1, 1, 1 ), vec3( 1, 1, -1 ), RED ) );

    double* image = new double[ WIDTH * HEIGHT * 3 ];
    memset( image, 0.0, sizeof( double ) * WIDTH * HEIGHT * 3 );

    omp_set_num_threads(50); // 线程个数
#pragma omp parallel for
    for ( int k = 0; k < SAMPLE; k++ )
    {
        double* p = image;
        for ( int i = 0; i < HEIGHT; i++ )
        {
            for ( int j = 0; j < WIDTH; j++ )
            {
                // 像素坐标转投影平面坐标
                double x = 2.0 * double(j) / double(WIDTH) - 1.0;
                double y = 2.0 * double(HEIGHT - i) / double(HEIGHT) - 1.0;

                // MSAA
                x += (randf() - 0.5f) / double(WIDTH);
                y += (randf() - 0.5f) / double(HEIGHT);

                vec3 coord = vec3(x, y, SCREEN_Z);          // 计算投影平面坐标
                vec3 direction = normalize(coord - EYE);    // 计算光线投射方向

                // 生成光线
                Ray ray;
                ray.startPoint = coord;
                ray.direction = direction;

                // 与场景的交点
                HitResult res = shoot(shapes, ray);
                vec3 color = vec3(0, 0, 0);

                if (res.isHit)
                {
                    // 命中光源直接返回光源颜色
                    if (res.material.isEmissive)
                    {
                        color = res.material.color;
                    }
                    // 命中实体则选择一个随机方向重新发射光线并且进行路径追踪
                    else
                    {
                        // 根据交点处法向量生成交点处反射的随机半球向量
                        Ray randomRay;
                        randomRay.startPoint = res.hitPoint;
                        randomRay.direction = randomDirection(res.material.normal);

                        // 根据反射率决定光线最终的方向
                        double r = randf();
                        if (r < res.material.specularRate)  // 镜面反射
                        {
                            vec3 ref = normalize(reflect(ray.direction, res.material.normal));
                            randomRay.direction = mix(ref, randomRay.direction, res.material.roughness);
                            color = pathTracing(shapes, randomRay, 0);
                        }
                        else if (res.material.specularRate <= r && r <= res.material.refractRate)    // 折射
                        {
                            vec3 ref = normalize(refract(ray.direction, res.material.normal, float(res.material.refractAngle)));
                            randomRay.direction = mix(ref, -randomRay.direction, res.material.refractRoughness);
                            color = pathTracing(shapes, randomRay, 0);
                        }
                        else    // 漫反射
                        {
                            vec3 srcColor = res.material.color;
                            vec3 ptColor = pathTracing(shapes, randomRay, 0);
                            color = ptColor * srcColor;    // 和原颜色混合
                        }
                        color *= BRIGHTNESS;
                    }
                }

                *p += color.x; p++;  // R 通道
                *p += color.y; p++;  // G 通道
                *p += color.z; p++;  // B 通道
            }
        }
    }

    draw( image );

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration< double > diff = end - start;

    std::cout << "take: " << diff.count() << " s\n";

    return 0;
}
