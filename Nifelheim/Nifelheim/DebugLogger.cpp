#include "DebugLogger.h"
#include <fstream>

DebugLogger* DebugLogger::_instance = nullptr;

DebugLogger::DebugLogger()
{

}

DebugLogger::~DebugLogger()
{


}

void DebugLogger::Dump()
{
	if (DebugLogger::GetInstance()->_errorLog.size() > 0)
	{
		std::ofstream fout("Errorlog.txt");
		for (auto &msg : DebugLogger::GetInstance()->_errorLog)
		{
			fout << msg << "\n";
		}
		fout.close();
	}
}

DebugLogger * DebugLogger::GetInstance()
{
	if (!_instance)
		_instance = new DebugLogger();
	return _instance;
}

void DebugLogger::AddMsg(std::string msg)
{
	DebugLogger::GetInstance()->_errorLog.push_back(msg);
}
