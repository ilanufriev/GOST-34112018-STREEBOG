#!/bin/python3

import os
import sys
import argparse

# Arguments parsing
argp = argparse.ArgumentParser(description='An auxiliary tool for building the project.')
argp.add_argument('--build', action='store_true', help='(Re)build project')
argp.add_argument('--build-clean', action='store_true', help='Clean build files and ' + \
                                                             'build project')
argp.add_argument('--clean', action='store_true', help='Clean build files')
argp.add_argument('--pcc', action='store_true', help='Pull compile commands to the root of the project')
argp.add_argument('--dbg', action='store_true', help='Build project with debug symbols. ' + \
                                                  'Use with --build-clean.')

args = argp.parse_args();

# Variables
pwd = os.path.abspath('.')
build_dir = pwd + '/' + 'build'

def report(cmd, ret):
    if ret != 0:
        print(f'Command \'{cmd}\' returned {ret}')
    else:
        print(f'Command \'{cmd}\' OK!')

def do_cmd(cmd):
    ret = os.system(cmd)
    report(cmd, ret)

def clean(build_dir):
    if not os.path.exists(build_dir):
        return

    do_cmd(f'rm -rf {build_dir}')
    do_cmd(f'rm compile_commands.json')

def build(build_dir, debug: bool):
    if not os.path.exists(build_dir):
        do_cmd(f'mkdir build')

    if debug:
        do_cmd(f'cd {build_dir} && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .')
    else:
        do_cmd(f'cd {build_dir} && cmake .. && cmake --build .')

def build_clean(build_dir, debug: bool):
    clean(build_dir)
    build(build_dir, debug)

def pcc(build_dir):
    do_cmd(f'cp {build_dir}/compile_commands.json .')

if args.build:
    build(build_dir, args.dbg)
elif args.build_clean:
    build_clean(build_dir, args.dbg)
elif args.clean:
    clean(build_dir)
elif args.pcc:
    pcc(build_dir)