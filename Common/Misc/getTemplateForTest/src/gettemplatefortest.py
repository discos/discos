import sys
import os
import shutil

usage = """
getTmplateForTest - create a test directory containing the necessary code templates
                    used for component automated testing.
                    This command should be run from within a Component directory at
                    the first level of depth, creating a 'test' directory at the same level 
                    of 'src, objects' ecc...
"""                    

def get_introot():
    introot = os.getenv("INTROOT")
    if not introot:
        msg = "ERROR: INTROOT environment variable is not defined"
        raise Exception(msg)
    return introot

def get_template_dir_name():
    introot = get_introot()
    return os.path.join(introot, "templates/test")

def get_template_dir():
    template_dir = get_template_dir_name()
    if not os.path.exists(template_dir):
        msg = "ERROR: template directory does not exist"
        raise Exception(msg)
    return template_dir

def command_line_util(args):
    if len(args) == 1:
        arg = args.pop()
        if arg in ('--help', '-h'):
            print usage
            sys.exit(0)
        else:
            target_dir = arg
    elif len(args) > 1:
        print usage
        sys.exit(0)
    else:
        target_dir = 'test' # Default target directory

    if os.path.exists(target_dir):
        msg = "ERROR: %s directory already exists." %target_dir
        raise Exception(msg)
    template_dir = get_template_dir()
    shutil.copytree(template_dir, target_dir)

