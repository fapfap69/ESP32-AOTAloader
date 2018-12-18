#!/usr/bin/env python

# Program to build CRC32 hash string of a file
#  A.Franco - CopyLeft 2018
#  ver 0.1    13/12/2018
import binascii
import sys
print("*** Make CRC32 of a file ...")
if len(sys.argv) < 2:
    print ("Wrong number of parameters. Almost the input file name.")
    sys.exit(1)
inpFile = sys.argv[1]
if len(sys.argv) == 3:
    outFIle = sys.argv[2]
else:
    outFile = ('.').join(inpFile.split('.')[:-1])
    if outFile == "":
        outFile = inpFile + ".crc32"
    else:
        outFile = outFile + ".crc32"
        
buf = open(inpFile,'rb').read()
buf = (binascii.crc32(buf) & 0xFFFFFFFF)
fo = open(outFile, "w")
fo.write("%08X" % buf);
fo.close()
print("Done !\n")
sys.exit(0)
 
