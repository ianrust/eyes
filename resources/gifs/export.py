import os
from subprocess import call

dirpath = os.path.dirname(os.path.realpath(__file__))
onlyfiles = [f for f in os.listdir(dirpath) if os.path.isfile(os.path.join(dirpath, f)) and f[-3:] == "gif"]

for f in onlyfiles:
    name = f[:-4]
    filename = os.path.join(dirpath,f)
    dest = os.path.join(dirpath,name)
    try:
        os.mkdir(dest)
        command = "convert %s -coalesce %s/%s%%03d.png" % (filename, dest, name)
        call(command.split())
    except OSError:
        print "Directory %s exists, skipping" % name
        # print "Directory %s exists, clearing contents" % name

        command = "rm -rf %s/*" % dest
        # call(command.split())
        
    # command = "convert %s -coalesce %s/%s%%03d.png" % (filename, dest, name)
    # call(command.split())