import os
import sys
import shutil
import subprocess
from timeit import timeit

scripts_dir = os.path.dirname(os.path.realpath(__file__))
build_tools_dir = os.path.abspath(os.path.join(scripts_dir, os.pardir))
root_dir = os.path.abspath(os.path.join(build_tools_dir, os.pardir))
build_dir = os.path.join(root_dir, "build")
bin_dir = os.path.join(build_dir, "bin")
release_dir = os.path.join(bin_dir, "Release")
generator = "Visual Studio 15 2017 Win64"

def change_dir_noexcept(path):
    try:
        os.chdir(path)
    except:
        os.chdir(root_dir)

def call_in_directory(dir, fn, *args, **kwargs):
    if not os.path.exists(dir):
        os.makedirs(dir)
    prevWorkingDir=os.getcwd()
    os.chdir(dir)
    try:
        return fn(*args, **kwargs)
    finally:
        change_dir_noexcept(prevWorkingDir)

def delete_build():
    if os.path.exists(build_dir) :
        call_in_directory(root_dir, shutil.rmtree, build_dir, ignore_errors=True)

def generate_project(options):
    memoryProfilingCmd = ["cmake", "-G", generator, root_dir]
    memoryProfilingCmd.extend(options)
    call_in_directory(build_dir, subprocess.check_call, memoryProfilingCmd, stderr=subprocess.STDOUT, shell=True)

def build_project():
    buildCmd = ["cmake", "--build", ".", "--target", "ALL_BUILD", "--config", "Release"]
    call_in_directory(build_dir, subprocess.check_call, buildCmd, stderr=subprocess.STDOUT, shell=True)

def run_launcher(args):
    subprocess_args = ['Launcher']
    subprocess_args.extend(args)
    stmt_str = "subprocess.check_call({0}, stderr=subprocess.STDOUT, shell=True)".format(subprocess_args)
    return call_in_directory(release_dir, timeit, stmt = stmt_str, setup = "import subprocess", number=1)
