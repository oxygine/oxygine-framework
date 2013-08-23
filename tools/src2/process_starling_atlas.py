from PIL import Image
import process


class starling_atlas_Processor(process.Process):
    node_id = "starling_atlas"
    def __init__(self):
        pass
    
    def process(self, context, el):
        file_name = el.getAttribute("file")         
        
        meta = context.add_meta(self.node_id)
        file_doc = context._open_xml(context.get_current_src_path(file_path))
        
        file_root = file_doc
        image_name = file_root.getAttribute("imagePath")
        image_path = context.get_current_src_path(image_name)
        
        image = Image.open(image_path)
        image.load()
                
        meta.setAttribute("tw", str(image.size[0]))
        meta.setAttribute("th", str(image.size[1]))