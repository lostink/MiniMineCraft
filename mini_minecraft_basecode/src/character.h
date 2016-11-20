#ifndef CHARACTER_H
#define CHARACTER_H
#include <camera.h>
#include <map>
#include <tuple>
using std::map;
using std::tuple;
const float G = 0.01;
class character : public Camera
{
public:
    Camera                      *WorldCamera;
    map<tuple<int,int,int>,int> *mesh;
    float velocity_down;//using to calculate velocity

    character();
    void SetMainCamera(Camera *input);
    void SetMesh(map<tuple<int,int,int>,int> *input);


    //Check if the character can move.
    //Attention: Camera should have 1.5 blocks height
    void RenewPlace(Camera* MainCamera);
    void CheckTranslateAlongLook(float amt);
    void CheckTranslateAlongRight(float amt);
    void CheckTranslateAlongUp(float amt);

    //Physics part:
    //velocity on z axis greater than zero : falling down
    //velocity on z axis less    than zero : jumping up
    void Jump();   //set velocity on z axis to
    void Falling();//This part notonly
};

#endif // CHARACTER_H
