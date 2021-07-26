#include "command.h"
#include "thread.h"

#include <chrono>

using namespace std;

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

	if(s == EndOfFileString)
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

CommandsProcessor::CommandsProcessor(size_t count) : count_{ count }
{
	handler_ = ICommandHandlerPtr{ new StaticCommandHandler(count_) };

	cout_thr = thread(CoutWriterThr, ref(stringsQueueCout), ref(ThreadCoutMutex), ref(stop_flag_), ref(cv_));
	file_thr1 = thread(FileWriterThr, ref(stringsQueueFile), ref(ThreadFileMutex), ref(stop_flag_), 1, ref(cv_));
	file_thr2 = thread(FileWriterThr, ref(stringsQueueFile), ref(ThreadFileMutex), ref(stop_flag_), 2, ref(cv_));
}

CommandsProcessor::~CommandsProcessor()
{
	stop_flag_ = false;
	cv_.notify_all();
	cout_thr.join();
	file_thr1.join();
	file_thr2.join();
}

/**
*	Add command int the block
*/
void CommandsProcessor::PushPool(const std::string& s)
{
	if (pool_.size() == 0)
	{
		firstCmdTime_ = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	pool_.push_back(s);
}

/**
*	Output string
*/
void CommandsProcessor::PrintString(const std::string& s)
{
	{
		lock_guard<std::mutex> guard(ThreadCoutMutex);
		stringsQueueCout.push(s);
	}
	{
		lock_guard<std::mutex> guard(ThreadFileMutex);
		stringsQueueFile.push(pair(s, firstCmdTime_));
	}
}

/**
*	Output block of commands
*/
void CommandsProcessor::PrintPool()
{
	CoutWriter coutWriter;

	if (pool_.size())
	{
		string s{"bulk: "};

		for(size_t i=0; i < pool_.size(); ++i)
		{
			s += pool_[i];

			if (i != pool_.size() - 1)
			{
				s += ", ";
			}
		}
		s += "\n";
		PrintString(s);
		pool_.clear();
	}
}
