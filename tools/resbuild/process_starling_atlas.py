try:
    import Image
except ImportError:
    from PIL import Image
    
from . import process


class starling_atlas_Processor(process.Process):
    node_id = "starling"
    def __init__(self):
        pass
    
    def process(self, context, walker):
       
        meta = walker.root_meta
        file_doc = context._open_xml(context.src_data + walker.getPath("file"))
        
        file_root = file_doc
        image_name = file_root.getAttribute("imagePath")
        image_path = context.src_data + walker.path + image_name
        
        image = Image.open(image_path)
        image.load()
                
        meta.setAttribute("tw", str(image.size[0]))
        meta.setAttribute("th", str(image.size[1]))
