#!/usr/bin/python

import Image
import sys
image = sys.argv[1]
cols = int(sys.argv[2])
rows = int(sys.argv[3])

src = Image.open(image)
fw = src.size[0] / cols
fh = src.size[1] / rows

dest = Image.new("RGBA", (fw * cols * rows, fh))
w = 0
for row in xrange(rows):
    for col in xrange(cols):  
        frame = src.crop((fw * col, fh * row, fw * (col + 1), fh * (row + 1)))
        dest.paste(frame, (w, 0))
        w += fw
    
dest.save("anim.png")
print "cols = %d" % (rows * cols, )
    
