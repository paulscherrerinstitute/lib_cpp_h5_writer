import sys
from os import listdir
from os.path import isfile, join
import h5py as h5
# import numpy as np

# Read H5 file
mypath =sys.argv[1].split("=")[1]
# gets list of files
list_of_files = [f for f in listdir(mypath) if isfile(join(mypath, f))]
# removes anything but h5
list_of_files = [f for f in list_of_files if (".h5" in f)]

# print("Total of %d files will be analyse. Files found under the folder %s: " % (len(list_of_files), mypath))
# for i, name_h5_file in enumerate(list_of_files):
    # print("%d: %s " % (i,name_h5_file))

list_of_frames = []
for file_h5_output in list_of_files:
    # print(mypath+file_h5_output)
    f = h5.File(mypath+file_h5_output, "r")
    # Get and print list of datasets within the H5 file
    datasetNames = [n for n in f.keys()]
    # opens the database
    for i in f['measurement']['acquisition']['data']['frame']:
        list_of_frames.append(i[0])
    f.close()



print(list_of_frames)