#include <iostream>
#include <string>

#include "commands_processor.h"

using namespace std;

int main(int argn, char** argc)
{
	if (argn != 2)
	{
		cout << "Usage: bulk <commands count>";
			return -1;
	}

	string s;
	CommandsProcessor cmd{ static_cast<size_t>(atoll(argc[1])) };

	while (cin >> s)
	{
		if (cmd.ProcessCommand(s) == false) break;
	}

	if (!cin)
	{
		cmd.ProcessCommand(EndOfFileString);
	}

	return 0;
}
