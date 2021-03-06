#include "shaderprogram.h"
#include <QFile>
#include <QStringBuilder>
#include <QDir>
#include <iostream>



ShaderProgram::ShaderProgram(GLWidget277 *context)
    : vertShader(), fragShader(), prog(),textureHandler(),normalmapHandler(),grayTextureHandler(),invMapHandler(),
      attrPos(-1), attrNor(-1), attrCol(-1),
      unifModel(-1), unifModelInvTr(-1), unifViewProj(-1), unifColor(-1),unifSampler(-1),attrUv(-1),unifNormal(-1),
      unifTime(-1),attrBlockType(-1),attrShiness(-1),unifLightDir(-1),unifGraySampler(-1),attrBiomeType(-1),context(context)
{}

void ShaderProgram::create(const char *vertfile, const char *fragfile)
{
    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();
    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertfile);
    QString qFragSource = qTextFileRead(fragfile);

    char* vertSource = new char[qVertSource.size()+1];
    strcpy(vertSource, qVertSource.toStdString().c_str());
    char* fragSource = new char[qFragSource.size()+1];
    strcpy(fragSource, qFragSource.toStdString().c_str());


    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, &vertSource, 0);
    context->glShaderSource(fragShader, 1, &fragSource, 0);
    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);
    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(vertShader);
    }
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrCol = context->glGetAttribLocation(prog, "vs_Col");

    //Yuxin MM02
    attrUv = context->glGetAttribLocation(prog, "vs_uv");
    attrBlockType = context->glGetAttribLocation(prog, "vs_blockType");
    attrShiness = context->glGetAttribLocation(prog, "vs_shiness");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifColor      = context->glGetUniformLocation(prog, "u_Color");

    //Yuxin MM02
    unifSampler = context->glGetUniformLocation(prog, "textureSampler");
    unifNormal = context->glGetUniformLocation(prog, "normalSampler");
    unifTime = context->glGetUniformLocation(prog, "u_Time");
    unifEye = context->glGetUniformLocation(prog, "eyePos");

    //Yuxin MM03
    attrBiomeType = context->glGetAttribLocation(prog, "vs_biomeType");
    unifLightDir = context->glGetUniformLocation(prog, "u_LightDir");
    unifLightCol = context->glGetUniformLocation(prog, "u_LightCol");
    unifGraySampler = context->glGetUniformLocation(prog, "grayTextureSampler");
    //Jiahao MM03
    unifInv = context->glGetUniformLocation(prog, "InvSampler");
}

void ShaderProgram::useMe()
{
    context->glUseProgram(prog);
}

void ShaderProgram::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &model[0][0]);
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &modelinvtr[0][0]);
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4 &vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if(unifViewProj != -1) {
    // Pass a 4x4 matrix into a uniform variable in our shader
                    // Handle to the matrix variable on the GPU
    context->glUniformMatrix4fv(unifViewProj,
                    // How many matrices to pass
                       1,
                    // Transpose the matrix? OpenGL uses column-major, so no.
                       GL_FALSE,
                    // Pointer to the first element of the matrix
                       &vp[0][0]);
    }
}

void ShaderProgram::setGeometryColor(glm::vec4 color)
{
    useMe();

    if(unifColor != -1)
    {
        context->glUniform4fv(unifColor, 1, &color[0]);
    }
}

void ShaderProgram::setTimeCount(int timeCount){
    useMe();

    if(unifTime != -1)
    {
        context->glUniform1i(unifTime,timeCount);
    }
}

void ShaderProgram::setEyePosition(glm::vec3 eyePos){
    useMe();
    if(unifEye!=-1)
    {
        context->glUniform3fv(unifEye,1,&eyePos[0]);
    }
}

void ShaderProgram::setLightDir(glm::vec4 lightDir){
    useMe();
    if(unifLightDir!=-1){
        context->glUniform4fv(unifLightDir,1,&lightDir[0]);
    }
}

void ShaderProgram::setLightCol(glm::vec4 lightCol){
    useMe();
    if(unifLightCol!=-1){
        context->glUniform4fv(unifLightCol,1,&lightCol[0]);
    }
}

//This function, as its name implies, uses the passed in GL widget
void ShaderProgram::draw(Drawable &d)
{
        useMe();

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

        // Remember, by calling bindPos(), we call
        // glBindBuffer on the Drawable's VBO for vertex position,
        // meaning that glVertexAttribPointer associates vs_Pos
        // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrUv != -1 && d.bindUV()) {
        context->glEnableVertexAttribArray(attrUv);
        context->glVertexAttribPointer(attrUv, 2, GL_FLOAT, false, 0, NULL);
    }
    //===================Added by Yuxin======================//
    if(attrPos != -1 && d.bindChunk()){
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos,4,GL_FLOAT,false,7*sizeof(glm::vec4),(void*)0);
    }
    if(attrNor != -1 && d.bindChunk()){
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor,4,GL_FLOAT,false,7*sizeof(glm::vec4),(void*)(sizeof(glm::vec4)));
    }
    if(attrCol !=-1 && d.bindChunk()){
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol,4,GL_FLOAT,false,7*sizeof(glm::vec4),(void*)(sizeof(glm::vec4)*2));
    }
    if(attrUv !=-1 && d.bindChunk()){
        context->glEnableVertexAttribArray(attrUv);
        context->glVertexAttribPointer(attrUv,2,GL_FLOAT,false,7*sizeof(glm::vec4),(void*)(sizeof(glm::vec4)*3));
    }
    if(attrBlockType!=-1 && d.bindChunk()){
        context->glEnableVertexAttribArray(attrBlockType);
        context->glVertexAttribPointer(attrBlockType,1,GL_FLOAT,false,7*sizeof(glm::vec4), (void*)(sizeof(glm::vec4)*4));
    }
    if(attrShiness!=-1 && d.bindChunk()){
        context->glEnableVertexAttribArray(attrShiness);
        context->glVertexAttribPointer(attrShiness,1,GL_FLOAT,false,7*sizeof(glm::vec4), (void*)(sizeof(glm::vec4)*5));
    }
    if(attrBiomeType!=-1 && d.bindChunk()){
        context->glEnableVertexAttribArray(attrBiomeType);
        context->glVertexAttribPointer(attrBiomeType,1,GL_FLOAT,false,7*sizeof(glm::vec4), (void*)(sizeof(glm::vec4)*6));
    }


    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrUv != -1) context->glDisableVertexAttribArray(attrUv);
    if (attrBlockType !=-1) context->glDisableVertexAttribArray(attrBlockType);
    if (attrShiness != -1) context->glDisableVertexAttribArray(attrShiness);
    if (attrBiomeType!=-1) context->glDisableVertexAttribArray(attrBiomeType);

    context->printGLErrorLog();
}

char* ShaderProgram::textFileRead(const char* fileName) {
    char* text;

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

QString ShaderProgram::qTextFileRead(const char *fileName)
{
    QString text;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

void ShaderProgram::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void ShaderProgram::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }
}

//Yuxin MM02
void ShaderProgram::bindTexture0()
{
    context->glActiveTexture(GL_TEXTURE0);
    context->glBindTexture(GL_TEXTURE_2D,textureHandler);
}

//Yuxin MM02
void ShaderProgram::setUpTexture(){
    useMe();

    context->glGenTextures(1, &textureHandler);
    context->glActiveTexture(GL_TEXTURE0);
    context->glBindTexture(GL_TEXTURE_2D,textureHandler);

    //Read in texture image
    int width, height;
    QDir currentDir = QDir::current();
    currentDir.cdUp();
    currentDir.cd("mini_minecraft_basecode/texturemap");
    QString textureFile = currentDir.path()+"/minecraft_textures_all.png";
    QByteArray inBytes;
    inBytes = textureFile.toUtf8();
    const char* fileName = inBytes.constData();
    unsigned char* image = SOIL_load_image(fileName,&width, &height, 0, SOIL_LOAD_RGB);
    printf("SOIL texture loading results: '%s'\n", SOIL_last_result());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(unifSampler!=-1){
        context->glUniform1i(unifSampler, 0);
    }

}

//Yuxin MM02
void ShaderProgram::bindNormalMap0(){
    context->glActiveTexture(GL_TEXTURE1);
    context->glBindTexture(GL_TEXTURE_2D,normalmapHandler);
}

//Yuxin MM02
void ShaderProgram::setUpNormalMap(){
    useMe();

    context->glGenTextures(1, &normalmapHandler);
    context->glActiveTexture(GL_TEXTURE1);
    context->glBindTexture(GL_TEXTURE_2D,normalmapHandler);

    //Read in normap map image
    int width, height;

    QDir currentDir = QDir::current();
    currentDir.cdUp();
    currentDir.cd("mini_minecraft_basecode/texturemap");
    //std::cout<<"currentDir is: "<<currentDir.path().toStdString()<<std::endl;
    QString textureFile = currentDir.path()+"/minecraft_normals_all.png";
    //std::cout<<"textureFile is: "<<textureFile.toStdString()<<std::endl;
    QByteArray inBytes;
    inBytes = textureFile.toUtf8();
    const char* fileName = inBytes.constData();
    unsigned char* image = SOIL_load_image(fileName,&width, &height, 0, SOIL_LOAD_RGB);
    printf("SOIL normal loading results: '%s'\n", SOIL_last_result());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if(unifNormal!=-1){
        context->glUniform1i(unifNormal, 1);
    }
}

//Yuxin MM03
void ShaderProgram::bindTexture1(){
    context->glActiveTexture(GL_TEXTURE2);
    context->glBindTexture(GL_TEXTURE_2D, grayTextureHandler);
}


void ShaderProgram::setUpGrayTexture(){
    useMe();

    context->glGenTextures(1,&grayTextureHandler);
    context->glActiveTexture(GL_TEXTURE2);
    context->glBindTexture(GL_TEXTURE_2D,grayTextureHandler);

    //Read in gray scale texture map image
    int width, height;

    QDir currentDir = QDir::current();
    currentDir.cdUp();
    currentDir.cd("mini_minecraft_basecode/texturemap");
    QString textureFile = currentDir.path()+"/minecraft_textures_all_grey_grass.png";
    QByteArray inBytes;
    inBytes = textureFile.toUtf8();
    const char* fileName = inBytes.constData();
    unsigned char* image = SOIL_load_image(fileName,&width, &height, 0, SOIL_LOAD_RGBA);
    printf("SOIL gray grass texture loading results: '%s'\n", SOIL_last_result());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if(unifGraySampler!=-1){
        context->glUniform1i(unifGraySampler, 2);
    }
}

//Jiahao MM03
void ShaderProgram::bindTexture31()
{
    context->glActiveTexture(GL_TEXTURE31);
    context->glBindTexture(GL_TEXTURE_2D,invMapHandler);
}
void ShaderProgram::setUpInventory(){
    useMe();

    context->glGenTextures(1, &invMapHandler);
    context->glActiveTexture(GL_TEXTURE31);
    context->glBindTexture(GL_TEXTURE_2D,invMapHandler);

    //Read in texture image
    int width, height;
    QDir currentDir = QDir::current();
    currentDir.cdUp();
    currentDir.cd("mini_minecraft_basecode/texturemap");
    QString textureFile = currentDir.path()+"/inventory.png";
    QByteArray inBytes;
    inBytes = textureFile.toUtf8();
    const char* fileName = inBytes.constData();
    unsigned char* image = SOIL_load_image(fileName,&width, &height, 0, SOIL_LOAD_RGB);
    printf("SOIL inv loading results: '%s'\n", SOIL_last_result());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(unifInv!=-1){
        context->glUniform1i(unifInv, 31);
    }
}
