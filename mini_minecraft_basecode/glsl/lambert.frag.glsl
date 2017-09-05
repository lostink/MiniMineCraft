#version 150
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform vec4 u_Color; // The color with which to render this instance of geometry.

//Yuxin MM02
uniform sampler2D textureSampler; //read texture image
uniform sampler2D normalSampler; //read normalmap image

//Yuxin MM03
uniform vec4 u_LightCol; // The color of virtual light
uniform sampler2D grayTextureSampler; //read gray scale grass texture image


// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;

//Yuxin MM02
in vec2 fs_uv;
in vec4 fs_tangent;
in vec4 fs_bitangent;
in vec4 fs_view;
in float fs_shiness;

//Yuxin MM03
in float fs_blockType;
in float fs_biomeType;


out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

void main()
{
        // Material base color (before shading)
        //vec4 diffuseColor = fs_Col;

        //Yuxin MM02 calculate the color based on texture sampler
        //Yuxin MM03 read from grayTextureSampler if it is grass, add a base color to grass based on biome type
        vec4 readColor = vec4(0,0,0,0);
        vec4 diffuseColor = vec4(0,0,0,1);
        vec2 finalUV;
        float transparency;
        if(abs(fs_blockType-2)<0.0001){
            readColor = texture(grayTextureSampler,fs_uv);
            //diffuseColor = readColor;
            if(abs(fs_biomeType)<0.0001){
                //DESERT, Yellow
                diffuseColor = vec4(0.75*readColor.r,0.72*readColor.g,0.33*readColor.b,readColor.a);
            }else if(abs(fs_biomeType-1)<0.0001){
                //FOREST Deep Green
                diffuseColor = vec4(0.31*readColor.r,0.80*readColor.g,0.20*readColor.b,readColor.a);
            }else if(abs(fs_biomeType-2)<0.0001){
                //SWAMP Brown Green
                diffuseColor = vec4(0.40*readColor.r,0.78*readColor.g,0.29*readColor.b,readColor.a);
            }else if(abs(fs_biomeType-3)<0.0001){
                //PLAIN Light Green
                diffuseColor = vec4(0.55*readColor.r,0.74*readColor.g,0.35*readColor.b,readColor.a);
            }else if(abs(fs_biomeType-4)<0.0001){
                //SNOW White
                diffuseColor = vec4(1.0f*readColor.r,1.0f*readColor.g,1.0f*readColor.b,readColor.a);
            }else if(abs(fs_biomeType-6)<0.0001){
                //Edge of chunk, use average color to smoothly connect two biomes
                diffuseColor = vec4(0.56*readColor.r,0.74*readColor.g,0.38*readColor.b,readColor.a);
            }
            transparency = readColor.a;
            if(fs_uv[0]>=6.0/16 && fs_uv[0]<=7.0/16){
                finalUV[0] = fs_uv[0] - 3.0/16;
                finalUV[1] = fs_uv[1] - 2.0/16;
                readColor = texture(grayTextureSampler,finalUV);
                diffuseColor = (1-transparency)*readColor + diffuseColor;
            }else{
                finalUV = fs_uv;
            }
        }else{
            readColor = texture(textureSampler,fs_uv);
            diffuseColor = readColor;
            finalUV = fs_uv;
        }

        //Yuxin MM03 calculate the reflected color based on the light color
        diffuseColor[0] = diffuseColor[0]*u_LightCol[0];
        diffuseColor[1] = diffuseColor[1]*u_LightCol[1];
        diffuseColor[2] = diffuseColor[2]*u_LightCol[2];

        //Yuxin MM02 calculate object space transformation matrix(column based)
        mat4 objTransform;
        objTransform[0] = fs_tangent;
        objTransform[1] = fs_bitangent;
        objTransform[2] = fs_Nor;
        objTransform[3] = vec4(0,0,0,1);

        //Yuxin MM02 apply the normal map value to the vertex
        vec4 normalmapColor = texture(normalSampler, finalUV);
        //Convert normalmapColor value from 0~255 to -1~1
        normalmapColor[0] = 2*normalmapColor[0]-1;
        normalmapColor[1] = 2*normalmapColor[1]-1;
        normalmapColor[2] = 2*normalmapColor[2]-1;
        normalmapColor[3] = 0;
        vec4 finalNormal = objTransform * normalmapColor;

        float ambientTerm = 0.2;

        //==========Blinn Phong Shading Model===========//
        //max(pow(dot(H,N), shiness),0)
        vec4 halfVector = 1.0/2 * (normalize(fs_view)+normalize(fs_LightVec));
        float diffuseTerm = max(pow(dot(normalize(halfVector),normalize(finalNormal)),fs_shiness),0);
        diffuseTerm = clamp(diffuseTerm, 0, 1);
        float lightIntensity = diffuseTerm + ambientTerm;

        //==========Blinn Phong Shading Model===========//



        //==========Lambert Shading Model============//
        ///float diffuseTerm = dot(normalize(finalNormal), normalize(fs_LightVec));

        // Calculate the diffuse term for Lambert shading
        // Can delete later since we are using finalNormal calculated from normal map
        // float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
        // Avoid negative lighting values
        ///diffuseTerm = clamp(diffuseTerm, 0, 1);
        ///float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.
        //==========Lambert Shading Model=============//


        //Compute final shaded color
        out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
}
