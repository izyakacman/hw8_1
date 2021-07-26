#include "thread.h"
#include "writer.h"

using namespace std;

void CoutWriterThr(queue<string>& stringQueue, mutex& threadMutex, atomic<bool>& bStopFlag, std::condition_variable& cv)
{
	CoutWriter writer;

	while (true)
	{
		std::unique_lock<mutex> lock( threadMutex );

		cv.wait( lock, [&stringQueue, &bStopFlag] { return !stringQueue.empty() || !bStopFlag; });

		if (!stringQueue.empty())
		{
			writer.Print(move(stringQueue.front()));
			stringQueue.pop();
		}
	}

	cout << "CoutWriterThr stop\n";


}

void FileWriterThr(std::queue<std::pair<std::string, long long>>& stringQueue, std::mutex& threadMutex, atomic<bool>& bStopFlag,
	int postfix, std::condition_variable& cv)
{
	FileWriter writer;

	while (true)
	{
		std::unique_lock<mutex> lock( threadMutex );

		cv.wait( lock, [&stringQueue, &bStopFlag] { return !stringQueue.empty() || !bStopFlag; });

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
