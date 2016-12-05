#ifndef PROCEDURALTERRAIN_H
#define PROCEDURALTERRAIN_H
#include <map>
#include <tuple>
#include <cmath>
#include <iostream>

using namespace std;
enum blocktype{dirt = 0, grass = 1, lava = 2, WATER = 3};

class ProceduralTerrain
{
public:
    map<tuple<int, int, int>, blocktype> mapWorld;
    tuple<int, int, int> startPosition;

    ProceduralTerrain();
    float Noise(int x, int z);// The basic noise function
    float SmoothNoise_1(int x,int y);
    float InterpolatedNoise_1(float x,float y);

    int PerlinNoise(float x, float z);// Perlin Noise function

    void createInitialWorld();// Create the initial 64 * 64 * 64 World
    bool searchBlockAt(int x, int y, int z);// Search a block at a specific position
    void addBlockAt(int x, int y, int z);// Add a block at a specific position
    void deleteBlockAt(int x, int y, int z);// Delete a block at a specific position
    void addNewChunk(int x, int z);// Add a new chunk start from x, z


    //Here are code from wikipedia about 2-D noise
    //Abandoned. Only lerp is still in use
    double lerp(float a0,float a1,float w);
//    double dotGridGradient(int ix,int iy,float x,float y);
//    double PL_New(float x,float y);
};

#endif // PROCEDURALTERRAIN_H
