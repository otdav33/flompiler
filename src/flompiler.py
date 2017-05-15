# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4

import sys

flprog = sys.stdin.read() #take input from stdin
output = "" #final thing to print out

class Line():
    #line of flang code
    inputs = []
    function = ""
    outputs = []
    def __init__(self, line):
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
    retval = []
    lines = string.split("\n")
    lf = 1 #line number starting over for each function
    for l in lines:
        if l[0] == "@":
            output += l
        else:
            current = Line(l)
            if current.function == "":
                sys.stderr.write("Every line must have a function.\n")
                exit(0)
            if current.function[0] != ";" and lf == 1:
                print(current.inputs)
                print(current.function)
                print(current.outputs)
                sys.stderr.write("Lambdas must have a ;\n")
                exit(0)

parse(flprog)
