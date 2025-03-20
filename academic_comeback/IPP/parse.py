import argparse
import sys
import lark
import xml.etree.ElementTree as ET

grammar = """
    start: program

    # Terms
    # ID: /(?!class$|self$|super$|nil$|true$|false$)[a-z_][a-zA-Z0-9_]*/
    ID: /[a-z_][a-zA-Z0-9_]*/
    DEF_ID: /:[a-z_][a-zA-Z0-9_]*/
    SEL_ID: /[a-z_][a-zA-Z0-9_]*:/
    # CID: /(?!class$|self$|super$|nil$|true$|false$)[A-Z][a-zA-Z0-9]*/
    CID: /[A-Z][a-zA-Z0-9]*/
    INT: /[+-]?[0-9]+/
    

    # Basic structures
    program: class program | 
    class: "class" CID ":" CID "{" method "}"
    method: selector block method | 
    
    # Selectors
    selector: ID | SEL_ID selectortail
    selectortail: SEL_ID selectortail | 
    
    # Blocks
    block: "[" blockpar "|" blockstat "]"
    blockpar: DEF_ID blockpar | 
    blockstat: ID ":=" expr "." blockstat | 
    
    # Expressions
    expr: exprbase exprtail
    exprtail: ID | exprsel
    exprsel: SEL_ID exprbase exprsel | 
    exprbase: INT | ID | CID | block | "(" expr ")"
    
    %import common.ESCAPED_STRING
    %ignore ESCAPED_STRING
    %ignore /\s+/
"""     

reserved_ids = {"class", "self", "super", "nil", "true", "false"}
defined_classes = {"Object", "Nil", "Integer", "String", "Block", "True", "False"}
defined_ids = {"plus", "class", "self", "super", "nil", "true", "false"}
class_inheritance = {"Object": None, "Nil": "Object", "Integer": "Object", "String": "Object", "Block": "Object", "True": "Object", "False": "Object"}

def main():
    parse_args()
    
    # Parse the input and check for lexical errors
    parser = lark.Lark(grammar, parser="lalr")
    try:
        tree = parser.parse(sys.stdin.read())
    except lark.exceptions.UnexpectedCharacters as e:
        print_error(e)
        sys.exit(21)
    except lark.exceptions.UnexpectedToken as e:
        print_error(e)
        sys.exit(22)
    except lark.exceptions.LarkError as e:
        print_error(e)
        sys.exit(99)
    
    print(tree.pretty())
    # check for semantic errors
    check_main_class(tree)
    traverse_tree(tree)
    
    
    #print the tree as XML
    root = tree_to_xml(tree)
    tree = ET.ElementTree(root)
    tree.write(sys.stdout, encoding="unicode", xml_declaration=True)

    return 0
    
def print_error(message):
    print(message, file=sys.stderr)
    
def parse_args():
    help_count = sys.argv.count("-h") + sys.argv.count("--help")
    
    if help_count > 1:
        print_error("Too many help options.")
        sys.exit(10)
    
    arg_parser = argparse.ArgumentParser(
        description="Script of type filter loads source code in SOL25 from standard input, checks lexical, syntactic and static semantics and writes XML representation of abstract syntactic tree of program to standard output.",
        add_help=False
    )
    
    arg_parser.add_argument("-h", "--help", action="store_true", help="Print this help message and exit.")
    
    args, unknown = arg_parser.parse_known_args()
    
    if unknown:
        print_error("Unknown argument")
        sys.exit(10)
        
    if args.help:
        arg_parser.print_help()
        sys.exit(0)
        
def check_main_class(tree):
    for subtree in tree.iter_subtrees():

        if subtree.data == "class":
            class_name = subtree.children[0]
            if isinstance(class_name, lark.Token) and class_name.value == "Main":
                has_run_method(subtree)
                return
  
    print_error("Missing class Main.")        
    sys.exit(31)

def has_run_method(subtree):
    for method_subtree in subtree.children[2:]:
        if isinstance(method_subtree, lark.Tree) and method_subtree.data == "method" and len(method_subtree.children) > 1:
            method_name = method_subtree.children[0]
            if isinstance(method_name, lark.Tree) and len(method_name.children) == 1 and isinstance(method_name.children[0], lark.Token) and method_name.children[0].value == "run":
                if len(method_name.children) == 1 and method_subtree.children[1].data == "block":
                    blockpar = method_subtree.children[1].children[0]
                    if blockpar.children and blockpar.children[0].type == "DEF_ID":
                        print_error("Method run has parameters")
                        sys.exit(33)
                return

  
    print_error("Main class missing method run")
    sys.exit(31)
    
def traverse_tree(tree):
    undefined_ids = set()
    undefined_classes = set()
    block_vars = set()
    call_arity = 0
    
    for subtree in tree.iter_subtrees():
        if subtree.data == "class":
            class_name = subtree.children[0]
            if isinstance(class_name, lark.Token) and class_name.type == 'CID':
                if class_name.value in defined_classes:
                    print_error(f"Class {class_name.value} redefined")
                    sys.exit(35)
                defined_classes.add(class_name.value)
                undefined_classes.discard(class_name.value)
                class_inheritance[class_name.value] = subtree.children[1].value
        
        if subtree.data == "block":
            undefined_ids -= defined_ids
            if undefined_ids:
                print_error(f"Undefined variables in block: {', '.join(undefined_ids)}")
                sys.exit(32)
            if len(block_vars) != call_arity:
                print_error(f"Block arity mismatch: {len(block_vars)} != {call_arity}")
                sys.exit(33)
            block_vars = set()
            call_arity = 0
            
            
        for child in subtree.children:
            if isinstance(child, lark.Token):
                if child.type == 'CID' and child.value not in defined_classes:
                    undefined_classes.add(child.value)
                elif child.type == 'ID':
                    if subtree.data == "selector" or subtree.data == "selectortail":
                        if child.value in reserved_ids:
                            print_error(f"Reserved word used as selector: {child.value}")
                            sys.exit(22)
                        defined_ids.add(child.value)
                    elif subtree.data == "blockstat":
                        if child.value in undefined_ids:
                            print_error(f"Undefined variable {child.value}")
                            sys.exit(32)
                        elif child.value in reserved_ids:
                            print_error(f"Assignment to reserved variable: {child.value}")
                            sys.exit(22)
                        if child.value in block_vars:
                            print_error(f"Variable {child.value} collides with block parameter")
                            sys.exit(35)
                        defined_ids.add(child.value)
                    elif subtree.data == "exprtail" and child.value in reserved_ids:
                        print_error(f"Reserved word used as selector: {child.value}")
                        sys.exit(22)
                    if child.value not in defined_ids:
                        undefined_ids.add(child.value)
                elif child.type == 'DEF_ID':
                    if subtree.data == "blockpar":
                        def_id = child.value[1:]
                        if def_id in reserved_ids:
                            print_error(f"Parameter name collides with reserved word: {def_id}")
                            sys.exit(22) 
                        elif def_id not in block_vars:
                            defined_ids.add(def_id)
                            block_vars.add(def_id)
                        else:
                            print_error(f"Variable {child.value} collides with other block parameter")
                            sys.exit(34)
                elif child.type == 'SEL_ID':
                    if subtree.data == "exprsel":
                        for x in subtree.children:
                            if isinstance(x, lark.Tree) and x.data == "exprbase":
                                call_arity += 1
    
    if undefined_classes:
        print_error(f"Undefined classes: {', '.join(undefined_classes)}")
        sys.exit(32)                            

    detect_circular_inheritance()
                        
def detect_circular_inheritance():
    def dfs(class_name, visited, stack):
        if class_name in stack:
            return True
        if class_name in visited:
            return False

        visited.add(class_name)
        stack.add(class_name)

        parent = class_inheritance.get(class_name)
        if parent and dfs(parent, visited, stack):
            return True

        stack.remove(class_name)
        return False

    visited = set()
    for class_name in class_inheritance:
        if class_name not in visited:
            if dfs(class_name, visited, set()):
                print_error(f"Circular inheritance detected for class {class_name}")
                sys.exit(35)

def tree_to_xml(node):
    if isinstance(node, lark.Tree):
        element = ET.Element(node.data)
        
        for child in node.children:
            element.append(tree_to_xml(child))
        
        return element
    
    elif isinstance(node, lark.Token):
        token_element = ET.Element(node.type)
        token_element.text = node.value
        return token_element
        
if __name__ == "__main__":
    main()