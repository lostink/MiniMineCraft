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


// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;

//Yuxin MM02
in vec2 fs_uv;
in vec4 fs_tangent;
in vec4 fs_bitangent;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

void main()
{
    // Material base color (before shading)
        //vec4 diffuseColor = fs_Col;

        //Yuxin MM02 calculate the color based on texture sampler
        vec4 diffuseColor = texture(textureSampler,fs_uv);

        //Yuxin MM02 calculate object space transformation matrix(column based)
        mat4 objTransform;
        objTransform[0] = fs_tangent;
        objTransform[1] = fs_bitangent;
        objTransform[2] = fs_Nor;
        objTransform[3] = vec4(0,0,0,1);

        //Yuxin MM02 apply the normal map value to the vertex
        vec4 normalmapColor = texture(normalSampler, fs_uv);
        //Convert normalmapColor value from 0~255 to -1~1
        normalmapColor[0] = 2*normalmapColor[0]-1;
        normalmapColor[1] = 2*normalmapColor[1]-1;
        normalmapColor[2] = 2*normalmapColor[2]-1;
        normalmapColor[3] = 0;
        vec4 finalNormal = objTransform * normalmapColor;
        float diffuseTerm = dot(normalize(finalNormal), normalize(fs_LightVec));

        // Calculate the diffuse term for Lambert shading
        //float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.2;

        float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.

        //Compute final shaded color
        out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
        //======Below for debug purpose, can delete later======//
        //out_Col = texture(textureSampler,fs_uv);
        //out_Col = normalmapColor;
        //out_Col = finalNormal;
        //out_Col = textureSpaceNormal;
}
