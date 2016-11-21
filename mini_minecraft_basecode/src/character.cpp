#include "character.h"

character::character():WorldCamera(nullptr),mesh(nullptr),velocity_down(0)
{
    upAngle = 0;
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
    forward_vec = glm::normalize(forward_vec);
    glm::vec3 translation = forward_vec * (amt + 0.3f);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)temp_eye[0];
    int y = (int)temp_eye[1];
    int z = (int)temp_eye[2];
    /*
    printf("%lf\n",forward_vec.length());
    printf("%lf %lf %lf\n",forward_vec[0],forward_vec[1],forward_vec[2]);
    printf("%lf %lf %lf\n",temp_eye[0],temp_eye[1],temp_eye[2]);
    printf("%d %d %d\n",x,y,z);
    */
    tuple<int,int,int> check1(x,y,z),check2(x,y-1,z);
    map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
    map<tuple<int,int,int>,int>::iterator it2= mesh->find(check2);
    if(it1 == it2)//it1 = it2 ,means there is no block there
    {
        WorldCamera->NoYTranslateAlongLook(amt);
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
    glm::vec3 translation = world_up * (amt);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)temp_eye[0];
    int y = (int)temp_eye[1];
    int z = (int)temp_eye[2];
    tuple<int,int,int> check1(x,y,z);
    map<tuple<int,int,int>,int>::iterator it1= mesh->find(check1);
    if(it1 == mesh->end())//it1 = it2 ,means there is no block there
    {
        WorldCamera->YTranslate(amt);
        RenewPlace(WorldCamera);
    }
}
void character::CheckRotateAboutRight(float deg)//left and right
{
    //printf("%f\n",deg);
    //printf("%lf %lf %lf\n",ref[0],ref[1],ref[2]);
    /*
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
    glm::vec3 origina_ref = ref - eye;
    glm::vec3 rotated_ref = ref - eye;
    rotated_ref = glm::vec3(rotation * glm::vec4(ref, 1));
    if (glm::dot
        (
                glm::normalize(glm::cross(origina_ref,world_up)),
                glm::normalize(glm::cross(rotated_ref,world_up))
        )
            < 0)
    {
        float angle = acos(glm::dot(glm::normalize(origina_ref),glm::normalize(world_up))) - 0.1;
        WorldCamera->RotateAboutRight(glm::degrees(angle));
        RenewPlace(WorldCamera);
        printf("%lf %lf %lf\n",(ref-eye)[0],(ref-eye)[1],(ref-eye)[2]);
    }
    else
    {
       WorldCamera->RotateAboutRight(deg);
       RenewPlace(WorldCamera);
    }
    */
    //printf("%f\n",deg);
    float temp_deg = upAngle + deg;
    if (temp_deg > 90)
        deg = 89.99 - upAngle;
    else if (temp_deg < -90)
        deg = -89.99 - upAngle;
    WorldCamera->RotateAboutRight(deg);
    RenewPlace(WorldCamera);
}
void character::CheckRotateAboutUp(float deg)
{
    //This function is only used for jump and fall
    //So it uses world up axis
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    WorldCamera->RotateAboutUp(deg);
    RenewPlace(WorldCamera);
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
                velocity_down = eye[1] - (z-i) - 2.5;
                //Block down has
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
void character::DeleteBlockLookAt()
{
    //First: RayMarching
    glm::vec3 start = eye;
    glm::vec3 norm  = (ref - eye);
    glm::ivec3 cmp(-100,-100,-100),result(cmp);
    norm = glm::normalize(norm);
    for (int i=0;i<12;++i){
        glm::vec3 NormForThis = norm;
        for (int j=0;j<3;++j)
            NormForThis[j] = i * NormForThis[j];
        glm::vec3 find = (start + NormForThis);
        int x = (int)find[0];
        int y = (int)find[1];
        int z = (int)find[2];
        tuple<int,int,int> temp(x,y,z);
        map<tuple<int,int,int>,int>::iterator it1= mesh->find(temp);
        //printf("Find:%d %d %d\n",x,y,z);
        //printf("%f %f %f\n",find[0],find[1],find[2]);
        //printf("start:%f %f %f\n\n",start[0],start[1],start[2]);
        if (it1!=mesh->end())
        {
            result[0] = x;
            result[1] = y;
            result[2] = z;
            break;
        }
    }
    if (result == cmp)
        return;
    tuple<int,int,int> target(result[0],result[1],result[2]);
    map<tuple<int,int,int>,int>::iterator it1= mesh->find(target);
    mesh->erase(it1);
    //TODO:
    //Tell chunk management to delete this
}
void character::AddBlockToLookAt()
{
    //First: RayMarching
    glm::vec3 start = eye;
    glm::vec3 norm  = (ref - eye);//Watching
    glm::ivec3 cmp(-100,-100,-100),result(cmp);
    norm = glm::normalize(norm);
    for (int i=0;i<12;++i){
        glm::vec3 NormForThis = norm;
        for (int j=0;j<3;++j)
            NormForThis[j] = i * NormForThis[j];
        glm::vec3 find = (start + NormForThis);
        int x = (int)find[0];
        int y = (int)find[1];
        int z = (int)find[2];
        tuple<int,int,int> temp(x,y,z);
        map<tuple<int,int,int>,int>::iterator it1= mesh->find(temp);
        if (it1!=mesh->end())
        {
            result[0] = x;
            result[1] = y;
            result[2] = z;
            break;
        }
    }
    if (result == cmp)
        return;
    //x y z represent the block look at.
    //now we need to know which face it looks at
    //basic thought for a one typical face:
    //1. Check if watching direction is pareller with this face
    //2. Calculate the intersection point and
    glm::vec3 FacingNorm;
    glm::vec3 Normals[6] = {glm::vec3(0,0, 1),
                            glm::vec3(0,0,-1),
                            glm::vec3( 1,0,0),
                            glm::vec3(-1,0,0),
                            glm::vec3(0, 1,0),
                            glm::vec3(0,-1,0)
                           };
    float d = 2147483647;
    //printf("result:%d %d %d\n",result[0],result[1],result[2]);
    for (int i=0;i<6;++i)
    {
        double x = result[0];
        double y = result[1];
        double z = result[2];
        glm::vec3 n = Normals[i];
        glm::vec3 p0;
        if ((n[0] > 0)||(n[1] > 0)||(n[2] > 0))
            p0 = glm::vec3(x+1,y+1,z+1);
        else
            p0 = glm::vec3(x,y,z);

        glm::vec3 l0 = eye;
        glm::vec3 l  = norm;
        if (fabs(glm::dot(l,n)) < 1e-7)
            continue;//parellel
        float temp_d = glm::dot((p0 - l0) , n) / glm::dot(l , n);
        /*
        printf("p0:%lf %lf %lf\n",p0[0],p0[1],p0[2]);
        printf("l0:%lf %lf %lf\n",l0[0],l0[1],l0[2]);
        printf(" n:%lf %lf %lf\n", n[0], n[1], n[2]);
        */
        glm::vec3 Intersection_Point = l0 + temp_d * l;
        if ((Intersection_Point[0] < x)||(Intersection_Point[0] > x+1)||
            (Intersection_Point[1] < y)||(Intersection_Point[1] > y+1)||
            (Intersection_Point[2] < z)||(Intersection_Point[2] > z+1))
            continue;
        //Having intersection point with that plane
        //No intersection with this face
        //
        if (temp_d < 0)
            continue;
            //Just in case something weird happend.
            //There should not happen if a intersetion point is behind the head of
            //player
        //printf("%f\n",temp_d);
        if (temp_d < d){
            //if this face is more close
            //this face is the one we watch.
            d = temp_d;
            FacingNorm = Normals[i];
        }
        //printf("FacingNorm:%lf %lf %lf\n\n", n[0], n[1], n[2]);

    }
    //Now we have the face that we are looking at.
    //we need to check if the position we want to place
    //intersects with character.
    glm::ivec3 NewBlock;
    if (FacingNorm[0] > 0.5)
        NewBlock = result + glm::ivec3( 1,0,0);
    else if (FacingNorm[0] < -0.5)
        NewBlock = result + glm::ivec3(-1,0,0);
    else if (FacingNorm[1] > 0.5)
        NewBlock = result + glm::ivec3(0, 1,0);
    else if (FacingNorm[1] < -0.5)
        NewBlock = result + glm::ivec3(0,-1,0);
    else if (FacingNorm[2] > 0.5)
        NewBlock = result + glm::ivec3(0,0, 1);
    else if (FacingNorm[2] < -0.5)
        NewBlock = result + glm::ivec3(0,0,-1);
    int eye_x = (int)eye[0];
    int eye_y = (int)eye[1];
    int eye_z = (int)eye[2];
    if (((NewBlock[0] == eye_x)&&(NewBlock[1] == eye_y  )&&(NewBlock[2] == eye_z)) ||
       ((NewBlock[0] == eye_x)&&(NewBlock[1] == eye_y-1)&&(NewBlock[2] == eye_z)))
        return;//Body Intersection
    //printf("%d %d %d\n\n========================\n",NewBlock[0],NewBlock[1],NewBlock[2]);
    tuple<int,int,int> NewAddingBlock(NewBlock[0],NewBlock[1],NewBlock[2]);
    (*mesh)[NewAddingBlock] = holding_type;
}

