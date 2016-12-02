#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

//Yuxin MM02
uniform int u_Time;         //Used to animate the water and lava texture

in vec2 vs_uv;              // The array of vertex uvs passed to the shader.
in float vs_blockType;        // The block type indicating if the uv should be animated

out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.

//Yuxin MM02
out vec2 fs_uv;            //The uv of each vertex
out vec4 fs_tangent;       //vertex local tangent vector
out vec4 fs_bitangent;     //vertex local bitangent vector

const vec4 lightDir = vec4(1,1,1,0);  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

void main()
{
    //fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation

    //Yuxin MM02
    //Animate the uv if the block is water or lava
    //fs_Col = vec4(vs_blockType,0,0,1);
    if(abs(vs_blockType-3)<0.0001){
        //fs_Col = vec4(vs_blockType,0,0,1);
        //u_Time must be multiples of 16
        int time = u_Time%16;
        float movement = time*1.0/128;
        fs_uv[0] = vs_uv[0]+movement;
        fs_uv[1] = vs_uv[1];
    }else{
        fs_uv = vs_uv;
    }

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.

    //Yuxin MM02
    vec3 vs_tangent = cross(vec3(0,1,0),vec3(vs_Nor));
    normalize(vs_tangent);
    fs_tangent = vec4(invTranspose * vs_tangent, 0);

    vec3 vs_bitangent = cross(vec3(vs_Nor),vs_tangent);
    normalize(vs_bitangent);
    fs_bitangent = vec4(invTranspose * vs_bitangent, 0);



    vec4 modelposition = u_Model * vs_Pos;   // Temporarily store the transformed vertex positions for use below

    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
