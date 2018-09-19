#! /usr/bin/env python

from upgrade_module import load_error_file_on_xml
import sys, getopt

def main(argv):
    errorfile = ""
    pointingxmlfile = ""
    receiver = ""

    try:
        opts, args = getopt.getopt(argv,"hge:p:r:",["errorfile=", "pointingxmlfile=", "receiver="])
    except getopt.GetoptError:
        print 'PMUpdate -e <errorfile> -p <pointingxmlfile> -r <receiver> -g'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'PMUpdate -e <errorfile> -p <pointingxmlfile> -r <receiver> -g '
            sys.exit()
        elif opt == "-g":
            from PMUpdate_main import main as main_gui
            main_gui()
            sys.exit()
        elif opt in ("-e", "--errorfile"):
            errorfile = arg
        elif opt in ("-p", "--pointingfile"):
            pointingxmlfile = arg
        elif opt in ("-r", "--receiver"):
            receiver = arg

    if errorfile == "" or pointingxmlfile == "" or receiver == "":
        print 'PMUpdate -e <errorfile> -p <pointingxmlfile> -r <receiver> -g'
        sys.exit("ERRORE: all parameters required")

    error_code , result = load_error_file_on_xml(errorfilename=errorfile, pointingxmlfilename=pointingxmlfile, receivername=receiver)
    if error_code:
        sys.exit("ERRORE: " + result)

if __name__ == "__main__":
   main(sys.argv[1:])


