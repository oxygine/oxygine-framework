#!/usr/bin/python

import os
def gen(path, project = ""):
	if not project:
		project = path

	projs = ("win32", "android", "macosx", "ios", "cmake", "emscripten")
	#projs = ("android", )
	#projs = ("ios", "macosx")
	#projs = ("macosx", )
	#projs = ("emscripten", )

	for platform in projs:
		dest = "../../examples/" + path + "/proj." + platform
		src =  "../../examples/" + path + "/src"
		import shutil
		shutil.rmtree(dest, True)
		cmd = "python ../gen_template.py %s -d %s -s %s -t %s" % (project, dest, src, platform)
		print(cmd)
		os.system(cmd)




if 0:
	gen("Demo")
	gen("Game/part1", "GamePart1")
	gen("Game/part2", "GamePart2")
	gen("Game/part3", "GamePart3")
	gen("Game/part4", "GamePart4")
	gen("TutorialResources")
	gen("HelloWorld")
	gen("Match3")

gen("Game/part5", "GamePart5")
#gen("DemoBox2D")