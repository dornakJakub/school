import argparse
import sys

def main():
    parse_args()
    return 0
    
def parse_args():
    arg_parser = argparse.ArgumentParser(description="Script of type filter loads source code in SOL25 from standard input, checks lexical, syntactic and static semantics and writes XML representation of abstract syntactic tree of program to standard output.")
    args, unknown = arg_parser.parse_known_args()
    
    if unknown:
        sys.exit(10)
        
if __name__ == "__main__":
    main()