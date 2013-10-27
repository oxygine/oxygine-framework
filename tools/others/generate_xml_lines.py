import os
import shutil
import glob

def gen_xml(args):
    wildcard = "*.*"
    path = args.data + "/" + args.input
    filelist = glob.glob(path + "/" + args.wildcard)
    
    dest = open(args.data + "/" + args.out, "w")
    write = dest.write
       

    write("<resources>\n")
    write("\t<set path=\"%s\"/>\n" % (args.input, ))
    write("\t<set scale_factor=\"%s\"/>\n" % (args.sfactor, ))
    if not args.load:
        write("\t<set load=\"false\"/>\n")
    else:
        write("\t<set load=\"true\"/>\n")
            
    
    if not args.atlasses:
        write("\t<atlas>\n")        

    for file in filelist:        
        name = os.path.split(file)[1]
        if args.atlasses:
            write("\t<atlas>\n\t")
        write("\t<image file='%s' cols=\"1\"/>\n" % (name))
        if args.atlasses:
            write("\t</atlas>\n")
            
    if not args.atlasses:
        write("\t</atlas>\n")        
        
    write("</resources>\n")    
    dest.close()

    
if __name__ == "__main__":

    import argparse	
    parser = argparse.ArgumentParser(description = "generates xml file with image resources")
    parser.add_argument("-d", "--data", help = "data folder", default = ".", required = True)
    parser.add_argument("-s", "--sfactor", help = "scale factor", default = "1")
    parser.add_argument("-i", "--input", help = "input folder with images", default = ".", required = True)
    parser.add_argument("-o", "--out", help = "new output file", default = "out.xml")
    parser.add_argument("-w", "--wildcard", help = "default is '*.png'", default = "*.png")
    parser.add_argument("-l", "--load", help = "preload files?", action="store_true")
    parser.add_argument("-a", "--atlasses", help = "separate atlasses for each file?", type=bool, default = False)


    args = parser.parse_args()
    gen_xml(args)