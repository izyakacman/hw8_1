﻿#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

#include "writer.h"

class CommandsProcessor;
constexpr auto EndOfFileString = "eof";

class ICommandHandler;
using ICommandHandlerPtr = std::unique_ptr<ICommandHandler>;
using IWriterPtr = std::unique_ptr<IWriter>;

/**
*	Command handler interface
*/
class ICommandHandler {
public:

	ICommandHandler() = delete;
	explicit ICommandHandler(size_t count) : count_{ count } {}
	virtual ~ICommandHandler() = default;

    virtual ICommandHandlerPtr ProcessCommand(CommandsProcessor*, const std::string&, bool& exit) = 0;

protected:

	const size_t count_;
};

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

	bool ProcessCommand(const std::string& cmd) 
	{
		bool res;

		ICommandHandlerPtr ptr = handler_->ProcessCommand(this, cmd, res);

		if (ptr)
		{
			handler_ = std::move(ptr);
		}

		return !res;
	}

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
	std::atomic<bool> stop_flag_ = true;
};

/**
*	Process command int the static mode
*/
class StaticCommandHandler : public ICommandHandler
{
public:

	StaticCommandHandler() = delete;
	StaticCommandHandler(size_t count) : ICommandHandler(count) {}
	~StaticCommandHandler() = default;

	ICommandHandlerPtr ProcessCommand(CommandsProcessor* cmd, const std::string& s, bool& exit) override;
};

/**
*	Process command int the dynamic mode
*/
class DynamicCommandHandler : public ICommandHandler
{
public:

	DynamicCommandHandler() = delete;
	DynamicCommandHandler(size_t count) : ICommandHandler(count) {}
	~DynamicCommandHandler() = default;

	ICommandHandlerPtr ProcessCommand(CommandsProcessor* cmd, const std::string& s, bool& exit) override;

private:

	size_t openBraceCount_ = 0;
};
