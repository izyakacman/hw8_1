#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>

#include "writer.h"
#include "command_handler.h"

using IWriterPtr = std::unique_ptr<IWriter>;

/**
*	Process commands
*/
class CommandsProcessor
{
public:

	explicit CommandsProcessor(size_t count);
	~CommandsProcessor();

	void PushPool(const std::string& s);

	size_t GetPoolSize()
	{
		return pool_.size();
	}

	void PrintPool();

	void PrintString(const std::string& s);

	bool ProcessCommand(const std::string& cmd);

private:

	const size_t count_;
	long long firstCmdTime_ = 0;
	ICommandHandlerPtr handler_;
	std::vector<std::string> pool_;
	std::vector<IWriterPtr> writersPool_;

	std::thread cout_thr;
	std::thread file_thr1;
	std::thread file_thr2;
	std::mutex ThreadCoutMutex;
	std::mutex ThreadFileMutex;
	std::queue<std::string> stringsQueueCout;
	std::queue<std::pair<std::string, long long>> stringsQueueFile;
	std::atomic_bool stop_flag_ = true;
	std::condition_variable cv_;
};


