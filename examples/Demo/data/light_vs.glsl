//shared data between vs/ps
varying lowp vec4 result_color;
varying mediump vec2 result_uv;
varying mediump vec2 result_uv2;
varying mediump vec2 result_pos;

uniform mediump mat4 mat;

attribute vec3 position;
attribute vec4 color;
attribute vec2 uv;
attribute vec2 uv2;

void main()
{
	gl_Position = mat * vec4(position, 1.0); 	
	result_color = color;
	result_uv = uv;
	result_uv2 = uv2;

	result_pos = position.xy;
}