#pragma once

#include <string>
#include <mutex>
#include <queue>

/**
*	Console writer thread
*/
void CoutWriterThr(std::queue<std::string>& stringQueue, std::mutex& threadMutex, std::atomic<bool>& bStopFlag);

/**
*	File writer thread
*/
void FileWriterThr(std::queue<std::pair<std::string, long long>>& stringQueue, std::mutex& threadMutex, std::atomic<bool>& bStopFlag, int postfix);