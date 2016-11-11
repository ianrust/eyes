import os
from subprocess import call

dirpath = os.path.dirname(os.path.realpath(__file__))
onlyfiles = [f for f in os.listdir(dirpath) if os.path.isfile(os.path.join(dirpath, f)) and f[-3:] == "png"]

for f in onlyfiles:
    name = f[:-4]
    filename = os.path.join(dirpath,f)
    dest = os.path.join(dirpath,name)
    try:
        os.mkdir(dest)
    except OSError:
        print "Direcory %s exists" % name
        
    command = "mv %s %s" % (filename, dest)
    call(command.split())