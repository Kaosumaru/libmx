#!/usr/bin/env python
import os
import sys
import glob
import subprocess
import distutils.spawn

import xml.etree.ElementTree as ET
from functools import reduce
import argparse

clang_format="clang-format"

def to_bytes(str):
    if sys.version_info >= (3,0):
        return bytes(str, "ascii")
    return bytes(str)

def callCommand(args, stdin = ""):
    """Launch given process with optional stdin input"""
    startupinfo = None
    if os.name == 'nt':
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    p = subprocess.Popen(args,
                         stdout = subprocess.PIPE,
                         stdin = subprocess.PIPE,
                         stderr = subprocess.STDOUT,
                         startupinfo = startupinfo)
    grep_stdout = p.communicate(input = to_bytes(stdin))[0]
    return grep_stdout.decode()

def listOfAllCPP():
    ret = []
    path = os.path.join(".")
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".h"):
                ret.append(os.path.join(root, file))
    return ret

def checkCPP(file):
    global clang_format
    print(file)
    xml_string = callCommand([clang_format, "-style=file", "-output-replacements-xml", file])
    if not xml_string:
        return 0
    tree = ET.ElementTree(ET.fromstring(xml_string))
    replacements = tree.findall('.//replacement')
    for replacement in replacements:
        offset = replacement.attrib['offset']
        print("Replacement at offset " + str(offset) + ": '"+ str(replacement.text) + "'")
    return len(replacements)

def fixCPP(file):
    global clang_format
    callCommand([clang_format, "-i", file])

def main():
    global clang_format
    parser = argparse.ArgumentParser(description='Validates cpp files. Run from tools folder.\nUses clang-format on all cpp/hpp files. In "info" mode, reports deviations from formatting rules, in fix "mode" automatically applies clang-format to all C++ files.')
    parser.add_argument("--mode", default="info", help="info/fix: 'info' only displays replacements, 'fix' applies them infile")
    parser.add_argument("--clangFormat", default="clang-format", help="clang-format binary, defaults to 'clang-format'")
    args = parser.parse_args()
    
    files = listOfAllCPP()
    clang_format = args.clangFormat

    if not distutils.spawn.find_executable(clang_format):
        print("Not found '" + clang_format + "'. Please install clang-format beforehand.")
        sys.exit(1)

    if args.mode == "info":
        replacements = reduce((lambda x, file: x + checkCPP(file)), files, 0)
        print("Number of replacements: " + str(replacements))
        sys.exit(replacements)

    if args.mode == "fix":
        for file in files:
            fixCPP(file)
        print("Done.")

if __name__ == "__main__":
    main()
