#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments


uniform sampler2D InvSampler;
in vec4 fs_Col;
in vec2 fs_uv;

out vec4 out_Col;

void main()
{
    // Copy the color; there is no shading.
    //out_Col = fs_Col;
    out_Col = texture(InvSampler,fs_uv);
}
