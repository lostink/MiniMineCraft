#include "inventory.h"

void Inventory::setCenter(QPoint Now)
{
    Center = Now;
}
GLenum Inventory::drawMode()
{
    return GL_TRIANGLES;
}
void Inventory::create()
{
    GLuint center_idx[6] = {0};
    glm::vec4 cent_vert_pos[4];
    glm::vec4 cent_vert_col[4];
    glm::vec2 cent_vert_UV[4];

    float x = Center.x();
    float width = x  * 2;
    float y = Center.y();
    float height = y * 2;

    for(int i = 0; i < 4; i++){
        cent_vert_pos[i][2] = 0;
        cent_vert_pos[i][3] = 1;
    }
    //Calculate its real screen coordinate
    float portion = 0.5;//from 0 to 1
    int blo_num = 10;
    float leftx = 0 - portion;
    float rightx = 0 + portion;
    float inv_height = (portion / blo_num) * width / height;
    float upy = -1 + inv_height;
    float downy = -1;
    //Calculate end.
    //Set index buffer
    center_idx[0] = 0;
    center_idx[1] = 1;
    center_idx[2] = 2;
    center_idx[3] = 0;
    center_idx[4] = 2;
    center_idx[5] = 3;
    //printf("%f %f %f %f\n",x,width,y,height);
    //Left up
    cent_vert_pos[0][0] = leftx;
    cent_vert_pos[0][1] = upy;
    //Left down
    cent_vert_pos[1][0] = leftx;
    cent_vert_pos[1][1] = downy;
    //right down
    cent_vert_pos[2][0] = rightx;
    cent_vert_pos[2][1] = downy;
    //right up
    cent_vert_pos[3][0] = rightx;
    cent_vert_pos[3][1] = upy;

    for(int i = 0; i < 4; i++){
        cent_vert_col[i] = glm::vec4(0.5f, 0.5f, 0.5f, 1);
    }
    //
    cent_vert_UV[0][0] = 0;
    cent_vert_UV[0][1] = 0;

    cent_vert_UV[1][0] = 0;
    cent_vert_UV[1][1] = 1;

    cent_vert_UV[2][0] = 1;
    cent_vert_UV[2][1] = 1;

    cent_vert_UV[3][0] = 1;
    cent_vert_UV[3][1] = 0;

    count = 6;

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
    context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), cent_vert_pos, GL_STATIC_DRAW);

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), cent_vert_col, GL_STATIC_DRAW);

    generateUV();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
    context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), cent_vert_UV, GL_STATIC_DRAW);
}
