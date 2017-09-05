#ifndef CHARACTER_H
#define CHARACTER_H
#include <camera.h>
#include <map>
#include <tuple>
#include <QSound>
#include <proceduralterrain.h>
#include <scene/chunkmanager.h>
#include <scene/screencenter.h>
#include <proceduralterrain.h>
#include <audiomanager.h>
#include <algorithm>
using std::max;
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
    screenCenter                      *screenLayer;
    AudioManager                      *audioplayer;
    float velocity_down;//using to calculate velocity
    blocktype holding_type;//TODO:use enum to replace this.
    int holding_type_int;
    float upAngle;//Used to make constraint
    bool DisableFlyingAndCollision;
    vector<tuple<int,int,int>> NewBlockVec;
    vector<char> river_sentence_main,river_sentence_branch;
    map<tuple<int,int,int>,blocktype> River_main,River_branch;
    map<pair<int,int>,bool>  ChunkExistance;
    map<tuple<int,int,int>,blocktype> MAP_SAND;

    character();
    void SetMainCamera(Camera *input);
    void SetMesh(map<tuple<int,int,int>,blocktype> *input);
    void SetManager(ChunkManager* input);
    void SetTerrain(ProceduralTerrain* input);
    void SetAudio(AudioManager* input);
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
    void ChangeFlyingAndCollision();
    //Raytracing part:
    void DeleteBlockLookAt();
    void AddBlockToLookAt();
    void RefreshBound();
    //River part:
    void SentenceGenerate();
    void ParsingMain(glm::vec3 location,glm::vec3 direction,const vector<char>& sentence,int start,int end,int width);
    void ParsingBranch(glm::vec3 location,glm::vec3 direction,const vector<char>& sentence,int start,int end,int width);
    void DigToSky(int x,int y,int z);
    void GenerateSand();
    //Holding part
    void holding_type_change(int delta);
    void setCenter(screenCenter* input);
//    void setCenter(screenCenter* input);
};
#endif // CHARACTER_H
