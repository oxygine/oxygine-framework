//shared data between vs/ps
varying mediump vec2 result_pos;
varying lowp vec4 result_color;
varying mediump vec2 result_uv;
varying mediump vec2 result_uv2;

uniform lowp sampler2D base_texture;
uniform lowp sampler2D normal_texture;
uniform mediump vec2 light;

void main()
{	
	lowp vec4 base = texture2D(base_texture, result_uv);	
	lowp vec3 normal = texture2D(normal_texture, result_uv2).xyz * 2.0 - 1.0;


	mediump vec3 dir = normalize(vec3(result_pos.xy, 0.0) - vec3(light, 200.0));


	dir = vec3(-dir.x, dir.y, -dir.z);
	

	lowp float l = max(dot(normal, dir), 0.0);

	gl_FragColor = vec4(l * base.rgb, base.a);
}