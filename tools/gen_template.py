from string import Template
import os
import glob
from mimetypes import guess_type
from mimetypes import add_type

def relpath(a, b):
   try:
      return os.path.relpath(a, b)
   except ValueError:
      pass   
   return a

def unixpath(path):
   return path.replace("\\", "/")
      
def run(args):
   name = args.name
   project = "proj." + args.type + "/"
   values = {"PROJECT":name}
   
   
   add_type("text/plain",".properties")
   add_type("text/plain",".cmd")
   add_type("text/plain",".mk")
   add_type("text/plain",".java")
   add_type("text/plain",".sln")
   add_type("text/plain",".vcxproj")
   add_type("text/plain",".filters")
   add_type("text/plain",".user")
   
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
   
   root_path = relpath(root_path, dest_path) + "/"
   
      
   values["ROOT"] = unixpath(root_path)
   
   
   def files(template, relto, path, gl):
      
      res = ""
      for src in gl:
            rel_src_path = relpath(src, relto)            
            res += template.replace("{file}", unixpath(rel_src_path))
            
      return res
      
   SRC = ""
   INCLUDE = ""
   
   if args.src:
      tmsrc = """<ClCompile Include="{file}" />"""
      tminc = """<ClInclude Include="{file}" />"""
      relto = dest_path
      if args.type == "android":
         tmsrc = "{file} "
         relto = dest_path + "/jni/src/"
         
      
      SRC = files(tmsrc, relto, args.src, glob.glob(args.src + "/*.cpp"))
      INCLUDE = files(tminc, relto, args.src, glob.glob(args.src + "/*.h"))      
   
   values["SRC"] = SRC
   values["INCLUDE"] = INCLUDE
   
      
   values["DATA"] = "../data"
   
   
   def process_folder(path, dest):
      try:
         os.mkdir(dest)
      except OSError:
         pass
      
      for src in os.listdir(path):
         src_path = path + src         
         t = Template(src)
         dest_local = t.substitute(**values)
         
         if os.path.isdir(src_path):
            
            process_folder(src_path + "/", dest + dest_local + "/")         
            continue
         
         dest_path = dest + dest_local
         
         
         from mimetypes import guess_type
         print "src " + src_path
         tp = guess_type(src_path)
         
         if tp[0].split("/")[0] == "text":         
            print "creating file: " + dest_path
            src_data = open(src_path, "r").read()
            t = Template(src_data)
            dest_data = t.safe_substitute(**values)                 
            
            dest_file = open(dest_path, "w")
            dest_file.write(dest_data)      
         else:
            print "copying file: " + dest_path
            import shutil
            shutil.copyfile(src_path, dest_path)
      
   
   top_path = templates_path + project + "/"

   process_folder(top_path, args.dest + "/")
   

if __name__ == "__main__":
   import argparse	
   parser = argparse.ArgumentParser(description="oxygine projects template generator")
   parser.add_argument("-t", "--type", help = "choose your IDE/build tools", 
                       choices = ["win32", "android"], default = "win32")
   
   parser.add_argument("-s", "--src", help = "folder with already created source files", default = "")   
   
   parser.add_argument("-d", "--dest", help = "destination location", default = ".")
  
   parser.add_argument("name")
   
   args = parser.parse_args()
   run(args)
   