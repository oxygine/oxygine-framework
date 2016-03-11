/* VBlurVertexShader.glsl */
attribute  vec3 position;
attribute  vec4 color;
attribute  vec2 uv;
 
uniform mediump float step;

varying mediump vec2 v_texCoord;
varying mediump vec4 v_color;
 
void main()
{
    gl_Position = vec4(position.xyz, 1.0);
    v_texCoord = uv;
    v_color = color;
}