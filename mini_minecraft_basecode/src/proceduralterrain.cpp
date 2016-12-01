#include "proceduralterrain.h"
#include "time.h"
ProceduralTerrain::ProceduralTerrain()
{
    srand(time(NULL));
}

// The basic noise function
float ProceduralTerrain::Noise(int x, int z)
{
    int n;
    n = x + z * 57;
    n = (n<<13) ^ n;
    return (( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0));
}

// Linear Interpolation
double ProceduralTerrain::lerp(float a0, float a1, float w)
{
    return (1.0 - w) * a0 + a1;
}

// Smooth the noise function
float ProceduralTerrain::SmoothNoise_1(int x,int y)
{
    float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
    float sides = ( Noise(x-1, y) +Noise(x+1, y) +Noise(x, y-1) +Noise(x, y+1) ) / 8;
    float center = Noise(x, y) / 4;
    return corners + sides + center;
}

//Interpolate the noise function
float ProceduralTerrain::InterpolatedNoise_1(float x, float y)
{
    int integer_X = int(x);
    float fractional_X = x - integer_X;
    int integer_Y = int(y);
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
    float y = 0;
    float p = 1.0/4.0;// Persistence
    int n = 5;// Octave
    float f = 1.0;// Frequency
    float A = 64.0;// Amplitude
    int result;
    for(int i = 0; i < n; ++i)
    {
        f = pow(2, 1.0*i);
        A = pow(p, 1.0*i);
        //printf("%lf %lf\n",f,A);
        y = y + A * InterpolatedNoise_1(f * x, f * z);
        //printf("%f\n", y);
    }
    y /= n;
    result = int((y + 1) * 32.0);
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
            y = PerlinNoise(1.0 * x / 10.0,1.0 * z / 10.0);

            for(int i = 0; i <= y; ++i)
            {
                tuple<int, int, int> position(x, i, z);
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, dirt));
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
    mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, dirt));
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
            y = PerlinNoise(1.0 * (x+i) / 10.0,1.0 * (z+j) / 10.0);
            for(int k = 0; k <= y; ++k)
            {
                tuple<int, int, int> position(x, k, z);
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, dirt));
            }
        }

}
