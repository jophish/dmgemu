import json
from pprint import pprint

data = json.load(open('opcodes.json'))

def fixStr(string):
    if "(" in string and ")" in string:
        string = string[:string.index("(")] + "IND_" + string[string.index("(")+1:string.index(")")] + string[string.index(")")+1:]
    return string.replace("+", "I").replace("-", "D")


for op in data['cbprefixed'].keys():
    cycles = data['cbprefixed'][op]['cycles']
    out = "{" + op + ", " + str(cycles[0]) + ", "
    if len(cycles) == 2:
        out += str(cycles[1]) + ", "
    else:
        out += "0, "
    out += str(data['cbprefixed'][op]['length']) + ",  false, "

    if 'operand1' in data['cbprefixed'][op].keys():
        out += fixStr("ARG_"+data['cbprefixed'][op]['operand1'].upper()) + ", "
    else:
        out += "ARG_NONE, "

    if 'operand2' in data['cbprefixed'][op].keys():
        out += fixStr("ARG_" + data['cbprefixed'][op]['operand2'].upper()) + ", "
    else:
        out += "ARG_NONE, "

    out += '"'+data['cbprefixed'][op]['mnemonic'].lower() + '"'+", OP_TMP},"

    print(out)


