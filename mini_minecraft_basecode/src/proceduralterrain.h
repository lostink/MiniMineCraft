#ifndef PROCEDURALTERRAIN_H
#define PROCEDURALTERRAIN_H
#include <map>
#include <tuple>
#include <cmath>

using namespace std;
enum blocktype{dirt = 0, grass = 1, lava = 2};

class ProceduralTerrain
{
public:
    map<tuple<int, int, int>, blocktype> mapWorld;
    tuple<int, int, int> startPosition;

    ProceduralTerrain();
    float Noise(int x, int z);// The basic noise function
    double lerp(float a0,float a1,float w);// Linear Interpolation
    float SmoothNoise_1(int x,int y);// Smooth the noise function
    float InterpolatedNoise_1(float x,float y);//Interpolate the noise function

    int PerlinNoise(float x, float z);// Perlin Noise function

    void createInitialWorld();// Create the initial 64 * 64 * 64 World
    bool searchBlockAt(int x, int y, int z);// Search a block at a specific position
    void addBlockAt(int x, int y, int z);// Add a block at a specific position
    void deleteBlockAt(int x, int y, int z);// Delete a block at a specific position
    void addNewChunk(int x, int z);// Add a new chunk start from x, z


};
#endif // PROCEDURALTERRAIN_H
