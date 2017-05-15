# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4

import sys

flprog = sys.stdin.read() #take input from stdin
output = "" #final thing to print out

class Line():
    #line of flang code
    def __init__(self, line):
        words = line.split()
        #start processing inputs, move to function, then outputs
        phase = "inputs"
        for w in words:
            if phase == "inputs":
                if w[0] >= "a" or w[0] <= "z": #if lowercase
                    self.inputs += w + " "
                else:
                    self.inputs = self.inputs[:-1] #take out last space
                    phase = "function"
            elif phase == "function":
                self.function = w
                phase = "outputs"
            elif phase == "outputs":
                self.outputs += w + " "
        self.outputs = self.outputs[:-1]

def parse(string):
    retval = []
    lines = string.split("\n")
    lf = 1 #line number starting over for each function
    for l in lines:
        if l[0] == "@":
            output += l
        else:
            current = Line(l)
            if current.function[0] != ";" and lf == 1:
                stderr.write("Lambdas must have a ;")
                print("here")
                exit(0)

