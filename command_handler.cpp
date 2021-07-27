#include "command_handler.h"
#include "commands_processor.h"

/**
*	Process command int the static mode
*/
ICommandHandlerPtr StaticCommandHandler::ProcessCommand(CommandsProcessor* cmd, const std::string& s, bool& exit)
{
	exit = true;

	if (s != EndOfFileString)
	{
		exit = false;

		if (s == "{")
		{
			cmd->PrintPool();
			return ICommandHandlerPtr{ new DynamicCommandHandler(count_) };
		}

		cmd->PushPool(s);

		if (cmd->GetPoolSize() == count_)
		{
			cmd->PrintPool();
		}
	}
	else
	{
		cmd->PrintPool();
	}

	return nullptr;
}

/**
*	Process command int the dynamic mode
*/
ICommandHandlerPtr DynamicCommandHandler::ProcessCommand(CommandsProcessor* cmd, const std::string& s, bool& exit)
{
	exit = true;

	if (s == EndOfFileString)
		return nullptr;

	exit = false;

	if (s == "{")
	{
		++openBraceCount_;
	}
	else
		if (s == "}")
		{
			if (openBraceCount_ == 0)
			{
				cmd->PrintPool();
				return ICommandHandlerPtr{ new StaticCommandHandler(count_) };
			}

			--openBraceCount_;
		}
		else
		{
			cmd->PushPool(s);
		}

	return nullptr;
}
