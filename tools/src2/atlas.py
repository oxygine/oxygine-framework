class rect:
    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        
    def copy(self):
        return rect(self.x, self.y, self.w, self.h)
    
class Node:
    def __init__(self, rect):
        self.rect = rect
        self._child0 = None
        self._child1 = None
        self._free = True
        
    def insert(self, w, h):
        new_node = None
        if self._child0 or self._child1:
            new_node = self._child0.insert(w, h)
            if new_node:
                return new_node
            
            new_node = self._child1.insert(w, h)
            if new_node:
                return new_node
      
            return None
        
        if not self._free:
            return None
        
        if w > self.rect.w:
            return None
        
        if h > self.rect.h:
            return None
        
        if w == self.rect.w and h == self.rect.h:
            return self
        
        self._child0 = Node(self.rect.copy())
        self._child1 = Node(self.rect.copy())
        
        
        dw = self.rect.w - w
        dh = self.rect.h - h
        
        if dw > dh:
            self._child0.rect.w = w
            
            self._child1.rect.x = self.rect.x + w
            self._child1.rect.w = self.rect.w - w            
        else:
            self._child0.rect.h =  h            
            
            self._child1.rect.y = self.rect.y + h
            self._child1.rect.h = self.rect.h - h
            if self._child1.rect.h == 934:
                q = 0
            
            
        return self._child0.insert(w, h)
             
  
class Atlas:
    def __init__(self, w, h,):        
        self.w = w        
        self.h = h
        self._tree = Node(rect(0, 0, w, h))
        
    def add(self, w, h):
        r = self._tree.insert(w, h)
        if r:
            r._free = False
        return r
        
          
          
            
            