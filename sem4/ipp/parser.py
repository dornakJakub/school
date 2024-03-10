import sys
import xml.etree.ElementTree as ET
import re

def main():
    #Argument parsing
    help = """ 
    Skript parser.py v jazyce python 3.10 nacte ze vstupu soubor obsahujici kod v IPP24. Zkontroluje jeho lexikalni a syntaktickou spravnost a vypise XML reprezentaci kodu na standartni vystup.
    
    Pouziti:
    python3 parser.py < input > output
    """
    if len(sys.argv) > 1:
        if len(sys.argv) == 2 and (sys.argv[1] == '--help' or sys.argv[1] == '-h'):
            print(help)
            return 0
        else:
            sys.exit(10)
    
    header = False
    
    #XML header insertion
    print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")

    for line in sys.stdin:

        #IPPcode24 header check
        if not header:
            if line == '.IPPcode24\n':
                header = True
            else:
                sys.exit(21)

        splitted = line.rstrip().split()
        print(splitted)

        

    return 0

if __name__ == '__main__':
    main()