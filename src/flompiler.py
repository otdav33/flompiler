# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4

import sys
import copy

flprog = sys.stdin.read() #take input from stdin
output = "" #final thing to print out

class Line():
    #line of flang code
    def __init__(self, line):
        self.inputs = []
        self.function = ""
        self.outputs = []
        words = line.split(" ")
        #start processing inputs, move to function, then outputs
        phase = "inputs"
        for w in words:
            if phase == "inputs":
                if w[0] >= "a" and w[0] <= "z": #if lowercase
                    self.inputs += [w]
                else:
                    phase = "function"
            if phase == "function":
                self.function = w
                phase = "outputs"
            elif phase == "outputs":
                self.outputs += [w]

def parse(string):
    global output
    retval = []
    lines = string.split("\n")
    lf = 1 #line number starting over for each function
    for l in lines:
        if l == "":
            continue
        if l[0] == "#":
            output += l
        else:
            current = Line(l)
            if current.function == "":
                sys.stderr.write("Every line must have a function.\n")
                exit(0)
            if current.function[0] == ";":
                lf = 1
                retval += [[]]
            elif lf == 1:
                sys.stderr.write("Programs must begin with lambdas.\n")
                exit(0)
            retval[-1] += [current]
            lf += 1
    return retval

scopes = parse(flprog)

def runline(line, scope):
    retval = ""
    if line.function[0] == "'":
        for o in line.outputs:
            retval += o + " = '" + line.function[1:] + "';\n"
            retval += satisfy(o)
    elif line.function[0] == "#":
        for o in line.outputs:
            retval += o + " = " + line.function[1:] + ";\n"
            retval += satisfy(o)
    elif line.function == "<":
        for o in line.outputs:
            retval += o + " = " + line.inputs[0] + ";\n"
            retval += satisfy(o)
    elif line.function == ">":
        retval += "if (" + line.inputs[0] + " > " + line.inputs[2] + ") {\n"
        temp = copy.deepcopy(scope) #copy scope
        retval += line.outputs[0] + " = " + line.inputs[1] + ";\n"
        retval += satisfy(line.outputs[0])
        retval += "} else {\n"
        temp = copy.deepcopy(scope) #copy scope again
        retval += line.outputs[1] + " = " + line.inputs[1] + ";\n"
        retval += satisfy(line.outputs[1])
        retval += "}\n"
    else:
        for operator in "+-*/%":
            if operator == line.function:
                for o in line.outputs:
                    retval += o + " = "
                    for i in line.inputs:
                        retval += i + " " + line.function + " "
                    retval = retval[:-2]
                    retval += ";\n"
                break
        else:
            args = ""
            if len(line.outputs) < 2:
                retval += line.outputs[0] + " = " + line.function + "("
            else:
                retval += line.function + "("
                for o in line.outputs:
                    args += "&" + o + ", "
            for i in line.inputs:
                args += i + ", "
            args = args[:-2]
            retval += args + ");\n"
