/* VBlurVertexShader.glsl */
attribute vec3 position;
attribute vec4 color;
attribute vec2 uv;
 
uniform mediump float step;

varying mediump vec2 v_texCoord;
varying mediump vec2 v_blurTexCoords[14];
 
void main()
{
    gl_Position = vec4(position.xy, 0, 1.0);
    mediump float z = step;


    v_texCoord = uv;
    v_blurTexCoords[ 0] = v_texCoord + vec2(-z * 7.0, 0.0);
    v_blurTexCoords[ 1] = v_texCoord + vec2(-z * 6.0, 0.0);
    v_blurTexCoords[ 2] = v_texCoord + vec2(-z * 5.0, 0.0);
    v_blurTexCoords[ 3] = v_texCoord + vec2(-z * 4.0, 0.0);
    v_blurTexCoords[ 4] = v_texCoord + vec2(-z * 3.0, 0.0);
    v_blurTexCoords[ 5] = v_texCoord + vec2(-z * 2.0, 0.0);
    v_blurTexCoords[ 6] = v_texCoord + vec2(-z      , 0.0);
    v_blurTexCoords[ 7] = v_texCoord + vec2( z      , 0.0);
    v_blurTexCoords[ 8] = v_texCoord + vec2( z * 2.0, 0.0);
    v_blurTexCoords[ 9] = v_texCoord + vec2( z * 3.0, 0.0);
    v_blurTexCoords[10] = v_texCoord + vec2( z * 4.0, 0.0);
    v_blurTexCoords[11] = v_texCoord + vec2( z * 5.0, 0.0);
    v_blurTexCoords[12] = v_texCoord + vec2( z * 6.0, 0.0);
    v_blurTexCoords[13] = v_texCoord + vec2( z * 7.0, 0.0);
}