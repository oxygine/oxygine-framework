from string import Template
import os
import glob

def relpath(a, b):
   try:
      return os.path.relpath(a, b)
   except ValueError:
      pass   
   return a
      
def run(args):
   name = args.name
   project = "proj." + args.type + "/"
   values = {"PROJECT":name}
   
   
   try:
      os.makedirs(args.dest)
   except OSError:
      #print "folder already exists"
      #return
      pass
   
   templates_path = os.path.split(__file__)[0] + "/templates/"
   root_path = templates_path + "/../../../"
   root_path = os.path.abspath(root_path)
   
   
   dest_path = os.path.abspath(args.dest) + "/"
   
   root_path = relpath(root_path, dest_path)
   
      
   values["ROOT"] = root_path
   
   
   def files(template, path, gl):
      
      res = ""
      for src in gl:
            rel_src_path = relpath(src, dest_path)            
            res += template.replace("{file}", rel_src_path)      
            
      return res
      
   SRC = ""
   INCLUDE = ""
   
   if args.src:
      SRC = files("""<ClCompile Include="{file}" />""", args.src, glob.glob(args.src + "/*.cpp"))
      INCLUDE = files("""<ClInclude Include="{file}" />""", args.src, glob.glob(args.src + "/*.h"))      
   
   values["SRC"] = SRC
   values["INCLUDE"] = INCLUDE
   
      
   values["DATA"] = "../data"
      
   
   path = templates_path + project + "/"
   for src in os.listdir(path):

      src_path = path + src
      
      t = Template(src)
      dest = t.substitute(**values)
      
      dest_path = args.dest + "/" + dest
      print "creating file: " + dest_path
      
      src_data = open(src_path, "r").read()
      t = Template(src_data)
      dest_data = t.safe_substitute(**values)     
      
      
      dest_file = open(dest_path, "w")
      dest_file.write(dest_data)      
   

if __name__ == "__main__":
   import argparse	
   parser = argparse.ArgumentParser(description="oxygine projects template generator")
   parser.add_argument("-t", "--type", help = "chouse your IDE/build tools", 
                       choices = ["win32", ], default = "win32")
   
   parser.add_argument("-s", "--src", help = "folder with already created source files", default = "")   
   
   parser.add_argument("-d", "--dest", help = "destination location", default = ".")
  
   parser.add_argument("name")
   
   args = parser.parse_args()
   run(args)
   