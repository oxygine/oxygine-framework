//shared data between vs/ps
varying lowp vec4 result_color;
varying mediump vec2 result_uv;
varying mediump vec2 result_uv2;

//SDF data
uniform lowp vec4 sdf_outline_color;
uniform mediump vec4 sdf_params;

#ifdef VS
uniform mediump mat4 mat;
uniform mediump vec3 msk[4];
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
	mediump float a = dot(msk[0], vec3(1.0, position.x, position.y));
	mediump float b = dot(msk[1], vec3(1.0, position.x, position.y));

	result_uv2.x = dot(msk[2], vec3(1.0, a, b));
	result_uv2.y = dot(msk[3], vec3(1.0, a, b));
#endif
}

#endif


#ifdef PS
uniform lowp vec4 clip_mask;
uniform lowp sampler2D base_texture;
uniform lowp sampler2D mask_texture;
uniform lowp sampler2D alpha_texture;

#ifdef SDF
#define DONT_MULT_BY_RESULT_COLOR
#endif

lowp vec4 get_base()
{
	lowp vec4 base = texture2D(base_texture, result_uv);	
#ifdef SEPARATE_ALPHA
	base.a = texture2D(alpha_texture, result_uv).r;	
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


lowp vec4 get_base_sdf()
{
	lowp float tx = texture2D(base_texture, result_uv).r;
     
#ifdef SDF_OUTLINE
    lowp float b =   min((tx - sdf_params.z) * sdf_params.w, 1.0);
    lowp float a = clamp((tx - sdf_params.x) * sdf_params.y, 0.0, 1.0);
	lowp vec4 res = (sdf_outline_color + (result_color - sdf_outline_color)*a) * b;
#else
	lowp float a = min((tx - sdf_params.x) * sdf_params.y, 1.0);
	lowp vec4 res = result_color * a;
#endif

    return res;
}

lowp vec4 get_color()
{

#ifdef REPLACED_GET_BASE
	//define REPLACED_GET_BASE and declare your own function replaced_get_base()
	lowp vec4 base = replaced_get_base();
#else
#	ifdef SDF
	lowp vec4 base = get_base_sdf();
#	else
	lowp vec4 base = get_base();
#	endif
#endif


#ifdef MASK
	mediump vec2 uv2 = clamp(result_uv2, clip_mask.xy, clip_mask.zw);
	lowp vec4 mask = texture2D(mask_texture, uv2);

#ifdef MASK_R_CHANNEL
	lowp float mask_alpha = mask.r + 0.001;	
#else
	lowp float mask_alpha = mask.a + 0.001;
#endif

	base = base * mask_alpha;
#endif

#ifdef DONT_MULT_BY_RESULT_COLOR
	//for your purposes
	return base;
#else
	return base * result_color;
#endif	
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
