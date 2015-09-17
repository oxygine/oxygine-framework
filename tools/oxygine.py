#this file is part of oxygine-framework. Copy it from oxygine-framework/tools/ to your folder and import for usage
VERSION = 1
_helper = None
def helper(root = ""):
    global _helper
    if not _helper:        
        if not root:
            import os
            if "OXYGINE" in os.environ:
                root = os.environ["OXYGINE"] + "/"
            else:
                print("OXYGINE env variable not set. Searching...")
                cur = os.path.split(__file__)[0]
                while cur:
                    parent = os.path.split(cur)[0]
                    if parent == cur:
                        break
                    cur = parent
                    
                    ox = cur + "oxygine-framework"
                    if os.path.isdir(ox):
                        print("oxygine-framework folder found at " + ox)
                        root = ox
                        break
                    #print cur
        import sys
        p = root + "/tools/"
        sys.path.append(p)
        import resbuild.oxygine_helper as oxygine_helper
        _helper = oxygine_helper.helper(root, VERSION)
        sys.path.remove(p)

    return _helper


#helper()