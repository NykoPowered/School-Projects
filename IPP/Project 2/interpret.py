#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import sys, re, getopt

#pole instrukcii
instructions = ['MOVE','CREATEFRAME','PUSHFRAME','POPFRAME','DEFVAR','CALL','RETURN','PUSHS','POPS','ADD','SUB','MUL','IDIV','LT',
              'GT','EQ','AND','OR','NOT','INT2CHAR','STRI2INT','READ','WRITE','CONCAT','STRLEN','GETCHAR','SETCHAR','TYPE','LABEL','JUMP','JUMPIFEQ','JUMPIFNEQ','DPRINT','BREAK'];

types = ['var','int','type','bool','string','label'];




def InstuctionArgumentCounter(args, count):		#funkce pro kontrolu argumentu instrukce
	if(len(args) != count):
		sys.exit(52)

def checkInteger(digit):		#kontrola ci je to integer
	try:
		int(digit)
	except:
		sys.exit(53)
	return digit;

def checkBoolean(value):		#kontrola ci je to boolean
	if value == "true":
		return True;
	elif value == "false":
		return False;
	else:
		sys.exit(53)

def checkVariable(variable):
	if "GF@" in variable or "TF@" in variable or "LF@" in variable:			#kontrola promenne
		return variable
	else:
		sys.exit(53)

def checkString(string):			#kontrola stringu
	if isinstance(string, str):
		esc = re.findall("\\\\\d\d\d", string)
		for escape_seq in esc:
			string = string.replace(escape_seq, chr(int(escape_seq.lstrip('\\'))))
		return string
	else:
		sys.exit(53)

def CheckArguments(argv):				#kontrola argumentu skriptu
	outputfile = ''
	try:
		opts, args = getopt.getopt(sys.argv[1:],"",["help","source=","input="])
	except getopt.GetoptError:
		sys.exit(10)
	for opt, arg in opts:
		if (opt == "--help" and len(opts) == 1):
			print("Skript interpret.py, verze python3.6")
			print("Spusteni: python3.6 interpret.py --help | --source=file | input=file")
			print("--help vypise napovedu")
			sys.exit(0)
		elif opt == "--source":
			outputfile = arg
			return outputfile
		elif opt == "--input":
			sys.exit(10)
		else:
			sys.exit(10)

def InstructionArgumentChecker(child):				#kontrola argumentu instrrukci
	ArgumentCounter=1
	for subchild in child:
		if(subchild.tag != "arg"+str(ArgumentCounter)) or (subchild.attrib["type"] not in types):
			sys.exit(31)
		if subchild.attrib["type"] != "string" and subchild.text is None:
			sys.exit(32)
		elif (subchild.text is not None and " " in subchild.text) and (subchild.attrib["type"] == "string" or subchild.attrib["type"] == "int"):
			sys.exit(31)
		ArgumentCounter+=1

def checkArgumentType(type, ArgumentType):			#kontrola type argumentu
	if(type != ArgumentType):
		sys.exit(32)

def getArgumentType(ArgumentType):		#funkce pro ziskani typu argumentu
	if(ArgumentType == "var"):
		return ArgumentType
	elif(ArgumentType == "int"):
		return ArgumentType
	elif(ArgumentType == "bool"):
		return ArgumentType
	elif(ArgumentType == "string"):
		return ArgumentType
	elif(ArgumentType == "label"):
		return ArgumentType
	elif(ArgumentType == "type"):
		return ArgumentType
	else:
		sys.exit(32)

def loadInput(arg):				#nacitani xml reprezentace
	try:
		tree = ET.parse(arg)
		xmlroot = tree.getroot()
	except:
		sys.exit(31)
	else:
		return xmlroot

def XMLChecker(root):		#kontrola ci je XML vstup spravny
	if(root.tag != "program") or ('language' not in root.attrib) or (root.attrib['language']  !=  "IPPcode19"):
		sys.exit(31)
	order = 1
	for ch in xmlroot:
		if(ch.tag != "instruction") or (ch.attrib["opcode"] not in instructions) or (ch.attrib["order"] != str(order)):
			sys.exit(31)
		order += 1

def getValueType(value):			#funkce na ziskani typu hodnoty
	if isinstance(value, str) and value not in ['true', 'false'] or value == None:
		return "string"
	elif isinstance(value, int):
		return "int"
	elif isinstance(value, str):
		return "bool"

def variableTypeChecker(variable):		#kontrola typu promenne
	if type(variable) is int:
		return "int";
	else:
		if isinstance(variable, str) and variable not in ['true', 'false', 'True', 'False'] or variable == None:
			return "string";
		elif (isinstance(variable, bool)) or (isinstance(variable, str) and variable in ['true', 'false', 'True', 'False']):
			return "bool";
		elif variable.isdigit():
			return "int";

def checkLabel(label, labelArray):		#kontrola labelu
	if label not in labelArray:
		sys.exit(52)

def getValue(value, type, label):		#funkce na ziskani hodnoty
	if type == "var":
		checkVariable(value.text)
		checkArgumentType("var", value.attrib["type"])
		return value.text;
	elif type == "int":
		checkInteger(value.text)
		checkArgumentType("int", value.attrib["type"])
		return int(value.text);
	elif type == "bool":
		checkBoolean(value.text)
		checkArgumentType("bool", value.attrib["type"])
		return value.text;
	elif type == "string":
		if value.text == None:
			return ""
		checkString(value.text)
		checkArgumentType("string", value.attrib["type"])
		return value.text;
	elif type == "type":
		if value.text not in ['int', 'bool', 'string']:
			sys.exit(52)
	elif type == "label":
		checkLabel(value.text, label)
		return value.text
	else:
		sys.exit(32)

def getVariableFrame(frameType, variableName, globalFrame, localFrame, temporaryFrame):		#funkce na ziskani framu
	if frameType == "GF":
		if variableName not in globalFrame:
			sys.exit(54)

	elif frameType == "TF":
		if temporaryFrame == None:
			sys.exit(55)
		if variableName not in temporaryFrame:
			sys.exit(54)

	elif frameType == "LF":
		if localFrame == None:
			sys.exit(55)
		if variableName not in localFrame:
			sys.exit(54)

def getVariable(variableName, globalFrame, localFrame, temporaryFrame):					#funkce na ziskani promenne
	if variableName in globalFrame:
		return globalFrame[variableName]
	elif localFrame is not None and variableName in localFrame:
		return localFrame[variableName]
	elif temporaryFrame is not None and variableName in temporaryFrame:
		return temporaryFrame[variableName]
	else:
		sys.exit(54)

def defVariable(frameName, variableName, globalFrame, localFrame, temporaryFrame):		#funkce pro def. promenne
	if frameName == "GF":
		globalFrame[variableName] = None

	elif frameName == "TF":
		if(temporaryFrame == None):
			sys.exit(55)
		else:
			temporaryFrame[variableName] = None

	elif frameName == "LF":
		if(localFrame == None):
			sys.exit(55)
		else:
			localFrame[variableName] = None
	else:
		sys.exit(55)

def setValue(frameName, varName, value, globalFrame, localFrame, temporaryFrame):		#priradenie hodnoty promenne
	if frameName == "GF":
		globalFrame[varName] = value
	elif frameName == "TF":
		temporaryFrame[varName] = value
	elif frameName == "LF":
		localFrame[varName] = value

def operatorRelational(val, values, globalFrame, localFrame, temporaryFrame, Labels):			#oprace s relacnimi operatory
	InstuctionArgumentCounter(val, 3)
	values.append(getValue(val[0], "var", Labels))
	getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
	typ1 = getArgumentType(val[1].attrib["type"])
	typ2 = getArgumentType(val[2].attrib["type"])
	if typ1 == 'int' or typ1 == 'string' or typ1 == 'bool' or typ1 == 'var':
		values.append(getValue(val[1], typ1, Labels))
		if typ1 == 'var':
			getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
			values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
	else:
		sys.exit(53)
	if typ2 == 'int' or typ2 == 'string' or typ2 == 'bool' or typ2 == 'var':
		values.append(getValue(val[2], typ2, Labels))
		if typ2 == 'var':
			getVariableFrame(values[2][:2], values[2][3:], globalFrame, localFrame, temporaryFrame)
			values[2] = getVariable(values[2][3:], globalFrame, localFrame, temporaryFrame)
	else:
		sys.exit(53)
	if(type(values[1]) != type(values[2])):
		sys.exit(53)
	return values

def operatorBoolean(val, values, globalFrame, localFrame, temporaryFrame, Labels):			#boolean operaotry
	values.append(getValue(val[0], "var", Labels))
	getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
	typ1 = getArgumentType(val[1].attrib["type"])
	typ2 = getArgumentType(val[2].attrib["type"])
	if typ1 == "bool" or typ1 == "var":
		values.append(getValue(val[1], typ1, Labels))
		if typ1 == "var":
			getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
			values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
			checkBoolean(values[1])
	else:
		sys.exit(53)
	if typ2 == "bool" or typ2 == "var":
		values.append(getValue(val[2], typ2, Labels))
		if typ2 == "var":
			getVariableFrame(values[2][:2], values[2][3:], globalFrame, localFrame, temporaryFrame)
			values[2] = getVariable(values[2][3:], globalFrame, localFrame, temporaryFrame)
			checkBoolean(values[2])
	else:
		sys.exit(53)
	getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
	return values;

def operatorClassic(val, values, globalFrame, localFrame, temporaryFrame, Labels):			#klasicke matematicke operatory
	InstuctionArgumentCounter(val, 3)
	values.append(getValue(val[0], "var", Labels))
	getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
	typ1 = getArgumentType(val[1].attrib["type"])
	typ2 = getArgumentType(val[2].attrib["type"])
	if typ1 == "int" or typ1 == "var":
		values.append(getValue(val[1], typ1, Labels))
		if typ1 == "var":
			getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
			values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
			checkInteger(values[1])
	else:
		sys.exit(53)
	if typ2 == "int" or typ2 == "var":
		values.append(getValue(val[2], typ2, Labels))
		if typ2 == "var":
			getVariableFrame(values[2][:2], values[2][3:], globalFrame, localFrame, temporaryFrame)
			values[2] = getVariable(values[2][3:], globalFrame, localFrame, temporaryFrame)
			checkInteger(values[2])
	else:
		sys.exit(53)
	getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
	return values;


def interpret(root):			#hlavni funcke na interpretaci kodu. Podle ifu zisti o jakou instrukci se jedna a provede jednotlive kontroli
	for child in root:
		global Labels, globalFrame, localFrame, temporaryFrame, Stack, StackFrame, StackFun, order
		values = []
		order += 1
		InstructionArgumentChecker(child)
		if(child.attrib["opcode"] in ['MOVE']):
			InstuctionArgumentCounter(child, 2)
			values.append(getValue(child[0], "var", Labels))
			getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
			typ = getArgumentType(child[1].attrib["type"])
			values.append(getValue(child[1], typ, Labels))
			if typ == "var":
				getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
				values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
			typ = getValueType(values[1])
			if typ == 'string':
				values[1] = checkString(values[1])
			if values[1] == None:
				sys.exit(56)

			setValue(values[0][:2], values[0][3:], values[1], globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['CREATEFRAME']):
			InstuctionArgumentCounter(child, 0)
			temporaryFrame = {}

		elif(child.attrib["opcode"] in ['PUSHFRAME']):
			InstuctionArgumentCounter(child, 0)
			if(temporaryFrame == None):
				sys.exit(55)
			StackFrame.append(temporaryFrame)
			localFrame = StackFrame[len(StackFrame) - 1]
			temporaryFrame = None

		elif(child.attrib["opcode"] in ['POPFRAME']):
			InstuctionArgumentCounter(child, 0)
			if(localFrame == None):
				sys.exit(55)
			temporaryFrame = StackFrame[0]
			StackFrame.pop(0)
			if len(StackFrame) != 0:
				localFrame = StackFrame[len(StackFrame) - 1]
			else:
				localFrame = None

		elif(child.attrib["opcode"] in ['DEFVAR']):
			InstuctionArgumentCounter(child, 1)
			values.append(getValue(child[0], "var", Labels))
			defVariable(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['CALL']):
			InstuctionArgumentCounter(child, 1)
			values.append(getValue(child[0], "label", Labels))
			if values[0] not in Labels:
				sys.exit(52)
			StackFun.append(int(child.attrib["order"]))
			interpret(xmlroot[int(Labels[child[0].text]):])
			break

		elif(child.attrib["opcode"] in ['RETURN']):
			InstuctionArgumentCounter(child, 0)
			if StackFun == []:
				sys.exit(56)
			interpret(xmlroot[StackFun.pop():])
			break
		elif(child.attrib["opcode"] in ['PUSHS']):
			InstuctionArgumentCounter(child, 1)
			typ = getArgumentType(child[0].attrib["type"])
			values.append(getValue(child[0], typ, Labels))
			if typ == 'var':
				getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
				values[0] = getVariable(values[0][3:], globalFrame, localFrame, temporaryFrame)
			elif typ in ['label', 'type']:
				sys.exit(53)
			Stack.append(values[0])

		elif(child.attrib["opcode"] in ['POPS']):
			InstuctionArgumentCounter(child, 1)
			if Stack == []:
				sys.exit(56)
			typ = getArgumentType(child[0].attrib["type"])
			if typ == 'var':
				values.append(getValue(child[0], typ, Labels))
				getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
			else:
				sys.exit(53)
			setValue(values[0][:2], values[0][3:], Stack.pop(), globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['ADD']):
			values = operatorClassic(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			result = values[1] + values[2]
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['MUL']):
			values = operatorClassic(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			result = values[1] * values[2]
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['IDIV']):
			values = operatorClassic(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			if(int(values[2]) == 0):
				sys.exit(57)
			result = int(values[1]) // int(values[2])
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['LT']):
			values = operatorRelational(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			result = values[1] < values[2]
			result = str(result).lower()
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['GT']):
			values = operatorRelational(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			result = values[1] > values[2]
			result = str(result).lower()
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['EQ']):
			values = operatorRelational(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			result = values[1] == values[2]
			result = str(result).lower()
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['AND']):
			values = operatorBoolean(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			if((values[1]) == "true" and (values[2]) == "true"):
				result = 'true'
			else:
				result = 'false'
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['OR']):
			values = operatorBoolean(child, values, globalFrame, localFrame, temporaryFrame, Labels)
			if((values[1]) == "true" or (values[2]) == "true"):
				result = 'true'
			else:
				result= 'false'
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['NOT']):
			InstuctionArgumentCounter(child, 2)
			values.append(getValue(child[0], "var", Labels))
			getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
			values.append(getValue(child[1], "bool", Labels))
			if((values[1]) == "true"):
				result = 'false'
			else:
				result = 'true'
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['INT2CHAR']):
			InstuctionArgumentCounter(child, 2)
			values.append(getValue(child[0], "var", Labels))
			getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
			typ1 = getArgumentType(child[1].attrib["type"])
			if typ1 == 'int' or typ1 == 'var':
				values.append(getValue(child[1], typ1, Labels))
				if typ1 == 'var':
					getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
					values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
					checkInteger(values[1])
			else:
				sys.exit(53)
			try:
				result = chr(values[1])
			except:
				sys.exit(58)
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['STRI2INT']):
			InstuctionArgumentCounter(child, 3)
			values.append(getValue(child[0], "var", Labels))
			getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
			typ1 = getArgumentType(child[1].attrib["type"])
			if typ1 == 'string' or typ1 == 'var':
				values.append(getValue(child[1], typ1, Labels))
				if typ1 == 'var':
					getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
					values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
					values[1] = checkString(values[1])
			else:
				sys.exit(53)
			typ2 = getArgumentType(child[2].attrib["type"])
			if typ2 == 'int' or typ2 == 'var':
				values.append(getValue(child[2], typ2, Labels))
				if typ2 == 'var':
					getVariableFrame(values[2][:2], values[2][3:], globalFrame, localFrame, temporaryFrame)
					values[2] = getVariable(values[2][3:], globalFrame, localFrame, temporaryFrame)
					checkInteger(values[2])
			else:
				sys.exit(53)
			if(len(values[1]) <= values[2]):
				sys.exit(58)
			result = ord(values[1][values[2]])
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['WRITE']):
			InstuctionArgumentCounter(child, 1)
			typ = getArgumentType(child[0].attrib["type"])
			values.append(getValue(child[0], typ, Labels))
			if typ == "var":
				getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
				values[0] = getVariable(values[0][3:], globalFrame, localFrame, temporaryFrame)
				typ = variableTypeChecker(values[0]);
			elif typ == 'int':
				checkInteger(values[0])
			elif typ == 'string':
				values[0] = checkString(values[0])
			elif typ == 'bool':
				values[0] = str(values[0]).lower()
				checkBoolean(values[0])
			else:
				values.append(getValue(child[0], typ, Labels))
			values[0] = str(values[0])
			if values[0] == None:
				sys.exit(56)
			print(values[0])

		elif(child.attrib["opcode"] in ['CONCAT']):
			InstuctionArgumentCounter(child, 3)
			values.append(getValue(child[0], "var", Labels))
			getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
			typ1 = getArgumentType(child[1].attrib["type"])
			if typ1 == 'string' or typ1 == 'var':
				values.append(getValue(child[1], typ1, Labels))
				if typ1 == 'var':
					getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
					values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
					values[1] = checkString(values[1])
			else:
				sys.exit(53)
			typ2 = getArgumentType(child[2].attrib["type"])
			if typ2 == 'string' or typ2 == 'var':
				values.append(getValue(child[2], typ2, Labels))
				if typ2 == 'var':
					getVariableFrame(values[2][:2], values[2][3:], globalFrame, localFrame, temporaryFrame)
					values[2] = getVariable(values[2][3:], globalFrame, localFrame, temporaryFrame)
					values[2] = checkString(values[2])
			else:
				sys.exit(53)
			result = values[1] + values[2]
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['STRLEN']):
			InstuctionArgumentCounter(child, 2)
			values.append(getValue(child[0], "var", Labels))
			getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
			typ = getArgumentType(child[1].attrib["type"])
			if typ == "string":
				values.append(getValue(child[1], "string", Labels))
			elif typ == "var":
				values.append(getValue(child[1], "var", Labels))
				getVariableFrame(values[1][:2], values[1][3:], globalFrame, localFrame, temporaryFrame)
				values[1] = getVariable(values[1][3:], globalFrame, localFrame, temporaryFrame)
				typ = variableTypeChecker(values[1]);
			if typ == 'string':
				values[1] = checkString(values[1])
			else:
				sys.exit(53)
			result = int(len(values[1]))
			setValue(values[0][:2], values[0][3:], result, globalFrame, localFrame, temporaryFrame)

		elif(child.attrib["opcode"] in ['LABEL']):
			continue
		elif(child.attrib["opcode"] in ['DPRINT']):
			InstuctionArgumentCounter(child, 1)
			typ = getArgumentType(child[0].attrib["type"])
			if typ == "var":
				values.append(getValue(child[0], "var", Labels))
				getVariableFrame(values[0][:2], values[0][3:], globalFrame, localFrame, temporaryFrame)
				values[0] = getVariable(values[0][3:], globalFrame, localFrame, temporaryFrame)
			else:
				values.append(getValue(child[0], typ, Labels))
			values[0] = str(values[0]) + '\n'
			sys.stderr.write(values[0])

		else:
			sys.exit(32)

if __name__ == "__main__":		#funkce main ktera spusti vsechni funkce
	localFrame = None
	temporaryFrame = None
	globalFrame = {}
	Labels = {}
	Stack = []
	StackFrame = []
	StackFun = []
	order = 1
	OutputFile = CheckArguments(sys.argv[1:])
	file = open(OutputFile, 'r')
	xmlroot = loadInput(OutputFile)
	XMLChecker(xmlroot)
	interpret(xmlroot)
