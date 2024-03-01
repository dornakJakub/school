class Instruction:
    _instruction_list = []

    def __init__(self, opcode, num, str_arg):
        self._opcode = opcode
        self._numOfArg: int = num
        self._type: str
        self._instruction_list.append(self)
        self.Arg1 = Argument(1, "int", str_arg)

    def get_opcode(self):
        return self._opcode

    def get_list(self):
        return self._instruction_list

    def get_arg(self):
        return self.Arg1

class DefVar(Instruction):
    
    def __init__(self, num, str_arg):
        super().__init__("DEFVAR", num, str_arg)

    def execute(self):
        print("Jsem Defvar s argumentem: " + self.Arg1.get_value())

class Move(Instruction):
    
    def __init__(self, num, str_arg):
        super().__init__("Move", num, str_arg)

    def execute(self):
        print("Jsem Defvar s argumentem: " + self.Arg1.get_value())
        

class Argument:

    def __init__(self, num, typ, value):
        self._num = num
        self._typ = typ
        self._value = value

    def get_value(self):
        return self._value

class Factory:
    @classmethod
    def resolve(cls, string: str):
        if string.upper() == "DEFVAR":
            return DefVar(2, "TestFactory")
        elif string.upper() == "MOVE":
            return Move(2, "TestMove")
        else:
            return

i1 = Factory.resolve("Defvar")
i2 = Factory.resolve("Move")

print(i1.get_opcode())
print(i2.get_opcode())

for i in i1.get_list():
    print(type(i))

i1.execute()