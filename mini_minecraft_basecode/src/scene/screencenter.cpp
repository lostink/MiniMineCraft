#include "screencenter.h"

void screenCenter::setCenter(QPoint Now)
{
    Center = Now;
    holding_type = 0;
}
GLenum screenCenter::drawMode()
{
    return GL_LINES;
}
void screenCenter::setHoldingtype(int input)
{
    holding_type = input;
}
void screenCenter::create()
{
    GLuint center_idx[12];
    glm::vec4 cent_vert_pos[8];
    glm::vec4 cent_vert_col[8];



    float x = Center.x();
    float width = x  * 2;
    float y = Center.y();
    float height = y * 2;
    for(int i = 0; i < 4; i++){
        center_idx[i] = i;
        cent_vert_pos[i][2] = 0;
        cent_vert_pos[i][3] = 1;
    }
    //printf("%f %f %f %f\n",x,width,y,height);
    cent_vert_pos[0][0] = 2.0 * (x - 1) / width - 1;
    cent_vert_pos[0][1] = 0;

    cent_vert_pos[1][0] = 2.0 * (x + 1) / width - 1;
    cent_vert_pos[1][1] = 0;

    cent_vert_pos[2][0] = 0;
    cent_vert_pos[2][1] = 2.0 * (y - 1) / height - 1;

    cent_vert_pos[3][0] = 0;
    cent_vert_pos[3][1] = 2.0 * (y + 1) / height - 1;
    for(int i = 0; i < 4; i++){
        cent_vert_col[i] = glm::vec4(1.0f, 1.0f, 1.0f, 1);
    }
    //Selection square
    float portion = 0.5;//from 0 to 1
    int blo_num = 10;
    float inv_width  = (2.0 * portion / blo_num);
    float inv_height = (portion / blo_num) * width / height;
    float leftx =  0 - portion + inv_width * (holding_type + 0);
    float rightx = 0 - portion + inv_width * (holding_type + 1);
    float upy = -1 + inv_height;
    float downy = -1;

    cent_vert_pos[4][0] = leftx;
    cent_vert_pos[4][1] = downy;

    cent_vert_pos[5][0] = leftx;
    cent_vert_pos[5][1] = upy;

    cent_vert_pos[6][0] = rightx;
    cent_vert_pos[6][1] = upy;

    cent_vert_pos[7][0] = rightx;
    cent_vert_pos[7][1] = downy;
    for(int i = 4; i < 8; i++)
    {
        cent_vert_pos[i][2] = 0;
        cent_vert_pos[i][3] = 1;
    }
    for(int i = 4; i < 8; i++)
        cent_vert_col[i] = glm::vec4(1.0f, 1.0f, 1.0f, 1);
    center_idx[4] = 4;
    center_idx[5] = 5;
    center_idx[6] = 5;
    center_idx[7] = 6;
    center_idx[8] = 6;
    center_idx[9] = 7;
    center_idx[10] = 7;
    center_idx[11] = 4;
    //done

    count = 12;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), center_idx, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec4), cent_vert_pos, GL_STATIC_DRAW);

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec4), cent_vert_col, GL_STATIC_DRAW);

}
