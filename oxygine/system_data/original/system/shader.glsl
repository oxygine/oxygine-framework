//shared data between vs/ps
varying lowp vec4 result_color;
varying mediump vec2 result_uv;
varying mediump vec2 result_uv2;


#ifdef VS
uniform highp mat4 mat;
uniform mediump vec3 msk[4];
attribute vec3 position;
attribute vec4 color;
attribute vec2 uv;

#ifdef SAMPLER_UV2
attribute vec2 uv2;
#endif

void program_main_vs()
{
	gl_Position = mat * vec4(position, 1.0); 

	result_color = color;
	result_uv = uv;

#ifdef MASK
	mediump float a = dot(msk[0], vec3(1.0, position.x, position.y));
	mediump float b = dot(msk[1], vec3(1.0, position.x, position.y));

	result_uv2.x = dot(msk[2], vec3(1.0, a, b));
	result_uv2.y = dot(msk[3], vec3(1.0, a, b));
#endif

#ifdef SAMPLER_UV2
	 result_uv2 = uv2;
#endif
}

#endif


#ifdef PS
uniform lowp vec4 clip_mask;
uniform lowp vec4 add_color;
uniform lowp sampler2D base_texture;
uniform lowp sampler2D mask_texture;
uniform lowp sampler2D alpha_texture;


lowp vec4 modify_base_pre(lowp vec4);
lowp vec4 modify_base(lowp vec4);
lowp vec4 replaced_get_base();
lowp vec4 replaced_get_color();

lowp vec4 get_base()
{
	lowp vec4 base = texture2D(base_texture, result_uv);	
	
#ifdef SEPARATE_ALPHA
	base.a = texture2D(alpha_texture, result_uv).r;	
#endif

#ifdef SEPARATE_ALPHA_UV2
	base.a = base.a * texture2D(alpha_texture, result_uv2).a;	
#endif

#ifdef MODIFY_BASE_PRE
	//define MODIFY_BASE_PRE and declare your own function modify_base_pre	
	base = modify_base_pre(base);
#endif

#ifdef ALPHA_PREMULTIPLY
	base.rgb = base.rgb * base.a;
#endif

#ifdef MODIFY_BASE
	//define MODIFY_BASE and declare your own function modify_base
	base = modify_base(base);
#endif

	return base;
}
lowp vec2 my_step(lowp vec2 a, lowp vec2 b)
{
#ifdef ANDROID
	lowp vec2 r;
	r.x = a.x < b.x  ? 1.0 : 0.0;
	r.y = a.y < b.y  ? 1.0 : 0.0;
	return r;
#else
	return step(a, b);
#endif
}

lowp vec4 get_color()
{

#ifdef REPLACED_GET_BASE
	//define REPLACED_GET_BASE and declare your own function replaced_get_base()
	lowp vec4 base = replaced_get_base();
#else
	lowp vec4 base = get_base();
#endif


#ifdef MASK
	//mediump vec2 uv2 = clamp(result_uv2, clip_mask.xy, clip_mask.zw);
	lowp vec4 mask = texture2D(mask_texture, result_uv2);

#ifdef MASK_R_CHANNEL
	lowp float mask_alpha = mask.r + 0.001;	
#else
	lowp float mask_alpha = mask.a + 0.001;
#endif

	lowp vec2 sc1 = my_step(clip_mask.xy, result_uv2.xy);
	lowp vec2 sc2 = my_step(result_uv2.xy, clip_mask.zw);
	lowp float m = mask_alpha * sc1.x * sc1.y * sc2.x * sc2.y;

	base = base * m;
#endif

#ifdef DONT_MULT_BY_RESULT_COLOR
	//for your purposes
	lowp vec4 res = base;
#else
	lowp vec4 res = base * result_color;
#endif	

#ifdef ADD_COLOR
	res = res + add_color * res.a;
#endif

	return res;
}


void program_main_ps()
{	

#ifdef REPLACED_GET_COLOR
	//define REPLACED_GET_COLOR and declare your own function replaced_get_color()
	gl_FragColor = replaced_get_color();
#else
	gl_FragColor = get_color();
#endif

}
#endif
