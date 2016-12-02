#include "proceduralterrain.h"
#include "time.h"
ProceduralTerrain::ProceduralTerrain()
{
    srand(time(NULL));
}

// The basic noise function
float ProceduralTerrain::Noise(int x, int z)
{
    int n;//?
    n = x + z * 57;
    n = (n<<13) ^ n;
    return (( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0)) / 2.0 + 0.5;
}
float ProceduralTerrain::SmoothNoise_1(int x,int y)
{
    float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
    float sides = ( Noise(x-1, y) +Noise(x+1, y) +Noise(x, y-1) +Noise(x, y+1) ) / 8;
    float center = Noise(x, y) / 4;
    return corners + sides + center;
}
float ProceduralTerrain::InterpolatedNoise_1(float x, float y)
{    
    int integer_X = int(x>0?x:x-1);
    float fractional_X = x - integer_X;
    int integer_Y = int(y>0?y:y-1);
    float fractional_Y = y - integer_Y;

    float v1 = SmoothNoise_1(integer_X, integer_Y);
    float v2 = SmoothNoise_1(integer_X + 1, integer_Y);
    float v3 = SmoothNoise_1(integer_X, integer_Y + 1);
    float v4 = SmoothNoise_1(integer_X + 1, integer_Y + 1);

    float i1 = lerp(v1 , v2 , fractional_X);
    float i2 = lerp(v3 , v4 , fractional_X);

    return lerp(i1 , i2 , fractional_Y);
}
// Perlin Noise function
int ProceduralTerrain::PerlinNoise(float x, float z)
{
    x+=0.37;
    z+=0.37;
    x /= 10.0f;
    z /= 10.0f;
    float y = 0;
    float p = 1.0/4.0;// Persistence
    int n = 1;// Octave
    float f = 1.0;// Frequency
    float A = 64.0;// Amplitude
    int result;
    for(int i = 0; i < n; ++i)
    {
        f = pow(2, 1.0*i);
        A = pow(p, 1.0*i);
        y = y + A * InterpolatedNoise_1(f * x, f * z);
    }
    result = (y * 32.0);
    return result;
}

// Create the initial 64 * 64 * 64 World
void ProceduralTerrain::createInitialWorld()
{
    int y;
    for(int x = 0; x < 64; ++x)
    {
        for(int z = 0; z < 64; ++z)
        {
            y = PerlinNoise(1.0 * x,1.0 * z);
            for(int i = 0; i <= y; ++i)
            {
                tuple<int, int, int> position(x, i, z);
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, WATER));
            }
        }
    }
}

// Search a block at a specific position
bool ProceduralTerrain::searchBlockAt(int x, int y, int z)
{
    tuple<int, int, int> position(x, y, z);
    map<tuple<int, int, int>, blocktype>::iterator iter;
    iter = mapWorld.find(position);
    if(iter == mapWorld.end())
        return false;
    else
        return true;
}

// Add a block at a specific position
void ProceduralTerrain::addBlockAt(int x, int y, int z)
{
    tuple<int, int, int> position(x, y, z);
    mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, WATER));
}

// Delete a block at a specific position
void ProceduralTerrain::deleteBlockAt(int x, int y, int z)
{
    tuple<int, int, int> position(x, y, z);
    map<tuple<int, int, int>, blocktype>::iterator iter;
    iter = mapWorld.find(position);
    mapWorld.erase(iter);
}

// Add a new chunk start from (x, z)
void ProceduralTerrain::addNewChunk(int x, int z)
{
    int y;
    for(int i = 0; i < 16; ++i)
        for(int j = 0; j < 16; ++j)
        {
            y = PerlinNoise(1.0 * (x+i),1.0 * (z+j));
            for(int k = 0; k <= y; ++k)
            {
                tuple<int, int, int> position(x +i , k, z + j);
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, WATER));
            }
        }

}
//==========================================
double ProceduralTerrain::lerp(float a0, float a1, float w)
{
    float ft = w * 3.14159265;
    float f  = (1-cos(ft)) * 0.5;
    return a0 * (1-f) + a1 * f;
    //return (1.0 - w) * a0 + a1;
}
