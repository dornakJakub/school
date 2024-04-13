import sys
import xml.etree.ElementTree as ET
import re

# General regex patterns
re_var = r'^(LF|GF|TF)@[a-zA-Z_\-&%*$!?][a-zA-Z_\-&%*$!?\d]*$'
re_symbol = r'^(LF|GF|TF)@[a-zA-Z_\-&%*$!?\d]+|(int|bool|string|nil)@.*$'
re_label = r'^[a-zA-Z_\-&%*$!?\d]+$'
re_type = r'^(int|string|bool)$'

# get_type function returns type of symbol
def get_type(symbol):
    # Regexes matching data types
    re_int = r'^int@([+-]?\d+|0x[0-9a-fA-F]+|0o[0-7]+)$'
    re_bool = r'^bool@(true|false)$'
    re_string = r'^string@.*$'
    re_nil = r'^nil@nil$'

    if re.match(re_var, symbol):
        return "var"
    elif re.match(re_int, symbol):
        return "int"
    elif re.match(re_bool, symbol):
        return "bool"
    elif re.match(re_string, symbol):
        return "string"
    elif re.match(re_nil, symbol):
        return "nil"
    elif re.match(re_type, symbol):
        return "type"
    elif re.match(re_label, symbol):
        return "label"
    else:
        sys.exit(23)
    
# check_escape function checks corectness of escape sequences
def check_escape(input_string):
    i = 0
    while i < len(input_string):
        if input_string[i] == '\\':
            if i + 3 >= len(input_string) or not input_string[i+1:i+4].isdigit():
                return False
        i += 1
    return True

# add_arg function adds xml representation of argument as subelement to it's instruction
def add_arg(parent, arg_id, arg_value):
    arg_type = get_type(arg_value)
    arg = ET.SubElement(parent, arg_id)
    arg.set("type", arg_type)
    
    non_edit = ["var", "label", "type"]
    to_edit = ["int", "bool", "string", "nil"]

    if arg_type in non_edit:
        arg.text = arg_value
    elif arg_type in to_edit:
        splitted = arg_value.split('@')
        if check_escape(splitted[1]):
            arg.text = splitted[1]
        else:
            sys.exit(23)
    else:
        sys.exit(99)

# add_instruction checks if opcode is valid and based on it checks for arguments with regular expressions.
# If both is correct creates new instruction in xml tree
def add_instruction(words, root, instruction_order):
    re_opcode = r'\w+'
    order = str(instruction_order) 
    opcode = words[0].upper()

    if not re.match(re_opcode, opcode):
        sys.exit(23)

    none = ["BREAK", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN"]
    symbol = ["DPRINT", "EXIT", "WRITE", "PUSHS"]
    label = ["CALL", "JUMP", "LABEL"]
    var = ["DEFVAR", "POPS"]
    var_symbol = ["MOVE", "INT2CHAR", "NOT", "STRLEN", "TYPE"]
    var_type = ["READ"]
    var_symbol_symbol = ["ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR"]
    label_symbol_symbol = ["JUMPIFEQ", "JUMPIFNEQ"]
    
    if opcode in var_symbol:
        if (len(words) == 3):
            if re.match(re_var, words[1]):
                if re.match(re_symbol, words[2]):
                    instruction = ET.SubElement(root, "instruction")
                    instruction.set("order", order)
                    instruction.set("opcode", opcode)

                    add_arg(instruction, "arg1", words[1])
                    add_arg(instruction, "arg2", words[2])

                    return 0
        sys.exit(23)
    if opcode in var_type:
        if (len(words) == 3):
            if re.match(re_var, words[1]):
                if re.match(re_type, words[2]):
                    instruction = ET.SubElement(root, "instruction")
                    instruction.set("order", order)
                    instruction.set("opcode", opcode)

                    add_arg(instruction, "arg1", words[1])
                    add_arg(instruction, "arg2", words[2])

                    return 0
        sys.exit(23)
    elif opcode in label:
        if (len(words) == 2):
            if re.match(re_label, words[1]):
                instruction = ET.SubElement(root, "instruction")
                instruction.set("order", order)
                instruction.set("opcode", opcode)

                add_arg(instruction, "arg1", words[1])

                return 0
        sys.exit(23)
    elif opcode in var:
        if (len(words) == 2):
            if re.match(re_var, words[1]):
                instruction = ET.SubElement(root, "instruction")
                instruction.set("order", order)
                instruction.set("opcode", opcode)

                add_arg(instruction, "arg1", words[1])

                return 0
        sys.exit(23)
    elif opcode in var_symbol_symbol:
        if (len(words) == 4):
            if re.match(re_var, words[1]):
                if re.match(re_symbol, words[2]):
                    if re.match(re_symbol, words[3]):
                        instruction = ET.SubElement(root, "instruction")
                        instruction.set("order", order)
                        instruction.set("opcode", opcode)

                        add_arg(instruction, "arg1", words[1])
                        add_arg(instruction, "arg2", words[2])
                        add_arg(instruction, "arg3", words[3])

                        return 0
        sys.exit(23)
    elif opcode in label_symbol_symbol:
        if (len(words) == 4):
            if re.match(re_label, words[1]):
                if re.match(re_symbol, words[2]):
                    if re.match(re_symbol, words[3]):
                        instruction = ET.SubElement(root, "instruction")
                        instruction.set("order", order)
                        instruction.set("opcode", opcode)

                        add_arg(instruction, "arg1", words[1])
                        add_arg(instruction, "arg2", words[2])
                        add_arg(instruction, "arg3", words[3])

                        return 0
        sys.exit(23)
    elif opcode in none:
        if (len(words) == 1):
            instruction = ET.SubElement(root, "instruction")
            instruction.set("order", order)
            instruction.set("opcode", opcode)

            return 0
        sys.exit(23)
    elif opcode in symbol:
        if (len(words) == 2):
            if re.match(re_symbol, words[1]):
                instruction = ET.SubElement(root, "instruction")
                instruction.set("order", order)
                instruction.set("opcode", opcode)

                add_arg(instruction, "arg1", words[1])

                return 0
        sys.exit(23)
    else:
        sys.exit(22)

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
    
    instruction_order = 0

    for line in sys.stdin:
        if line.startswith('#'):
            line_without_comment = " \t\n"
        elif line == "\n":
            line_without_comment = line
        else:
            line_without_comment = line.split('#', 1)[0].strip()
        if not line_without_comment.isspace():
            if not header:
                
                if line_without_comment.upper() == ".IPPCODE24":
                    header = True
                    root = ET.Element("program")
                    root.set("language", "IPPcode24")
                else:
                    sys.exit(21)
            else:
                instruction_order = instruction_order + 1
                splitted = line_without_comment.rstrip().split()
                add_instruction(splitted, root, instruction_order)
    if not header:
        sys.exit(21)

    #XML insertion
    tree = ET.ElementTree(root)
    print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
    tree.write(sys.stdout, encoding='unicode')

    return 0

if __name__ == '__main__':
    main()