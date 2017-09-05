#include "character.h"
#define RIVER_HEIGHT 4
character::character():WorldCamera(nullptr),mesh(nullptr),velocity_down(0),Manager(nullptr),terrain(nullptr)
{
    upAngle = 0;
    DisableFlyingAndCollision = 0;
    velocity_down = 0;
    SentenceGenerate();
    printf("Generating Rivers...");
    ParsingMain(glm::vec3(32,4,32),glm::vec3(cos(0.1),0,sin(0.1)),river_sentence_main,0,river_sentence_main.size(),4);
    ParsingBranch(glm::vec3(0,4,32),glm::vec3(-cos(0.1),0,sin(0.1)),river_sentence_branch,0,river_sentence_branch.size(),4);
    holding_type_int = 0;
    holding_type = DIRT;
    printf("River Generate Ends.");
}
void character::SetMainCamera(Camera *input)
{
    WorldCamera = input;
}
void character::SetMesh(map<tuple<int, int, int>, blocktype> *input)
{
    mesh = input;
    //Initialize my river block
    //Here insert my river block
    for (int fx = 0;fx<64;++fx)
        for (int fz = 0;fz<64;++fz)
        {
            tuple<int,int,int> River_block(fx,RIVER_HEIGHT,fz);
            map<tuple<int,int,int>,blocktype>::iterator it_river = River_main.find(River_block);
            map<tuple<int,int,int>,blocktype>::iterator it_river_branchj = River_branch.find(River_block);

            if (it_river!=River_main.end() || it_river_branchj!=River_branch.end())
            {
                DigToSky(fx,RIVER_HEIGHT,fz);
                (*mesh)[River_block] = WATER;
            }
        }
    //Insert end.
}
void character::SetManager(ChunkManager *input)
{
    Manager = input;
}
void character::SetTerrain(ProceduralTerrain *input)
{
    terrain = input;
}
void character::setCenter(screenCenter *input)
{
    screenLayer = input;
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
    if(DisableFlyingAndCollision == 1)
    {
        WorldCamera->NoYTranslateAlongLook(amt);
        RenewPlace(WorldCamera);
        RefreshBound();
        return;
    }
    glm::vec3 forward_vec = glm::vec3(look[0],0,look[2]);
    forward_vec = glm::normalize(forward_vec);
    glm::vec3 translation = forward_vec * (amt>0?amt + 0.3f:amt - 0.3f);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)(temp_eye[0]>0?temp_eye[0]:temp_eye[0]-1);
    int y = (int)(temp_eye[1]>0?temp_eye[1]:temp_eye[1]-1);
    int z = (int)(temp_eye[2]>0?temp_eye[2]:temp_eye[2]-1);

    tuple<int,int,int> check1(x,y,z),check2(x,y-1,z);
    map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(check1);
    map<tuple<int,int,int>,blocktype>::iterator it2= mesh->find(check2);

    if((it1 == it2) && (it1 == mesh->end()))//it1 = it2 ,means there is no block there
    {
        WorldCamera->NoYTranslateAlongLook(amt);
        RenewPlace(WorldCamera);
        RefreshBound();
    }
}
void character::CheckTranslateAlongRight(float amt){
    if (DisableFlyingAndCollision == 1)
    {
        WorldCamera->TranslateAlongRight(amt);
        RenewPlace(WorldCamera);
        RefreshBound();
        return;
    }
    glm::vec3 translation = right * (amt>0? amt + 0.3f:amt -0.3f);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)(temp_eye[0]>0?temp_eye[0]:temp_eye[0]-1);
    int y = (int)(temp_eye[1]>0?temp_eye[1]:temp_eye[1]-1);
    int z = (int)(temp_eye[2]>0?temp_eye[2]:temp_eye[2]-1);
    tuple<int,int,int> check1(x,y,z),check2(x,y-1,z);
    map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(check1);
    map<tuple<int,int,int>,blocktype>::iterator it2= mesh->find(check2);
    if(it1 == it2)//it1 = it2 ,means there is no block there
    {
        WorldCamera->TranslateAlongRight(amt);
        RenewPlace(WorldCamera);
        RefreshBound();
    }
}
void character::CheckTranslateAlongUp(float amt){
    //printf("%f %f %f\n",world_up[0],world_up[1],world_up[2]);
    if (DisableFlyingAndCollision == 1)
    {
        WorldCamera->YTranslate(amt);
        RenewPlace(WorldCamera);
        return;
    }
    glm::vec3 translation = world_up * (amt);//Treat character as a cylinder
    glm::vec3 temp_eye = eye + translation;
    int x = (int)(temp_eye[0]>0?temp_eye[0]:temp_eye[0]-1);
    int y = (int)(temp_eye[1]>0?temp_eye[1]:temp_eye[1]-1);
    int z = (int)(temp_eye[2]>0?temp_eye[2]:temp_eye[2]-1);

    tuple<int,int,int> check1(x,y,z);
    map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(check1);
    if(it1 == mesh->end())//it1 = it2 ,means there is no block there
    {
        WorldCamera->YTranslate(amt);
        RenewPlace(WorldCamera);
    }
}
void character::CheckRotateAboutRight(float deg)//left and right
{
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
    //printf("%f %f %f\n",eye[0],eye[1],eye[2]);
    if (DisableFlyingAndCollision)
    {
        CheckTranslateAlongUp(0.5);
        return;
    }
    if (fabs(velocity_down) < 1e-7){
        //if u r in the air u cannot jump
        velocity_down = -0.5;
        //tuple<int,int,int> a(32,25,32);
        //(*mesh)[a] = 1;
    }
}
void character::Falling()
{
    //printf("%f\n",velocity_down);
    if (DisableFlyingAndCollision == 1)
        return;
    if (mesh == nullptr)
        return;
    velocity_down += G;
    int x = (int)(eye[0] > 0?eye[0]:eye[0]-1);
    int y = (int)(eye[2] > 0?eye[2]:eye[2]-1);
    int z;
    if (velocity_down >= 0){
        double temp_z = (eye[1] - 1.5f);
        z = (int)(temp_z>0?temp_z:temp_z-1);
    }
    else{
        double temp_z = (eye[1] + 0.5f);
        z = (int)(temp_z>0?temp_z:temp_z-1);
    }
    if (velocity_down > 0)
    {
        for (int i=0;i<velocity_down+1;++i)
        {
            tuple<int,int,int> check1(x,z-i,y);
            map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(check1);
            if (it1 == mesh->end())
            {
                continue;
            }
            else
            {
                velocity_down = eye[1] - (z-i) - 2.5;
                //Block down has
                //printf("v:%lf\n",velocity_down);
                break;
            }
        }
    }
    else
    {
        for (int i=0;i>velocity_down-1;--i)
        {
            tuple<int,int,int> check1(x,z-i,y);
            map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(check1);
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
    for (int i=0;i<60;++i){
        glm::vec3 NormForThis = 1.0f / 12.0f * norm;
        NormForThis *= (float)i;
        glm::vec3 find = (start + NormForThis);
        int x = (int)(find[0]>0?find[0]:find[0]-1);
        int y = (int)(find[1]>0?find[1]:find[1]-1);
        int z = (int)(find[2]>0?find[2]:find[2]-1);
        tuple<int,int,int> temp(x,y,z);
        map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(temp);
        //printf("Find:%d %d %d\n",x,y,z);
        //printf("%f %f %f\n",find[0],find[1],find[2]);
        //printf("start:%f %f %f\n\n",start[0],start[1],start[2]);
        if ((it1!=mesh->end()))
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
    map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(target);
    if (it1->second == BEDROCK) return;
    else
    {
        mesh->erase(it1);
        //TODO:
        //Tell chunk management to delete this
        Manager->deleteBlockAt(result[0],result[1],result[2]);
    }
}
void character::AddBlockToLookAt()
{
    //First: RayMarching
    glm::vec3 start = eye;
    glm::vec3 norm  = (ref - eye);//Watching
    glm::ivec3 cmp(-100,-100,-100),result(cmp);
    norm = glm::normalize(norm);
    for (int i=0;i<60;++i){
        glm::vec3 NormForThis = 1.0f / 12.0f * norm;
        NormForThis *= (float)i;
        glm::vec3 find = (start + NormForThis);
        int x = (int)(find[0]>0?find[0]:find[0]-1);
        int y = (int)(find[1]>0?find[1]:find[1]-1);
        int z = (int)(find[2]>0?find[2]:find[2]-1);
        tuple<int,int,int> temp(x,y,z);
        map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(temp);
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
    int eye_x = (int)(eye[0]>0?eye[0]:eye[0]-1);
    int eye_y = (int)(eye[1]>0?eye[1]:eye[1]-1);
    int eye_z = (int)(eye[2]>0?eye[2]:eye[2]-1);
    if (((NewBlock[0] == eye_x)&&(NewBlock[1] == eye_y  )&&(NewBlock[2] == eye_z)) ||
       ((NewBlock[0] == eye_x)&&(NewBlock[1] == eye_y-1)&&(NewBlock[2] == eye_z)))
        return;//Body Intersection
    //printf("%d %d %d\n\n========================\n",NewBlock[0],NewBlock[1],NewBlock[2]);
    tuple<int,int,int> NewAddingBlock(NewBlock[0],NewBlock[1],NewBlock[2]);
    (*mesh)[NewAddingBlock] = holding_type;
    Manager->addBlockAt(NewBlock[0],NewBlock[1],NewBlock[2]);
}
void character::RefreshBound()
{
    //printf("In!\n");
    NewBlockVec.clear();
    glm::ivec2 deltaBlock[8];
    deltaBlock[0] = glm::ivec2(-1,-1);
    deltaBlock[1] = glm::ivec2(-1, 0);
    deltaBlock[2] = glm::ivec2(-1,+1);
    deltaBlock[3] = glm::ivec2( 0,-1);
    deltaBlock[4] = glm::ivec2( 0,+1);
    deltaBlock[5] = glm::ivec2(+1,-1);
    deltaBlock[6] = glm::ivec2(+1, 0);
    deltaBlock[7] = glm::ivec2(+1,+1);
    for (int i=0;i<8;++i)
    {
        int block_x = (((int)(eye[0]>=0?eye[0]:eye[0]-16)) / 16) + deltaBlock[i][0];
        int block_z = (((int)(eye[2]>=0?eye[2]:eye[2]-16)) / 16) + deltaBlock[i][1];
        tuple<int,int,int> target(block_x * 16,-128,block_z * 16);
        map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(target);
        if (it1 == mesh->end())
        {
            terrain->addNewChunk(block_x * 16,block_z * 16);
            //Here insert my river block
            for (int fx = -3;fx<16+3;++fx)
                for (int fz = -3;fz<16+3;++fz)
                {
                    tuple<int,int,int> River_block(block_x * 16+fx,RIVER_HEIGHT,block_z*16+fz);
                    map<tuple<int,int,int>,blocktype>::iterator it_river = River_main.find(River_block);
                    map<tuple<int,int,int>,blocktype>::iterator it_river_branchj = River_branch.find(River_block);
                    if (it_river!=River_main.end() || it_river_branchj!=River_branch.end())
                    {
                        DigToSky(block_x * 16+fx,RIVER_HEIGHT,block_z*16+fz);
                        (*mesh)[River_block] = WATER;
                    }
                }
            //Insert end.
            for (int j=-8;j<4;++j){
                tuple<int,int,int> startPos(block_x * 16,j*16,block_z * 16);
                NewBlockVec.push_back(startPos);
            }
        }
    }
}
void character::ChangeFlyingAndCollision()
{
    DisableFlyingAndCollision ^= 1;

}
vector<tuple<int,int,int>>* character::GetNewBlockVec()
{
    return &NewBlockVec;
}
#define AWAYDISTANCE 3
void character::DigToSky(int x, int y, int z){
    for (int i=-AWAYDISTANCE;i<=AWAYDISTANCE;++i)
        for (int j=-AWAYDISTANCE;j<=AWAYDISTANCE;++j)
        {
            int height = max(abs(i),abs(j));
            bool grass_flag = 1;
            for (int tempy = y + height*3 ;tempy < 64;++tempy)
            {
                tuple<int,int,int> check(x+i,tempy,z+j);
                map<tuple<int,int,int>,blocktype>::iterator it1= mesh->find(check);
                if (it1!=mesh->end())
                {
                    if (grass_flag){
                        (*mesh)[check] = GRASS;
                        grass_flag = 0;
                    }
                    else
                        mesh->erase(it1);
                }
            }
        }
}
void character::ParsingMain(glm::vec3 location, glm::vec3 direction,const vector<char>&sentence, int start, int end, int width)
{
    if (start >= end) return;
    if (width <= 0) return;
    //Randomize moving

    ProceduralTerrain temp;
    float Random_angle = (temp.PerlinNoise_2D(location[0]*4,location[2]*4) / 32.0 - 0.5) * 3.141592653589793;
    glm::vec3 direction_o(direction);
    direction[0] = direction_o[0] * cos(Random_angle) +direction_o[2] * sin(Random_angle);
    direction[1] = 0;
    direction[2] = direction_o[0] * sin(Random_angle) -direction_o[2] * cos(Random_angle);

    if (sentence[start] == 'A')
    {
        int x = int(location[0]>0?location[0]:location[0]-1);
        int y = int(location[1]>0?location[1]:location[1]-1);
        int z = int(location[2]>0?location[2]:location[2]-1);
        for (int dx = -1 * width;dx <= width;++dx)
            for (int dz = -1 * width;dz <= width ;++dz)
            {
                if (abs(dx) + abs(dz)<= width)
                {
//                    printf("%d %d %d\n",x+dx,y,z+dz);
                    tuple<int ,int , int> current(x + dx,RIVER_HEIGHT,z + dz);
                    River_main[current] = WATER;
                }
            }
        ParsingMain(location+direction,direction,sentence,start + 1,end,width);
    }
    else if (sentence[start] == 'B')
    {
        ParsingMain(location + direction,direction,sentence,start+1,end,width);

        if (rand() % 20 == 2)
        {
            glm::vec3 direction_ori(direction);
            float flag = (rand() % 2 == 1)?1:-1;
            float angle = flag * 3.141592653589793 / 4.0;
            direction[0] = direction_ori[0] * cos(angle) +direction_ori[2] * sin(angle);
            direction[1] = 0;
            direction[2] = direction_ori[0] * sin(angle) -direction_ori[2] * cos(angle);
            width--;
            for (int times = 0;times < 15;++times)
            {
                int x = int(location[0]>0?location[0]:location[0]-1);
                int y = int(location[1]>0?location[1]:location[1]-1);
                int z = int(location[2]>0?location[2]:location[2]-1);
                for (int dx = -1 * width;dx <= width-1;++dx)
                    for (int dz = -1 * width;dz <= width-1;++dz)
                    {
                        if (abs(dx) + abs(dz)<= width)
                        {
        //                    printf("%d %d %d\n",x+dx,y,z+dz);
                            tuple<int ,int , int> current(x + dx,RIVER_HEIGHT,z + dz);
                            River_branch[current] = WATER;
                        }
                    }
                location = location + direction;
            }
            ParsingMain(location + direction,direction,sentence,start+1,end,width-1);
        }
    }
}
void character::ParsingBranch(glm::vec3 location, glm::vec3 direction,const vector<char>&sentence, int start, int end, int width)
{
    if (width==0)
        width+=rand() % 5 <=3?1:0;
    if (start >= end) return;
    if (width <= 0) return;
//    printf("%c\n",sentence[start]);

    if (sentence[start] == 'A')
    {
        for (int times = 0;times < 10;++times)
        {
            int x = int(location[0]>0?location[0]:location[0]-1);
            int y = int(location[1]>0?location[1]:location[1]-1);
            int z = int(location[2]>0?location[2]:location[2]-1);
            for (int dx = -1 * width;dx <= width;++dx)
                for (int dz = -1 * width;dz <= width ;++dz)
                {
                    if (abs(dx) + abs(dz)<= width)
                    {
    //                    printf("%d %d %d\n",x+dx,y,z+dz);
                        tuple<int ,int , int> current(x + dx,RIVER_HEIGHT,z + dz);
                        River_branch[current] = WATER;
                    }
                }
            location = location + direction;
        }
        ParsingBranch(location+direction,direction,sentence,start + 1,end,width);
    }
    else if (sentence[start] == 'B')
    {
        glm::vec3 direction_ori(direction);

        ProceduralTerrain temp;
        float Random_angle = (temp.PerlinNoise_2D(location[0]*4,location[2]*4) / 32.0 - 0.5) * 3.141592653589793;

        float angle = 3.141592653589793 / 6;
        direction[0] = direction_ori[0] * cos(angle+Random_angle) +direction_ori[2] * sin(angle+Random_angle);
        direction[1] = 0;
        direction[2] = direction_ori[0] * sin(angle+Random_angle) -direction_ori[2] * cos(angle+Random_angle);
        ParsingBranch(location,direction,sentence,start+1,end,width-1);
        direction[0] = direction_ori[0] * cos(-angle-Random_angle) +direction_ori[2] * sin(-angle-Random_angle);
        direction[1] = 0;
        direction[2] = direction_ori[0] * sin(-angle-Random_angle) -direction_ori[2] * cos(-angle-Random_angle);
        ParsingBranch(location,direction,sentence,start+1,end,width-1);
    }
}
void character::SentenceGenerate()
{
    river_sentence_main.clear();
    river_sentence_branch.clear();
    //River 1:
    //Main River with few branches
    river_sentence_main.push_back('A');
    river_sentence_main.push_back('B');
    river_sentence_main.push_back('A');
    river_sentence_main.push_back('A');
    river_sentence_main.push_back('A');
    river_sentence_main.push_back('A');
    river_sentence_main.push_back('A');
    while (river_sentence_main.size() < 200)
    {
        vector<char> temp;
        temp.clear();
        for (int i=0;i<river_sentence_main.size();++i)
        {
            if (river_sentence_main[i] == 'A')
            {
                temp.push_back('A');
                temp.push_back('A');
                temp.push_back('B');
                temp.push_back('A');
                temp.push_back('A');
                temp.push_back('A');
                temp.push_back('A');
            }
            else
                temp.push_back(river_sentence_main[i]);
            if (temp.size() > 200) break;
        }
        river_sentence_main.clear();
        for (int i=0;i<temp.size();++i)
            river_sentence_main.push_back(temp[i]);
    }
    //River 2:
    //Branching river:
    for (int i=0;i<1;++i)
        river_sentence_branch.push_back('A');
    river_sentence_branch.push_back('B');
    for (int i=0;i<3;++i)
        river_sentence_branch.push_back('A');
    while (river_sentence_branch.size() <20)
    {
        vector<char> temp;
        temp.clear();
        for (int i=0;i<river_sentence_branch.size();++i)
        {
            if (river_sentence_branch[i] == 'A')
            {
                temp.push_back('A');
                temp.push_back('B');
                temp.push_back('A');
                temp.push_back('A');
                temp.push_back('A');
            }
            else
                temp.push_back(river_sentence_branch[i]);
            if (temp.size() > 20)
                break;
        }
        river_sentence_branch.clear();
        for (int i=0;i<temp.size();++i)
            river_sentence_branch.push_back(temp[i]);
    }

}
void character::holding_type_change(int delta)
{
    //set holding type
    holding_type_int = (10 + holding_type_int + delta) % 10;

    switch (holding_type_int) {
    case 0:
        holding_type = DIRT;
        break;
    case 1:
        holding_type = GRASS;
        break;
    case 2:
        holding_type = LAVA;
        break;
    case 3:
        holding_type = STONE;
        break;
    case 4:
        holding_type = WOOD;
        break;
    case 5:
        holding_type = LEAF;
        break;
    case 6:
        holding_type = BEDROCK;
        break;
    case 7:
        holding_type = COAL;
        break;
    case 8:
        holding_type = IRONORE;
        break;
    case 9:
        holding_type = WATER;
        break;
    default:
        break;
    }
    screenLayer->setHoldingtype(holding_type_int);
    screenLayer->create();
}
