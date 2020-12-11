#include <ddcDef.h>
#include <ddcString.h>

#include <stdio.h>

#include "./utils.h"
#include "./tokens.h"
#include "./parser.h"
#include "./generate.h"
#include "./file.h"

#include <stdio.h>
#include <stdlib.h>

sizet tokens_get_command_count(struct token* tokens, sizet tokenCount)
{
	sizet semiColons = 0;
	for (sizet i = 0; i < tokenCount; i++)
		if (tokens[i].value.cstr[0] == ';') semiColons++;
	return semiColons;
}
int tokens_get_next_command(struct token* tokens, int start, sizet tokenCount)
{
	for (sizet i = start; i < tokenCount; i++)
		if (tokens[i].value.cstr[0] == ';') return i;
	return 0;
}

int main(int agsc, char** ags)
{
	ddPrint_cstring("\x1b[38;2;255;255;255m");
	if (agsc < 2) compile_error("NO INPUT FILES");

	bool debug = false;
	if (agsc > 3 && ddString_compare_cstring(make_constant_ddString("-debug"), ags[3])) debug = true;

	sizet tokenCount = 0;
	ddString file = read_file(ags[1]);
	struct token* tokens = tokenize_file(file, &tokenCount);
	if (debug) ddPrintf("TokenCount: %d\n", tokenCount);
	raze_ddString(&file);
	for (sizet i = 0; i < tokenCount; i++)
	{
		tokens[i].value.cstr[tokens[i].value.length] = '\0';
		if (debug) ddPrintf("%d: %s: %s\n", i, TKN_STRS[tokens[i].type], tokens[i].value.cstr);
	}
	ddString fileOut = make_ddString("");
	ddString_push_cstring_back(&fileOut, "global _start\n_start:\n	push rbp;\n	mov rbp, rsp;\n");
	init_generation();
	ddString asmCode[40];
	for (int i = 0; i < 40; i++)
	{
		asmCode[i] = make_ddString("");
	}
	sizet commandPos = 0;
	sizet nextCommandPos = tokens_get_next_command(tokens, commandPos, tokenCount);
	while (commandPos < tokenCount)
	{
		struct tokenNode* commandHead = make(struct tokenNode, 1);
		(*commandHead) = make_tokenNode(nullptr, nullptr, nullptr, nullptr);
		parser(tokens, commandHead, commandPos, nextCommandPos, tokenCount);
		sizet lineCount = 0;
		generate_asm(commandHead, asmCode, &lineCount);
		for (sizet i = lineCount-1; i >= 0; i--)
		{
			ddString_push_back(&fileOut, asmCode[i]);
		}
		commandPos = nextCommandPos+1;
		nextCommandPos = tokens_get_next_command(tokens, commandPos, tokenCount);
	}
	ddString_push_cstring_back(&fileOut, "	mov	eax, 0;\n	pop	rbp;\n	mov	rax, 60;\n	mov	rdi, 0;\n	syscall;\n");
	ddPrintf("DONE COMPILE\n");
	if (agsc < 3)
		ddPrint_ddString(fileOut);
	else if (!debug)
		write_file(ags[2], fileOut);
	else ddPrint_ddString(fileOut);
	if (debug) ddPrint_nl();
	raze(tokens);
	return 0;
}
