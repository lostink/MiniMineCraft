#include "character.h"

character::character():WorldCamera(nullptr),mesh(nullptr),velocity_down(0)
{

}
void character::SetMainCamera(Camera *input)
{
    WorldCamera = input;
}
void character::SetMesh(map<tuple<int, int, int>, int> *input)
{
    mesh = input;
}
void character::RenewPlace(Camera *MainCamera){
    //Copy for this time.
    //Later figure out how to make it efficient.
    WorldCamera = MainCamera;
    this->fovy      = MainCamera->fovy;
    this->width     = MainCamera->width;
    this->height    = MainCamera->height;
    this->aspect    = MainCamera->aspect;

    this->eye       = MainCamera->eye;
    this->ref       = MainCamera->ref;
    this->look      = MainCamera->look;
    this->up        = MainCamera->up;
    this->right     = MainCamera->right;
    this->world_up  = MainCamera->world_up;
    this->V         = MainCamera->V;
    this->H         = MainCamera->H;
    //
}
//These three functions will
void character::CheckTranslateAlongLook(float amt){
    glm::vec3 forward_vec = glm::vec3(look[0],0,look[2]);
    forward_vec /= forward_vec.length();
    glm::vec3 translation = forward_vec * (amt + 0.3f);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)temp_eye[0];
    int y = (int)temp_eye[1];
    int z = (int)temp_eye[2];
    //printf("%d %d %d\n",x,y,z);
    tuple<int,int,int> check1(x,y,z),check2(x,y+1,z);
    map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
    map<tuple<int,int,int>,int>::iterator it2= mesh->find(check2);
    if(it1 == it2)//it1 = it2 ,means there is no block there
    {
        WorldCamera->TranslateAlongLook(amt);
        RenewPlace(WorldCamera);
    }
}
void character::CheckTranslateAlongRight(float amt){
    glm::vec3 translation = right * (amt + 0.3f);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)temp_eye[0];
    int y = (int)temp_eye[1];
    int z = (int)temp_eye[2];
    tuple<int,int,int> check1(x,y,z),check2(x,y+1,z);
    map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
    map<tuple<int,int,int>,int>::iterator it2= mesh->find(check2);
    if(it1 == it2)//it1 = it2 ,means there is no block there
    {
        WorldCamera->TranslateAlongRight(amt);
        RenewPlace(WorldCamera);
    }
}
void character::CheckTranslateAlongUp(float amt){
    glm::vec3 translation = up * (amt + 0.3f);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)temp_eye[0];
    int y = (int)temp_eye[1];
    int z = (int)temp_eye[2];
    tuple<int,int,int> check1(x,y,z),check2(x,y,z+1);
    map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
    map<tuple<int,int,int>,int>::iterator it2= mesh->find(check2);
    if(it1 == it2)//it1 = it2 ,means there is no block there
    {
        WorldCamera->TranslateAlongUp(amt);
        RenewPlace(WorldCamera);
    }
}
//=======================================
void character::Jump()
{
    if (fabs(velocity_down) < 1e-7){
        //if u r in the air u cannot jump
        velocity_down = -0.5;
        //tuple<int,int,int> a(32,25,32);
        //(*mesh)[a] = 1;
    }
}
void character::Falling()
{
    velocity_down += G;
    int x = (int)eye[0];
    int y = (int)eye[2];
    int z;
    if (velocity_down > 0){
        double temp_z = (eye[1] - 1.5f);
        z = (int)temp_z;
    }
    else{
        double temp_z = (eye[1] + 0.5f);
        z = (int)temp_z;
    }
    //For loop check if block is outthere somewhere
    /*
    tuple<int,int,int> check1(x,y,z);
    map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
    if (it1 == mesh->end())
    {
        //no block hitting
        printf("End\n");
    }
    else
    {
        velocity_down = eye[1] - z;
    }
    */
    if (velocity_down > 0)
    {
        for (int i=0;i<velocity_down+1;++i)
        {
            tuple<int,int,int> check1(x,z-i,y);
            map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
            if (it1 == mesh->end())
            {
            }
            else
            {
                velocity_down = eye[1] - (z-i) - 1.5;
                //printf("v:%lf\n",velocity_down);
                break;
            }
        }
    }else
    {
        for (int i=0;i>velocity_down-1;--i)
        {
            tuple<int,int,int> check1(x,z-i,y);
            map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
            if (it1 == mesh->end())
            {
            }
            else
            {
                velocity_down = eye[1] - (z-i) + 0.5;
                break;
            }
        }
    }
    //After setting velocity,every thing should be fine
    CheckTranslateAlongUp(-1 * velocity_down);
}
//TODO:
//tuple<int,int,int>
