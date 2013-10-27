
import sys

class rect:
    def __init__(self, x = 0, y = 0, w = 0, h = 0):
        self.x = x
        self.y = y
        self.w = w
        self.h = h

    def size(self):
        return self.w, self.h

    def pos(self):
        return self.x, self.y

    def xyxy(self):
        return self.x, self.y, self.get_right(), self.get_bottom()

    def get_left(self):
        return self.x

    def get_right(self):
        return self.x + self.w

    def get_bottom(self):
        return self.y + self.h

    def get_top(self):
        return self.y

    def is_empty(self):
        return self.w <=0 or self.h <= 0

    def is_contains(self, r):
        return r.x >= self.x and r.y >= self.y and\
            r.get_bottom() <= self.get_bottom() and\
            r.get_right() <= self.get_right()
    
    def is_contained(self, r):
        return self.x >= r.x and self.y >= r.y and\
               self.get_bottom() <= r.get_bottom() and\
               self.get_right() <= r.get_right()        

    def get_intersection(self, rc):
        c = rect(self.x, self.y, self.w, self.h)
        c.x = max(self.x, rc.x)
        c.y = max(self.y, rc.y)

        r = min(self.get_right(), rc.get_right())
        b = min(self.get_bottom(), rc.get_bottom())

        c.w = r - c.x
        c.h = b - c.y
        return c

    def get_clipX(self, x):
        r1 = rect(self.x, self.y, x - self.x, self.h)
        r2 = rect(x, self.y, self.get_right() - x, self.h)
        r1.w = min(self.w, max(0, r1.w))
        r2.w = min(self.w, max(0, r2.w))
        return r1, r2

    def get_clipY(self, y):
        r1 = rect(self.x, self.y, self.w, y - self.y)
        r2 = rect(self.x, y, self.w, self.get_bottom() - y)
        r1.h = min(self.h, max(0, r1.h))
        r2.h = min(self.h, max(0, r2.h))
        return r1, r2

    def __repr__(self):
        return "(%d, %d, %d, %d)" % (self.x, self.y, self.w, self.h)
        
 

class frame:
    def __init__(self, w, h):
        self.w = w
        self.h = h



class MyException(Exception):
    def __init__(self, free):
        self.free = free

class Node:
    def __init__(self, rect, atl, data):
        self.rect = rect
        self.atlas = atl
        self.data = data
        
class Atlas:
    n = 0
    def __init__(self, padding, w, h):
        self.w = w
        self.h = h
        self.free_rects = []
        self.free_rects.append(rect(padding, padding, w - padding, h - padding))
        self.nodes = []
        Atlas.n += 1
        
    @staticmethod
    def optimize(clipped):
        res = []
        append = res.append

        for r in clipped:
            add = True
            is_contained = r.is_contained
            for t in clipped:
                if t == r:
                    continue
                if is_contained(t):
                    add = False
                    break
            if add:
                append(r)       
                
        return res
        
    def save(self):
        #return
        from PIL import Image, ImageDraw
        
        #SCALE = 50
        #OFFSET = 5
        SCALE = 1
        OFFSET = 0
        def conv(*xyxy):
            return xyxy[0] * SCALE, xyxy[1] * SCALE, xyxy[2] * SCALE, xyxy[3] * SCALE
        
        def conv_in(*xyxy):        
            return xyxy[0] * SCALE + OFFSET, xyxy[1] * SCALE + OFFSET, \
                   xyxy[2] * SCALE - OFFSET, xyxy[3] * SCALE - OFFSET        
        
        im = Image.new("RGBA", (self.w * SCALE, self.h * SCALE))
        
        draw = ImageDraw.Draw(im)        
        draw.rectangle(conv(0, 0, self.w, self.h), fill="white")        

        for src_rect in self.rects:
            draw.rectangle(conv(*src_rect.xyxy()), fill = "red", outline="black")
            
        imcopy = im.copy()
        drawcopy = ImageDraw.Draw(imcopy)
        
        for fr in self.free_rects:
            rim = Image.new("RGBA", im.size)
            rimdraw = ImageDraw.Draw(rim)
            rimdraw.rectangle(conv_in(*fr.xyxy()), fill = "green", outline="black")
            mask = Image.new("RGBA", im.size, "white")
            maskdraw = ImageDraw.Draw(mask)
            maskdraw.rectangle(conv_in(*fr.xyxy()), fill = "#c01010")
            mask = mask.split()[0]

            imcopy = Image.composite(imcopy, rim, mask)        
            
        imcopy.save("png/%s.png" %(self.n, ))
        
    def add(self, w, h, data):

        dest_rect = None
        mn = 0xffffffffffffffff

        
        for r in self.free_rects:
            if r.w >= w and r.h >= h:
                #v = min((r.w - w), (r.h - h))
                #v = r.w * r.h - w * h
                v = r.y * r.x * r.x
                if v < mn:
                #if 1:
                    mn = v
                    dest_rect = r
                #    break
                #break
            
        if not dest_rect:
            #self.save()
            return None
        
        src_rect = rect(dest_rect.x, dest_rect.y, w, h)

        clipped = []
        append = clipped.append
        for r in self.free_rects:
            if r.get_intersection(src_rect).is_empty():
                append(r)
                continue
            r1, _ = r.get_clipX(src_rect.get_left())
            _, r2 = r.get_clipX(src_rect.get_right())
            r3, _ = r.get_clipY(src_rect.get_top())
            _, r4 = r.get_clipY(src_rect.get_bottom())

            if not r1.is_empty():
                append(r1)
            if not r2.is_empty():
                append(r2)
            if not r3.is_empty():
                append(r3)
            if not r4.is_empty():
                append(r4)

        self.free_rects = self.optimize(clipped)
        
        node = Node(src_rect, self, data)
        self.nodes.append(node)
        return node

if __name__ == "__main__":
    import random
    r = rect(0, 0, 10, 10)
    fr = []
    random.seed(0)
    for x in xrange(200):
        fr.append(frame(random.randint(10,60), random.randint(10,60)))
    
    
    fr = [frame(1, 2), frame(2, 3), frame(2, 3), frame(3, 3), frame(8, 2), frame(4, 1), frame(4, 2), frame(1, 1), frame(3, 3),frame(3, 3),frame(3, 3),]
    #fr = [frame(2, 2), frame(3, 3), frame(2, 2), ]
    #fr = [frame(1, 1), frame(6, 3),  frame(8, 1), frame(2,2)]
    
    #fr = fr #30223
    #fr = sorted(fr, key = lambda v: -max(v.h, v.w)) #21450
    #fr = sorted(fr, key = lambda v: -v.h * v.w) #22492
    #fr = sorted(fr, key = lambda v: -v.h) #21880
    #fr = sorted(fr, key = lambda v: -v.w) #20573
    
    im = Image.new("RGBA", (r.w * SCALE, r.h * SCALE))
    draw = ImageDraw.Draw(im)
    
    draw.rectangle(conv(0,0,r.w,r.h), fill="white")
    
    exc = ""
    atlas = Atlas(0, r.w, r.h)
    for f in fr:
        node = atlas.add(f.w, f.h)
        if not node:
            break
        
    s = 0
    for f in fr:
        s += f.w * f.h   
    
    print "left: " + str(s)
    im.save("image-%s.png" %(s, ))
    
