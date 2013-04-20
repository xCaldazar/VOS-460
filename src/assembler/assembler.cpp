// TODO
// - Needs to be fixed for jumps.

#include "assembler.h"
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <stdio.h>
using namespace std;

/**
 * Construct object and load opcodes file into a map of opCodes and
 * a set of immediates.
 *
 * @param opListPath The path to the opcode listing to be used for assembly.
 */
Assembler::Assembler(const string &opListPath)
{
	char immediate;
	string opCode;
	ifstream opListFile(opListPath.c_str());
	istream_iterator<string> it (opListFile), end;

	// Load opcodes file into memory
	for (int i = 0; it != end; ++i, ++it) {
		immediate = it->at(it->size() - 1);
		opCode = it->substr(0, it->size() - 2);

		// Build set of opCodes capable of immediate addressing
		if(immediate == '1') {
			immediates.insert(opCode);
		}

		// Build map of generic opCodes
		opCodes.insert(pair<string, int>(opCode, i));
	}

	// Verify data existence
	if  (this->opCodes.empty()) {
		throw "Could not read data in from operation codes file";
	}
}

/**
 * Load source file line by line and assemble opcode field of object code.
 *
 * @param sourcePath The path to the program that will be assembled
 * @return void
 */
void Assembler::build(const string &sourcePath)
{
	int object = 0, lineNumber = 1;
	instruction op = {"", 0};
	ifstream sourceFile(sourcePath.c_str());
	ofstream programFile("test.o");

	// Parse source file and build assembly file
	for (string line; getline(sourceFile, line); ++lineNumber) {
		op = parse(line);

		// Check for no instruction
		if (op.command.empty()) {
			continue;
		}

		// Check for invalid instructions
		if (opCodes.find(op.command) == opCodes.end()) {
			string message = "Illegal instruction";
			throw parseError(lineNumber, message, line).c_str();
		}

		// Check for invalid immediate instructions
		if (op.immediate and immediates.find(op.command) == immediates.end()) {
			string message = "Illegal immediate instruction";
			throw parseError(lineNumber, message, line).c_str();
		}

		object = ((opCodes[op.command] << 2 | op.rd) << 1 | op.immediate) << 8 | op.value;
		programFile << object << endl;

		printf("[Operation]: %s, [Immediate]: %u, [Parameter]: %u, [Parameter]: %u\n", op.command.c_str(), op.immediate, op.rd, op.value);
	}
}

/**
 * Splits an assembly instruction into its command and parameters
 *
 * @param line The source code instruction to be interpereted.
 * @return Assembler::instruction
 */
Assembler::instruction Assembler::parse(const string &line)
{
	instruction op = {"", 0};
	int start = 0, end = 0;

	// Get the size of the instruction without comments
	int size = min(line.find_first_of("!"), line.size() - 1);

	// Extract the command
	if (size > 0) {
		start = line.find_first_not_of("\t ");
		end = line.find_first_of("\t \r\n", start);
		op.command = line.substr(start, end - start);
	}

	// Set immediate bit and remove from command
	if (op.command[op.command.size() - 1] == 'i') {
		op.command.erase(op.command.size() - 1);
		op.immediate = 1;
	}

	// Extract the first argument, if it exists
	if (size > end) {
		start = line.find_first_not_of("\t ", end);
		end = line.find_first_of("\t \r\n", start);
		istringstream(line.substr(start, end - start)) >> op.rd;
		op.value &= INT_MASK;
	}

	// Extract the second argument, if it exists
	if (size > end) {
		start = line.find_first_not_of("\t ", end);
		end = line.find_first_of("\t \r\n", start);
		istringstream(line.substr(start, end - start)) >> op.value;
		op.value &= INT_MASK;
	}

	return op;
}

/**
 * Formats parsing errors in a consistant and human readable way.
 * Ex: On line 5, Invalid Instruction: "add 2 172"
 *
 * @param lineNumber The line of which the error occurred
 * @param msg The error message to display
 * @param line The contents of the line in question
 * @return string
 */
string Assembler::parseError(int lineNumber, string msg, string line)
{
	stringstream ss;
	ss << lineNumber;
	string error = "On line: " + ss.str() + ", " + msg + ": \"" + line + "\"";
	return error;
}
