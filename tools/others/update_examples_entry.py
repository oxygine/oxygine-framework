d = [#"..\\..\\examples\\Demo",
     "..\\..\\examples\\DemoBox2D",
     #"..\\..\\examples\\GameTemplate",
     "..\\..\\examples\\HelloWorld",
     "..\\..\\examples\\Match3",
     "..\\..\\..\\oxygine-movieclip\\examples\\example1\\",
     "..\\..\\..\\oxygine-frame\\examples\\example1\\",
     "..\\..\\..\\oxygine-frame\\examples\\example2\\",
     "..\\..\\..\\oxygine-sound\\test\\",
     "..\\..\\..\\oxygine-pipeline\\example\\game\\runnable",
     ]
for dest in d:
    s = dest + "\\system"
    shutil.rmtree(s, True)
    print s
    shutil.copytree("data\\system", s)
    