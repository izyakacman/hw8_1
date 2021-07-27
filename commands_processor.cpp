#include "commands_processor.h"
#include "writer.h"

#include <chrono>

using namespace std;

namespace {

void CoutWriterThr(queue<string>& stringQueue, mutex& threadMutex, std::atomic_bool& bStopFlag, std::condition_variable& cv)
{
	CoutWriter writer;

	while (bStopFlag)
	{
		std::unique_lock<mutex> lock(threadMutex);

		cv.wait(lock, [&stringQueue, &bStopFlag] { return !stringQueue.empty() || !bStopFlag; });

		if (!stringQueue.empty())
		{
			writer.Print(move(stringQueue.front()));
			stringQueue.pop();
		}
	}
}

void FileWriterThr(std::queue<std::pair<std::string, long long>>& stringQueue, std::mutex& threadMutex, std::atomic_bool& bStopFlag,
	int postfix, std::condition_variable& cv)
{
	FileWriter writer;

	while (bStopFlag)
	{
		std::unique_lock<mutex> lock(threadMutex);

		cv.wait(lock, [&stringQueue, &bStopFlag] { return !stringQueue.empty() || !bStopFlag; });

		if (!stringQueue.empty())
		{
			auto p = move(stringQueue.front());
			writer.SetTime(p.second, postfix);
			writer.Print(p.first);
			stringQueue.pop();
		}
	}
}
} // namespace


CommandsProcessor::CommandsProcessor(size_t count) : count_{ count }
{
	handler_ = ICommandHandlerPtr{ new StaticCommandHandler(count_) };

	cout_thr = thread(CoutWriterThr, ref(stringsQueueCout), ref(ThreadCoutMutex), ref(stop_flag_), ref(cv_));
	file_thr1 = thread(FileWriterThr, ref(stringsQueueFile), ref(ThreadFileMutex), ref(stop_flag_), 1, ref(cv_));
	file_thr2 = thread(FileWriterThr, ref(stringsQueueFile), ref(ThreadFileMutex), ref(stop_flag_), 2, ref(cv_));
}

CommandsProcessor::~CommandsProcessor()
{
	{
		std::lock_guard<std::mutex> lock_cout(ThreadCoutMutex);
		std::lock_guard<std::mutex> lock_file(ThreadFileMutex);
		stop_flag_ = false;
	}
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

	cv_.notify_all();
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

bool CommandsProcessor::ProcessCommand(const std::string& cmd)
{
	bool res;

	ICommandHandlerPtr ptr = handler_->ProcessCommand(this, cmd, res);

	if (ptr)
	{
		handler_ = std::move(ptr);
	}

	return !res;
}

