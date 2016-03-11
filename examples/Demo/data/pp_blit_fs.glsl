/* BlurFragmentShader.glsl */
//precision mediump float;
 
uniform mediump sampler2D s_texture;
 
varying mediump vec2 v_texCoord;
 
void main()
{
    gl_FragColor = texture2D(s_texture, v_texCoord);
}