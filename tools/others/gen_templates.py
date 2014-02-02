import os
def gen(path, project = ""):
	if not project:
		project = path

	for platform in ["win32", "android", "macosx", "ios"]:
		dest = "../../examples/" + path + "/proj." + platform
		src =  "../../examples/" + path + "/src"
		import shutil
		shutil.rmtree(dest, True)
		cmd = "python ../gen_template.py %s -d %s -s %s -t %s" % (project, dest, src, platform)
		print cmd
		os.system(cmd)




gen("Demo")
#gen("DemoBox2D")
gen("Game/part1", "GamePart1")
gen("Game/part2", "GamePart2")
gen("Game/part3", "GamePart3")
gen("Game/part4", "GamePart4")
gen("HelloWorld")
gen("Match3")
