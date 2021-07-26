#include "thread.h"
#include "writer.h"

using namespace std;

void CoutWriterThr(queue<string>& stringQueue, mutex& threadMutex, atomic<bool>& bStopFlag, std::condition_variable& cv)
{
	CoutWriter writer;

	while (bStopFlag)
	{
		std::unique_lock<mutex> lock( threadMutex );

		cv.wait( lock, [&stringQueue] { return !stringQueue.empty(); });

		writer.Print(move(stringQueue.front()));
		stringQueue.pop();
	}

	cout << "CoutWriterThr stop\n";

/////////
/*
	while (bStopFlag)
	{
		lock_guard<std::mutex> guard(threadMutex);

		if (!stringQueue.empty())
		{
			writer.Print(move(stringQueue.front()));
			stringQueue.pop();
		}

	} // while (bStopFlag)
*/	
}

void FileWriterThr(std::queue<std::pair<std::string, long long>>& stringQueue, std::mutex& threadMutex, atomic<bool>& bStopFlag,
	int postfix, std::condition_variable& /*cv*/)
{
	FileWriter writer;

	while (bStopFlag)
	{
		lock_guard<std::mutex> guard(threadMutex);

		if (!stringQueue.empty())
		{
			auto p = move(stringQueue.front());
			writer.SetTime(p.second, postfix);
			writer.Print(p.first);
			stringQueue.pop();
		}

	} // while (bStopFlag)

	cout << "FileWriterThr stop\n";
}
