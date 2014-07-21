import cStringIO
lines = open("../../../SDL/include/SDL_opengl.h").readlines()
funcs = ["glUseProgram", 
         "glVertexAttribPointer", 
         "glActiveTexture", 
         "glEnableVertexAttribArray", 
         "glDisableVertexAttribArray", 
         "glDeleteProgram", 
         "glGetShaderiv", 
         "glGetShaderInfoLog", 
         "glCreateShader", 
         "glCreateProgram", 
         "glAttachShader", 
         "glShaderSource", 
         "glCompileShader", 
         "glBindAttribLocation", 
         "glLinkProgram", 
         "glUniform1i", 
         "glUniform4fv", 
         "glUniform1f", 
         "glUniformMatrix4fv", 
         "glBindFramebuffer", 
         "glGenFramebuffer", 
         "glGenFramebuffers", 
         "glCheckFramebufferStatus", 
         "glDeleteFramebuffers", 
         "glGetUniformLocation", 
         "glFramebufferTexture2D", 
         "glCompressedTexImage2D", 
         "glBindBuffer",
         "glGenBuffers",
         "glBufferData",
         "glDeleteBuffers"]


#GLAPI void APIENTRY glDeleteProgram (GLuint program);
#PFNGLDELETEPROGRAMPROC

base = cStringIO.StringIO()
init = cStringIO.StringIO()

def get(name):
	shname = name[2:len(name)]
	up = shname.upper()	
	proc = "PFNGL" + up + "PROC"
	nm = " " + name + " "
	for l in lines:
		if l.find(nm) != -1:
			st = l
			if 0:
				st = ""				
				
			st = st[0:-2]
			
			base.write("%s _%s = 0;\n" % (proc, name))
			base.write(st + "\n")
			base.write("{\n")
			
			args = st[st.find("(") + 1:st.find(")")]
			args = args.split(",")
			
			allargs = []
			for ar in args:
				sp = ar.split(" ")
				q = sp[-1].split("*")[-1]
				if q == "void":
					continue
				allargs.append(q)
			
			base.write("\t")
			if not st.startswith("GLAPI void"):
				base.write("return ");				
			base.write("_%s(" %(name, ))
			
			d = ""
			for ar in allargs:
				d += ar + ","
				
			d = d[0:-1] + ");\n"
			
			base.write(d)				

			base.write("}\n")			
			init.write("_%s = (%s)SDL_GL_GetProcAddress(\"%s\");\n" % (name, proc, name))
			
			#st.replace(name, )
			
for f in funcs:
	get(f)
	
	
print base.getvalue()
print init.getvalue()