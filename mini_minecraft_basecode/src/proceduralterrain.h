#ifndef PROCEDURALTERRAIN_H
#define PROCEDURALTERRAIN_H
#include <map>
#include <tuple>
#include <cmath>
#include <iostream>

using namespace std;
enum blocktype{EMPTY = -1,DIRT = 0, GRASS = 1, LAVA = 2, STONE = 3, WOOD = 4, LEAF = 5, BEDROCK = 6, COAL = 7, IRONORE = 8, WATER = 9};
class ProceduralTerrain
{
public:
    map<tuple<int, int, int>, blocktype> mapWorld;
    map<tuple<int, int, int>, blocktype> mapCave;
    tuple<int, int, int> startPosition;

    ProceduralTerrain();
    float Noise_2D(int x, int z);// The basic 2D noise function
    float Noise_3D_xz(int x, int y, int z);// The basic 3D noise function for generating the horizontal angle
    float Noise_3D_y(int x, int y, int z);// The basic 3D noise function for generating the vertical angle
    float SmoothNoise_2D(int x,int y);// 2D Smooth function
    float SmoothNoise_3D_xz(int x,int y,int z);// 3D Smooth function
    float SmoothNoise_3D_y(int x,int y,int z);// another 3D Smooth function

    double Interpolate_2D(float a0,float a1,float w);// Cosine Interpolation function
    float InterpolatedNoise_2D(float x,float y);// 2D Interpolate and Smooth
    float InterpolatedNoise_3D_xz(float x,float y, float z);// 3D Interpolate and Smooth
    float InterpolatedNoise_3D_y(float x,float y, float z);//Another 3D Interpolate and Smooth

    int PerlinNoise_2D(float x, float z);// Perlin Noise function
    float PerlinNoise_3D_xz(float x,float y,float z);//3D Perlin Noise funtion ,return a radius
    float PerlinNoise_3D_y(float x,float y,float z);//Another 3D Perlin Noise funtion ,return a radius

    void createInitialWorld();// Create the initial 64 * 64 * 64 World
    bool searchBlockAt(int x, int y, int z);// Search a block at a specific position
    void addBlockAt(int x, int y, int z);// Add a block at a specific position
    void deleteBlockAt(int x, int y, int z);// Delete a block at a specific position
    void addNewChunk(int x, int z);// Add a new chunk start from x, z

    void CreateTunnel(int x,int y,int z,float radius);//Remove a bunch of blocks around (x,y,z)
    void CreateEllipsoidcave(int x,int y,int z);
    void CaveGenerator();//Generate caves after the terrain has generated
};

#endif // PROCEDURALTERRAIN_H
