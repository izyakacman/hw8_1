#include "thread.h"
#include "writer.h"

using namespace std;

void CoutWriterThr(queue<string>& stringQueue, mutex& threadMutex, bool& bStopFlag)
{
	CoutWriter writer;

	while (bStopFlag)
	{
		lock_guard<std::mutex> guard(threadMutex);

		if (!stringQueue.empty())
		{
			writer.Print(move(stringQueue.front()));
			stringQueue.pop();
		}

	} // while (bStopFlag)
}

void FileWriterThr(std::queue<std::pair<std::string, long long>>& stringQueue, std::mutex& threadMutex, bool& bStopFlag, int postfix)
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
}
