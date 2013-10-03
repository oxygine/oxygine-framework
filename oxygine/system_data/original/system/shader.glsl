//shared data between vs/ps
varying lowp vec4 result_color;
varying mediump vec2 result_uv;
varying mediump vec2 result_uv2;

#ifdef VS
uniform mediump mat4 mat;
attribute vec3 position;
attribute vec4 color;
attribute vec2 uv;
attribute vec2 uv2;

void program_main_vs()
{
	gl_Position = mat * vec4(position, 1.0); 

	result_color = color;
	result_uv = uv;
#ifdef MASK
	result_uv2 = uv2;
#endif
}

#endif


#ifdef PS
uniform lowp vec4 clip_mask;
uniform lowp sampler2D base_texture;
uniform lowp sampler2D mask_texture;
uniform lowp sampler2D alpha_texture;

lowp vec4 get_base()
{
	lowp vec4 base = texture2D(base_texture, result_uv);	
#ifdef SEPARATE_ALPHA
	base.a = texture2D(alpha_texture, result_uv).r;	
#endif

#ifdef ALPHA_PREMULTIPLY
	base.rgb = base.rgb * base.a;
#endif

#ifdef MODIFY_BASE
	base = modify_base(base);
#endif

	return base;
}

lowp vec4 get_color()
{
	lowp vec4 base = get_base();


#ifdef MASK
	mediump vec2 uv2 = clamp(result_uv2, clip_mask.xy, clip_mask.zw);
	lowp vec4 mask = texture2D(mask_texture, uv2);

#ifdef SEPARATE_ALPHA
	lowp float mask_alpha = mask.r;	
#else
	lowp float mask_alpha = mask.a;
#endif

	base = base * mask_alpha;
#endif

	return base * result_color;
}


void program_main_ps()
{	
	gl_FragColor = get_color();
}
#endif
