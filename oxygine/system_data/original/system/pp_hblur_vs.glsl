/* VBlurVertexShader.glsl */
attribute vec3 position;
attribute vec4 color;
attribute vec2 uv;
 
uniform mediump float step;


varying mediump vec4 v_color;
varying mediump vec2 v_texCoord;
varying mediump vec2 v_blurTexCoords0;
varying mediump vec2 v_blurTexCoords1;
varying mediump vec2 v_blurTexCoords2;
varying mediump vec2 v_blurTexCoords3;
varying mediump vec2 v_blurTexCoords4;
varying mediump vec2 v_blurTexCoords5;
varying mediump vec2 v_blurTexCoords6;
varying mediump vec2 v_blurTexCoords7;
varying mediump vec2 v_blurTexCoords8;
varying mediump vec2 v_blurTexCoords9;
varying mediump vec2 v_blurTexCoords10;
varying mediump vec2 v_blurTexCoords11;
varying mediump vec2 v_blurTexCoords12;
varying mediump vec2 v_blurTexCoords13;
 

void main()
{
    gl_Position = vec4(position.xy, 0, 1.0);
    mediump float z = step;

    v_color = color;
    v_texCoord = uv;
    v_blurTexCoords0 = v_texCoord + vec2(-z * 7.0, 0.0);
    v_blurTexCoords1 = v_texCoord + vec2(-z * 6.0, 0.0);
    v_blurTexCoords2 = v_texCoord + vec2(-z * 5.0, 0.0);
    v_blurTexCoords3 = v_texCoord + vec2(-z * 4.0, 0.0);
    v_blurTexCoords4 = v_texCoord + vec2(-z * 3.0, 0.0);
    v_blurTexCoords5 = v_texCoord + vec2(-z * 2.0, 0.0);
    v_blurTexCoords6 = v_texCoord + vec2(-z      , 0.0);
    v_blurTexCoords7 = v_texCoord + vec2( z      , 0.0);
    v_blurTexCoords8 = v_texCoord + vec2( z * 2.0, 0.0);
    v_blurTexCoords9 = v_texCoord + vec2( z * 3.0, 0.0);
    v_blurTexCoords10 = v_texCoord + vec2( z * 4.0, 0.0);
    v_blurTexCoords11 = v_texCoord + vec2( z * 5.0, 0.0);
    v_blurTexCoords12 = v_texCoord + vec2( z * 6.0, 0.0);
    v_blurTexCoords13 = v_texCoord + vec2( z * 7.0, 0.0);
}