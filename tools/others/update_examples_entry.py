examples = "../../examples/"

src = examples + "Demo/src/entry_point.cpp"
parent = "../../../"

items = (
     examples + "DemoBox2D",
     examples + "Game/Part1",
     examples + "Game/Part2",
     examples + "Game/Part3",
     examples + "Game/Part4",
     examples + "HelloWorld",
     examples + "Match3",
     examples + "TutorialResources",
     parent + "oxygine-frame/examples/example1",
     parent + "oxygine-frame/examples/example2",
     parent + "oxygine-magicparticles/example/HelloWorld",
     parent + "oxygine-pipeline/example/game/project/",
     parent + "oxygine-sound/examples/example1/",
     )

for item in items:
    import shutil
    shutil.copyfile(src, item + "/src/entry_point.cpp")
    