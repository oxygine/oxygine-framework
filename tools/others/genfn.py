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
         "glBindAttribLocation",
         "glLinkProgram",
         "glUniform1i",
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
         "glDeleteBuffers"]


# GLAPI void APIENTRY glDeleteProgram (GLuint program);
# PFNGLDELETEPROGRAMPROC

base = cStringIO.StringIO()
defl = cStringIO.StringIO()
init = cStringIO.StringIO()
extern = cStringIO.StringIO()
define = cStringIO.StringIO()


init.write("""
void initGLExtensions(myGetProcAdress func)
{
    if (_glUseProgram)
        return;

""")

base.write("""
extern "C"
{
""")


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

base.write("""
}
""")

print(defl.getvalue())
print(base.getvalue())
print(init.getvalue())

open("res.cpp", "w").write(defl.getvalue() + base.getvalue() + init.getvalue())
open("res.h", "w").write(extern.getvalue() + define.getvalue())
