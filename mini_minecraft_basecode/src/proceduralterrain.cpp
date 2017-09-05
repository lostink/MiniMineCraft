#include "proceduralterrain.h"
#include "time.h"
ProceduralTerrain::ProceduralTerrain()
{
    srand(time(NULL));
}

// The basic 2D noise function
float ProceduralTerrain::Noise_2D(int x, int z)
{
    int n;
    n = x + z * 57;
    n = (n<<13) ^ n;
    return (( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0)) / 2.0 + 0.5;
}

// The basic 3D noise function for generating the horizontal angle
float ProceduralTerrain::Noise_3D_xz(int x, int y, int z)
{
    int n;
    n = x + y + z * 57;
    n = (n<<13) ^ n;
    return (( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0)) / 2.0 + 0.5;
}

// The basic 3D noise function for generating the vertical angle
float ProceduralTerrain::Noise_3D_y(int x, int y, int z)
{
    int n;
    n = x + y + z * 57;
    n = (n<<13) ^ n;
    return (( 1.0 - ( (n * (n * n * 24919 + 671249) + 1376312589) & 0x7fffffff) / 1073741824.0)) / 2.0 + 0.5;
}

// 2D Smooth function
float ProceduralTerrain::SmoothNoise_2D(int x,int y)
{
    float corners = ( Noise_2D(x-1, y-1)+Noise_2D(x+1, y-1)+Noise_2D(x-1, y+1)+Noise_2D(x+1, y+1) ) / 16;
    float sides = ( Noise_2D(x-1, y) +Noise_2D(x+1, y) +Noise_2D(x, y-1) +Noise_2D(x, y+1) ) / 8;
    float center = Noise_2D(x, y) / 4;
    return corners + sides + center;
}
//3D smooth function
float ProceduralTerrain::SmoothNoise_3D_xz(int x, int y, int z)
{
    float center = Noise_3D_xz(x,y,z) / 4;
    float side   = (Noise_3D_xz(x-1,y,z) + Noise_3D_xz(x+1,y,z)
                  +Noise_3D_xz(x,y-1,z) + Noise_3D_xz(x,y+1,z)
                  +Noise_3D_xz(x,y,z-1) + Noise_3D_xz(x,y,z+1)) / 24;
    float side_corner = (Noise_3D_xz(x+1,y+1,z) + Noise_3D_xz(x+1,y-1,z)
                       +Noise_3D_xz(x-1,y+1,z) + Noise_3D_xz(x-1,y-1,z)
                       +Noise_3D_xz(x+1,y,z+1) + Noise_3D_xz(x+1,y,z-1)
                       +Noise_3D_xz(x-1,y,z+1) + Noise_3D_xz(x-1,y,z-1)
                       +Noise_3D_xz(x,y+1,z+1) + Noise_3D_xz(x,y+1,z-1)
                       +Noise_3D_xz(x,y-1,z+1) + Noise_3D_xz(x,y-1,z-1)) / 36;
    float corner = (Noise_3D_xz(x+1,y+1,z+1) + Noise_3D_xz(x+1,y+1,z-1)
                  +Noise_3D_xz(x+1,y-1,z+1) + Noise_3D_xz(x+1,y-1,z-1)
                  +Noise_3D_xz(x-1,y+1,z+1) + Noise_3D_xz(x-1,y+1,z-1)
                  +Noise_3D_xz(x-1,y-1,z+1) + Noise_3D_xz(x-1,y-1,z-1)) / 96;
    return center + side + side_corner + corner;
}
//another 3D smooth function0
float ProceduralTerrain::SmoothNoise_3D_y(int x, int y, int z)
{
    float center = Noise_3D_y(x,y,z) / 4;
    float side   = (Noise_3D_y(x-1,y,z) + Noise_3D_y(x+1,y,z)
                  +Noise_3D_y(x,y-1,z) + Noise_3D_y(x,y+1,z)
                  +Noise_3D_y(x,y,z-1) + Noise_3D_y(x,y,z+1)) / 24;
    float side_corner = (Noise_3D_y(x+1,y+1,z) + Noise_3D_y(x+1,y-1,z)
                       +Noise_3D_y(x-1,y+1,z) + Noise_3D_y(x-1,y-1,z)
                       +Noise_3D_y(x+1,y,z+1) + Noise_3D_y(x+1,y,z-1)
                       +Noise_3D_y(x-1,y,z+1) + Noise_3D_y(x-1,y,z-1)
                       +Noise_3D_y(x,y+1,z+1) + Noise_3D_y(x,y+1,z-1)
                       +Noise_3D_y(x,y-1,z+1) + Noise_3D_y(x,y-1,z-1)) / 36;
    float corner = (Noise_3D_y(x+1,y+1,z+1) + Noise_3D_y(x+1,y+1,z-1)
                  +Noise_3D_y(x+1,y-1,z+1) + Noise_3D_y(x+1,y-1,z-1)
                  +Noise_3D_y(x-1,y+1,z+1) + Noise_3D_y(x-1,y+1,z-1)
                  +Noise_3D_y(x-1,y-1,z+1) + Noise_3D_y(x-1,y-1,z-1)) / 96;
    return center + side + side_corner + corner;
}
// Cosine Interpolation function
double ProceduralTerrain::Interpolate_2D(float a0, float a1, float w)
{
    float ft = w * 3.14159265;
    float f  = (1-cos(ft)) * 0.5;
    return a0 * (1-f) + a1 * f;
}

// 2D Interpolate and Smooth
float ProceduralTerrain::InterpolatedNoise_2D(float x, float y)
{    
    int integer_X = int(x>0?x:x-1);
    float fractional_X = x - integer_X;
    int integer_Y = int(y>0?y:y-1);
    float fractional_Y = y - integer_Y;

    float v1 = SmoothNoise_2D(integer_X, integer_Y);
    float v2 = SmoothNoise_2D(integer_X + 1, integer_Y);
    float v3 = SmoothNoise_2D(integer_X, integer_Y + 1);
    float v4 = SmoothNoise_2D(integer_X + 1, integer_Y + 1);

    float i1 = Interpolate_2D(v1 , v2 , fractional_X);
    float i2 = Interpolate_2D(v3 , v4 , fractional_X);

    return Interpolate_2D(i1 , i2 , fractional_Y);
}

// 3D Interpolate and Smooth
float ProceduralTerrain::InterpolatedNoise_3D_xz(float x,float y, float z)
{
    int integer_X = int(x>0?x:x-1);
    float fractional_X = x - integer_X;
    int integer_Y = int(y>0?y:y-1);
    float fractional_Y = y - integer_Y;
    int integer_Z = int(z>0?z:z-1);
    float fractional_Z = z - integer_Z;

    float v1 = SmoothNoise_3D_xz(integer_X, integer_Y, integer_Z);
    float v2 = SmoothNoise_3D_xz(integer_X + 1, integer_Y, integer_Z);
    float v3 = SmoothNoise_3D_xz(integer_X, integer_Y + 1,integer_Z);
    float v4 = SmoothNoise_3D_xz(integer_X + 1, integer_Y + 1,integer_Z);
    float v5 = SmoothNoise_3D_xz(integer_X, integer_Y, integer_Z+1);
    float v6 = SmoothNoise_3D_xz(integer_X + 1, integer_Y, integer_Z+1);
    float v7 = SmoothNoise_3D_xz(integer_X, integer_Y + 1,integer_Z+1);
    float v8 = SmoothNoise_3D_xz(integer_X + 1, integer_Y + 1,integer_Z+1);

    float i1 = Interpolate_2D(v1 , v2 , fractional_X);
    float i2 = Interpolate_2D(v3 , v4 , fractional_X);
    float i3 = Interpolate_2D(v5 , v6 , fractional_X);
    float i4 = Interpolate_2D(v7 , v8 , fractional_X);

    float j1 = Interpolate_2D(i1 , i2 , fractional_Y);
    float j2 = Interpolate_2D(i3 , i4 , fractional_Y);

    float k1 = Interpolate_2D(j1 , j2 , fractional_Z);

    return k1;
}

//Another 3D Interpolate and Smooth
float ProceduralTerrain::InterpolatedNoise_3D_y(float x,float y, float z)
{
    int integer_X = int(x>0?x:x-1);
    float fractional_X = x - integer_X;
    int integer_Y = int(y>0?y:y-1);
    float fractional_Y = y - integer_Y;
    int integer_Z = int(z>0?z:z-1);
    float fractional_Z = z - integer_Z;

    float v1 = SmoothNoise_3D_y(integer_X, integer_Y, integer_Z);
    float v2 = SmoothNoise_3D_y(integer_X + 1, integer_Y, integer_Z);
    float v3 = SmoothNoise_3D_y(integer_X, integer_Y + 1,integer_Z);
    float v4 = SmoothNoise_3D_y(integer_X + 1, integer_Y + 1,integer_Z);
    float v5 = SmoothNoise_3D_y(integer_X, integer_Y, integer_Z+1);
    float v6 = SmoothNoise_3D_y(integer_X + 1, integer_Y, integer_Z+1);
    float v7 = SmoothNoise_3D_y(integer_X, integer_Y + 1,integer_Z+1);
    float v8 = SmoothNoise_3D_y(integer_X + 1, integer_Y + 1,integer_Z+1);

    float i1 = Interpolate_2D(v1 , v2 , fractional_X);
    float i2 = Interpolate_2D(v3 , v4 , fractional_X);
    float i3 = Interpolate_2D(v5 , v6 , fractional_X);
    float i4 = Interpolate_2D(v7 , v8 , fractional_X);

    float j1 = Interpolate_2D(i1 , i2 , fractional_Y);
    float j2 = Interpolate_2D(i3 , i4 , fractional_Y);

    float k1 = Interpolate_2D(j1 , j2 , fractional_Z);
    return k1;
}

// 2D Perlin Noise function
int ProceduralTerrain::PerlinNoise_2D(float x, float z)
{
    x+=0.37;
    z+=0.37;
    x /= 10.0f;
    z /= 10.0f;
    float y = 0;
    float p = 1.0/4.0;// Persistence
    int n = 1;// Octave
    float f = 1.0;// Frequency
    float A;// Amplitude
    int result;
    for(int i = 0; i < n; ++i)
    {
        f = pow(2, 1.0*i);
        A = pow(p, 1.0*i);
        y = y + A * InterpolatedNoise_2D(f * x, f * z);
    }
    result = (y * 32.0);
    return result;
}
//3D Perlin Noise funtion
float ProceduralTerrain::PerlinNoise_3D_xz(float x, float y,float z)
{
    x+=0.37;
    y+=0.37;
    z+=0.37;

    x /= 10.0f;
    y /= 10.0f;
    z /= 10.0f;
    float angle = 0;
    float p = 1.0/4.0;// Persistence
    int n = 1;// Octave
    float f = 1.0;// Frequency
    float A;// Amplitude
    float result;
    for(int i = 0; i < n; ++i)
    {
        f = pow(2, 1.0*i);
        A = pow(p, 1.0*i);
        angle = angle + A * InterpolatedNoise_3D_xz(f * x, f * y, f * z);
    }
    result = (angle * 2 * 3.141592653589793f);
    return result;
}
//Another 3D Perlin Noise funtion
float ProceduralTerrain::PerlinNoise_3D_y(float x, float y,float z)
{
    x+=0.37;
    y+=0.37;
    z+=0.37;

    x /= 10.0f;
    y /= 10.0f;
    z /= 10.0f;
    float angle = 0;
    float p = 1.0/4.0;// Persistence
    int n = 1;// Octave
    float f = 1.0;// Frequency
    float A;// Amplitude
    float result;
    for(int i = 0; i < n; ++i)
    {
        f = pow(2, 1.0*i);
        A = pow(p, 1.0*i);
        angle = angle + A * InterpolatedNoise_3D_y(f * x, f * y, f * z);
    }
    result = (1 - angle) * 3.141592653589793f;
    return result;
}

float ProceduralTerrain::sinNoise(int x, int z)
{
    float result = (sin((x+z)/100.0))/2.0 + 0.5;
    return result;
}

void ProceduralTerrain::CreateInitialBiomeMap()
{
    // Initial world
    for(int x = 0; x < 64; x+=16)
        for(int z = 0; z < 64; z+=16)
        {
            pair<int, int> biomePos(x, z);
            mapBiome[biomePos] = DESERT;
        }
}

// Create the initial 64 * 192 * 64 World
void ProceduralTerrain::createInitialWorld()
{
    // DIRT: y from 0 to (ground surface - 1)
    // GRASS: y = ground surface
    // STONE: y from -127 to -1
    // BEDROCK: y = -128
    /*
    int y;
    for(int x = 0; x < 64; ++x)
    {
        for(int z = 0; z < 64; ++z)
        {
            y = PerlinNoise_2D(1.0 * x,1.0 * z);
            tuple<int, int, int> positiongrass(x, y, z);
            mapWorld.insert(pair<tuple<int, int, int>, blocktype>(positiongrass, GRASS));
            tuple<int, int, int> positionbedrock(x, -128, z);
            mapWorld.insert(pair<tuple<int, int, int>, blocktype>(positionbedrock, BEDROCK));
            for(int k = 0; k < y; ++k)
            {
                tuple<int, int, int> position(x, k, z);
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, DIRT));
            }
            for(int k = -127; k < 0; ++k)
            {
                tuple<int, int, int> position(x, k, z);
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, STONE));
            }
        }
    }
    */
    for (int i=0;i<4;++i)
        for (int j=0;j<4;++j)
            addNewChunk(i*16,j*16);
    //CreateInitialBiomeMap();
    // Generate cave
    CaveGenerator();
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
    mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, DIRT));
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
void HeightGenerate(float &height,float value){
    if (value <= 0.2)
        height = 0.5;
    else if (value >= 0.8)
        height = 2;
    else height = 1;
}
void ProceduralTerrain::addNewChunk(int x, int z)
{
    pair<int, int> biomePos(x, z);
    float flag = sinNoise(x, z);
    if(flag <= 0.2)
        mapBiome.insert(pair<pair<int, int>, biometype>(biomePos, DESERT));
    else if((flag > 0.2)&&(flag <= 0.4))
        mapBiome.insert(pair<pair<int, int>, biometype>(biomePos, FOREST));
    else if((flag > 0.4)&&(flag <= 0.6))
        mapBiome.insert(pair<pair<int, int>, biometype>(biomePos, SWAMP));
    else if(flag > 0.6)
        mapBiome.insert(pair<pair<int, int>, biometype>(biomePos, PLAIN));

    int y;

    biometype BIO_NOW = mapBiome[biomePos];
    //First, find the height
    for(int i = 0; i < 16; ++i)
        for(int j = 0; j < 16; ++j)
        {
            y = (PerlinNoise_2D(1.0 * (x+i),1.0 * (z+j)));
            pair<int,int> block_x_z(x + i, z + j);
            Block_Height[block_x_z] = y;
        }
    //Smooth edge. Especially for mountains
        float x_left,x_right,z_left,z_right;
        float flag_height_x_left  = sinNoise(x-16+z, x-16-z);
        float flag_height_x_right = sinNoise(x+16+z, x+16-z);
        float flag_height_z_left  = sinNoise(x+z-16, x-z+16);
        float flag_height_z_right = sinNoise(x+z+16, x-z-16);
        HeightGenerate(x_left ,flag_height_x_left);
        HeightGenerate(x_right,flag_height_x_right);
        HeightGenerate(z_left ,flag_height_z_left);
        HeightGenerate(z_right,flag_height_z_right);

        for(int i = 0; i < 16; ++i)
            for(int j = 0; j < 16; ++j)
            {
                float real_times = ((i / 16.0) * x_right + (15 - i)/16.0 * x_left
                                  +(j / 16.0) * z_right + (15 - j)/16.0 * z_left) / 2;
                pair<int,int> block_x_z(x + i, z + j);
                Block_Height[block_x_z] = Block_Height[block_x_z] * real_times;
            }
    //Finallt
    for(int i = 0; i < 16; ++i)
        for(int j = 0; j < 16; ++j)
        {
            //y = (PerlinNoise_2D(1.0 * (x+i),1.0 * (z+j)) * height_more);
            pair<int,int> block_x_z(x + i, z + j);
            y = Block_Height[block_x_z];


            tuple<int, int, int> positiongrass(x + i, y, z + j);
            if(BIO_NOW == DESERT)
            {
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(positiongrass, SAND));

            }
            else
            {
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(positiongrass, GRASS));
            }
            tuple<int, int, int> positionbedrock(x + i, -128, z + j);
            mapWorld.insert(pair<tuple<int, int, int>, blocktype>(positionbedrock, BEDROCK));
            for(int k = -127; k < 0; ++k)
            {
                tuple<int, int, int> position(x + i , k, z + j);
                map<tuple<int,int,int>,blocktype>::iterator it1= mapCave.find(position);
                if (it1!=mapCave.end())
                {
                    if (it1->second == LAVA)
                        mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, LAVA));
                    mapCave.erase(it1);
                }
                else
                    mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, STONE));
            }
            for(int k = 0; k < y; ++k)
            {
                tuple<int, int, int> position(x + i , k, z + j);
                if(BIO_NOW == DESERT)
                {
                    mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, SAND));
                }
                else
                {
                    mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, DIRT));
                }
            }
            if (y>=30)
            {
                tuple<int, int, int> position(x + i, y +1, z + j);
                mapWorld.insert(pair<tuple<int, int, int>, blocktype>(position, SNOW));
            }
        }

}

// Remove a bunch of blocks around (x,y,z) by tunnel radius
#define CAVE_SIZE 15
void ProceduralTerrain::CreateTunnel(int x, int y, int z, float radius)
{
    for(int i = -CAVE_SIZE; i <= CAVE_SIZE; ++i)
        for(int j = -CAVE_SIZE; j <= CAVE_SIZE; ++j)
            for(int k = -CAVE_SIZE; k <= CAVE_SIZE; ++k)
            {
                if ((1.0 * i * i + 1.0 * j * j + 1.0 * k * k) > radius) continue;
                tuple<int, int, int> position(x + i , y + k, z + j);

                map<tuple<int,int,int>,blocktype>::iterator it1= mapWorld.find(position);
                if (it1!=mapWorld.end())
                    mapWorld.erase(it1);
            }
    //IRON AND COAL
    for(int i = -CAVE_SIZE; i <= CAVE_SIZE; ++i)
        for(int j = -CAVE_SIZE; j <= CAVE_SIZE; ++j)
            for(int k = -CAVE_SIZE; k <= CAVE_SIZE; ++k)
            {
                if ((1.0 * i * i + 1.0 * j * j + 1.0 * k * k) > radius + 1) continue;
                tuple<int, int, int> position(x + i , y + k, z + j);
                map<tuple<int,int,int>,blocktype>::iterator it1= mapWorld.find(position);
                if (it1!=mapWorld.end())
                {
                    int rand_num = rand();
                    if (rand_num % 20 == 4)
                    {
                        it1->second = COAL;
                    }
                    else if (rand_num % 20 == 3)
                    {
                        it1->second = IRONORE;
                    }
                }
            }
}

// Create an ellipsoid-shaped cave of random XYZ scale
void ProceduralTerrain::CreateEllipsoidcave(int x, int y, int z)
{
    int Scale_x = (rand() % 4) + 4;
    int Scale_y = (rand() % 4) + 4;
    int Scale_z = (rand() % 4) + 4;

    for(int i = -Scale_x; i <= Scale_x; ++i)
        for(int j = -Scale_y; j <= Scale_y; ++j)
            for(int k = -Scale_z; k <= Scale_z; ++k)
            {
                if ((1.0 * i * i/(Scale_x * Scale_x) + 1.0 * j * j/(Scale_y * Scale_y) + 1.0 * k * k /(Scale_z * Scale_z)) > 1) continue;
                tuple<int, int, int> position(x + i , y + k, z + j);
                mapCave[position] = EMPTY;

                map<tuple<int,int,int>,blocktype>::iterator it1= mapWorld.find(position);
                if (it1!=mapWorld.end())
                    mapWorld.erase(it1);

            }
    // Lava
    for(int i = -Scale_x; i <= Scale_x; ++i)
        for(int j = -Scale_y; j <= Scale_y; ++j)
            for(int k = -Scale_z; k <= Scale_z; ++k)
            {
                if ((1.0 * i * i/(Scale_x * Scale_x) + 1.0 * j * j/(Scale_y * Scale_y) + 1.0 * k * k /(Scale_z * Scale_z)) > 1.1) continue;
                tuple<int, int, int> CurrentBlock(x + i , y + k, z + j);
                map<tuple<int,int,int>,blocktype>::iterator it1= mapWorld.find(CurrentBlock);
                int number_rand = rand() % 20;
                if (it1!=mapWorld.end())
                {
                    if (number_rand <= 2)
                    for (int di = -1; di <= 1; ++ di)
                        for (int dj = -1; dj <= 1; ++ dj)
                        {
                            if (abs(di) + abs(dj) == 2) continue;
                            tuple<int, int, int> LavaBlock(x + i + di, y + k, z + j + dj);
                            mapWorld[LavaBlock] = LAVA;
                            mapCave[LavaBlock] = LAVA;
                        }
                    break;
                }
            }
    //IRON AND COAL
    for(int i = -Scale_x; i <= Scale_x; ++i)
        for(int j = -Scale_y; j <= Scale_y; ++j)
            for(int k = -Scale_z; k <= Scale_z; ++k)
            {
                if ((1.0 * i * i/(Scale_x * Scale_x) + 1.0 * j * j/(Scale_y * Scale_y) + 1.0 * k * k /(Scale_z * Scale_z)) > 1.5) continue;
                tuple<int, int, int> position(x + i , y + k, z + j);
                map<tuple<int,int,int>,blocktype>::iterator it1= mapWorld.find(position);
                int rand_num = rand();
                if (it1!=mapWorld.end())
                {
                    if (rand_num % 18 == 4)
                    {
                        it1->second = COAL;
                    }
                    else if (rand_num % 18 == 3)
                    {
                        it1->second = IRONORE;
                    }
                }
                if (rand_num % 18 == 4)
                {
                    mapCave[position]= COAL;
                }
                else if (rand_num % 18 == 3)
                {
                    mapCave[position] = IRONORE;
                }
            }
}

// Generate caves after the terrain has generated
void ProceduralTerrain::CaveGenerator()
{
    float Moving_x = 32;
    float Moving_y = 0;
    float Moving_z = 32;

    for (int step = 0; step < 128 ; ++step)
    {
        if ((step > 20) && ( rand() % 50 > 30)){
            break;
            // Ensuring Length is greater than 10 and stop randomly.
        }
        float angle_xz_fai = PerlinNoise_3D_xz(Moving_x,Moving_y,Moving_z);
        float angle_y_theta  = PerlinNoise_3D_y(Moving_x,Moving_y,Moving_z);
        float Direction_x = sin(angle_y_theta) * cos(angle_xz_fai);
        float Direction_y = cos(angle_y_theta);
        float Direction_z = sin(angle_y_theta) * sin(angle_xz_fai);
        for (int length = 0; length < 3; ++ length)
        {
            int step_x = int(Moving_x + Direction_x > 0?Moving_x + Direction_x:Moving_x + Direction_x-1);
            int step_y = int(Moving_y + Direction_y > 0?Moving_y + Direction_y:Moving_y + Direction_y-1);
            int step_z = int(Moving_z + Direction_z > 0?Moving_z + Direction_z:Moving_z + Direction_z-1);
            CreateTunnel(step_x,step_y,step_z, 3.5);
        }
        Moving_x+=Direction_x * 3;
        Moving_y+=Direction_y * 3;
        Moving_z+=Direction_z * 3;
        if (rand() % 10 > 8)
            CreateEllipsoidcave(Moving_x,Moving_y,Moving_z);
    }
}
