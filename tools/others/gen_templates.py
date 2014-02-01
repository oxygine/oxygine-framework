import os
def gen(path, project = ""):
	if not project:
		project = path

	dest = "../../examples/" + path + "/proj.win32"
	src =  "../../examples/" + path + "/src"
	cmd = "python ../templates/gen.py %s -d %s -s %s" % (project, dest, src)
	print cmd
	os.system(cmd)
	pass




gen("Demo")
gen("DemoBox2D")
gen("Game/part1", "GamePart1")
gen("Game/part2", "GamePart2")
gen("Game/part3", "GamePart3")
gen("Game/part4", "GamePart4")
gen("HelloWorld")
gen("Match3")
