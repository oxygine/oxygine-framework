#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import unicode_literals, print_function

import cStringIO

lines = open("../../../SDL/include/SDL_opengl_glext.h").readlines()
funcs = ["glShaderSource",
         "glUseProgram",
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
         "glCompileShader",
         "glDetachShader",
         "glBindAttribLocation",
         "glLinkProgram",
         "glUniform1i",
         "glUniform2f",
         "glUniform2fv",
         "glUniform3fv",
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
         "glDeleteBuffers"
         "glGetProgramInfoLog",
         "glGetProgramiv",
         "glGenerateMipmap",
         "glStencilOpSeparate",
         "glGenRenderbuffers",
         "glBindRenderbuffer",
         "glRenderbufferStorage",
         "glFramebufferRenderbuffer",
         "glDeleteRenderbuffers",
         "glDeleteShader",
         "glDeleteBuffers",
         "glGetProgramInfoLog",
         "glBlendEquation",
         "glBlendFuncSeparate",
         "glBlendEquationSeparate",
         "glGetAttribLocation",
         ]


# GLAPI void APIENTRY glDeleteProgram (GLuint program);
# PFNGLDELETEPROGRAMPROC

base = cStringIO.StringIO()
defl = cStringIO.StringIO()
init = cStringIO.StringIO()
extern = cStringIO.StringIO()
define = cStringIO.StringIO()


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

            # PFNGLSHADERSOURCEPROC _glShaderSource = 0;
            base.write("%s _%s = 0;\n" % (proc, name))
            # base.write(st + "\n")

            extern.write("extern %s _%s;\n" % (proc, name))
            define.write("#define ox%s DECLARE_GLEXT(%s)\n" % (name, name))
            # extern.write(st + "\n")

            stf = st.replace(name, "def_" + name)

            defl.write(stf + "\n")
            # base.write("{\n")
            defl.write("{")

            if "void " not in stf:
            	defl.write("return 0;")  

            args = st[st.find("(") + 1:st.find(")")]
            args = args.split(",")

            allargs = []
            for ar in args:
                sp = ar.split(" ")
                q = sp[-1].split("*")[-1]
                if q == "void":
                    continue
                allargs.append(q)

            """
            base.write("\t")
            if not st.startswith("GLAPI void"):
                base.write("return ");
                defl.write("return 0;")
            base.write("_%s(" %(name, ))

            d = ""
            for ar in allargs:
                d += ar + ","

            d = d[0:-1] + ");\n"

            base.write(d)

            base.write("}\n")
            """
            defl.write("}\n")

            init.write("GETFUNC(_%s, def_%s, %s, \"%s\");\n" %
                       (name, name, proc, name))

            # st.replace(name, )

for f in funcs:
    get(f)

print(defl.getvalue())
print(base.getvalue())
print(init.getvalue())


def replace(src, block_begin, block_end, data):
	start = src.find(block_begin) + len(block_begin) + 1
	end = src.find(block_end)
	src = src[0:start] + data + src[end:]

	return src

HEADER = "../../oxygine/src/core/gl/oxgl.h"
CPP    = "../../oxygine/src/core/gl/oxgl.cpp"

header = open(HEADER, "r").read()
cpp    = open(CPP,    "r").read()

header = replace(header, "//!--pfngl-begin--!", "//!--pfngl-end--!", extern.getvalue())	
header = replace(header, "//!--oxgl-begin--!",  "//!--oxgl-end--!",  define.getvalue())	

cpp = replace(cpp, "//!--defgl-begin--!", "//!--defgl-end--!", defl.getvalue())	
cpp = replace(cpp, "//!--_gl-begin--!",  "//!--_gl-end--!",  base.getvalue())	
cpp = replace(cpp, "//!--getfunc-begin--!",  "//!--getfunc-end--!",  init.getvalue())	

open(HEADER, "w").write(header)
open(CPP, "w").write(cpp)
