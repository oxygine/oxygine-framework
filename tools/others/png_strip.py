from PIL import Image
import glob

images = []
w = 0
h = 0
for g in glob.glob("*.png"):
    im = Image.open(g)
    images.append(im)
    h = max(h, im.size[1])
    w += im.size[1]
    
anim = Image.new("RGBA", (w, h))
w = 0
for im in images:
    anim.paste(im, (w, 0))
    w += im.size[0]
    
anim.save("anim.png")
    
