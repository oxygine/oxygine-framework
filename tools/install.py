import os
oxygine_path = os.path.realpath(".\\..\\")
cmd = "setx OXYGINE %(path)s" % {"path":oxygine_path, }
os.system(cmd)