#ifndef CHARACTER_H
#define CHARACTER_H
#include <camera.h>
#include <map>
#include <tuple>
#include <proceduralterrain.h>
#include <scene/chunkmanager.h>
#include <proceduralterrain.h>
using std::map;
using std::tuple;
const float G = 0.01;//9.8 M/S^2 1 -> 16
class character : public Camera
{
public:
    Camera                            *WorldCamera;
    map<tuple<int,int,int>,blocktype> *mesh;
    ChunkManager                      *Manager;
    ProceduralTerrain                 *terrain;
    float velocity_down;//using to calculate velocity
    blocktype holding_type;//TODO:use enum to replace this.
    float upAngle;//Used to make constraint
    vector<tuple<int,int,int>> NewBlockVec;

    character();
    void SetMainCamera(Camera *input);
    void SetMesh(map<tuple<int,int,int>,blocktype> *input);
    void SetManager(ChunkManager* input);
    void SetTerrain(ProceduralTerrain* input);

    //Check if the character can move.
    //Attention: Camera should have 1.5 blocks height
    void RenewPlace(Camera* MainCamera);
    void CheckTranslateAlongLook(float amt);
    void CheckTranslateAlongRight(float amt);
    void CheckTranslateAlongUp(float amt);

    vector<tuple<int,int,int>>* GetNewBlockVec();

    void CheckRotateAboutUp(float deg);
    void CheckRotateAboutRight(float deg);
    //Physics part:
    //velocity on z axis greater than zero : falling down
    //velocity on z axis less    than zero : jumping up
    void Jump();   //set velocity on z axis to
    void Falling();//This part notonly

    //Raytracing part:
    void DeleteBlockLookAt();
    void AddBlockToLookAt();
    void RefreshBound();
};
#endif // CHARACTER_H
