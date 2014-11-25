#this file is part of oxygine-framework. Copy it from oxygine-framework/tools/ to your folder and import for usage
VERSION = 1
_helper = None
def helper(root = ""):
    global _helper
    if not _helper:        
        if not root:
            import os
            root = os.environ["OXYGINE"] + "/"
        import sys
        p = root + "/tools/resbuild/"
        sys.path.append(p)
        import oxygine_helper
        _helper = oxygine_helper.helper(root, VERSION)
        sys.path.remove(p)

    return _helper
