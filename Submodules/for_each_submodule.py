import os
import subprocess
import sys

def run_git_command(command, repo='.') :
    git_query = subprocess.Popen(command, shell=True, cwd=repo, stdin=subprocess.PIPE, stderr=subprocess.PIPE, env=os.environ)
    (_, error) = git_query.communicate()
    res = git_query.poll()
    if(res != 0) :
        print(error)
    return res

def get_submodules():
    res = []
    with os.scandir('.') as it:
        for entry in it:
            if entry.is_dir():
                res.append(entry)
    return res

def for_each_submodule(commands):
    working_dir = os.getcwd()
    for submodule_dir in get_submodules() :
        for command in commands:
            submodule_abs_path = os.path.join(working_dir, submodule_dir.name)
            run_git_command(command, repo=submodule_abs_path)

def main(argv):
    for_each_submodule(commands = [argv])

if __name__ == "__main__":
   main(sys.argv[1:])