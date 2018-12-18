#!/usr/bin/python

# Program to build SHA256 hash string of a file
#  A.Franco - CopyLeft 2018
#  ver 0.1    13/12/2018
import hashlib
import sys
print("*** Make di SHA256 file ...")
if len(sys.argv) < 2:
    print ("Wrong number of parameters. Almost the input file name.")
    sys.exit(1)
inpFile = sys.argv[1]
if len(sys.argv) == 3:
    outFIle = sys.argv[2]
else:
    outFile = ('.').join(inpFile.split('.')[:-1])
    if outFile == "":
        outFile = inpFile + ".sha256"
    else:
        outFile = outFile + ".sha256"
sha256_hash = hashlib.sha256()
with open(inpFile,"rb") as f:
    # Read and update hash string value in blocks of 4K
    for byte_block in iter(lambda: f.read(4096),b""):
        sha256_hash.update(byte_block)
fo = open(outFile, "w")
fo.write(sha256_hash.hexdigest())
fo.close()
print("Done !\n")
sys.exit(0)