//
// C++ Interface: srcFileParser
//
//  \copyright 2013 DCBlaha.  Distributed under the GPL.
//
#include "DirList.h"
#include "ComponentFinder.h"
#include <map>
#include <vector>
#include "Debug.h"



class Logger:public DebugFile
    {
    public:
	void open(char const * const outPath);
	void logOutputOld(char const * const fn);
	void logProcess(char const * const srcFile, char const * const *argv, int argc);
	void logProcessStatus(bool success);
    };
extern Logger sLog;


struct fileInfo
{
    fileInfo(const time_t srcFileDate, bool present):
        mSourceFileDate(srcFileDate), mPresent(present)
        {}
    time_t mSourceFileDate;
    bool mPresent;
};

/// Recursively finds source files, and parses the source file
/// for static information, and saves into analysis files.
class srcFileParser:public dirRecurser
{
public:
    srcFileParser(const ComponentFinder &compFinder):
	mSrcRootDir(nullptr), mAnalysisDir(nullptr), mComponentFinder(compFinder)
	{}
    virtual ~srcFileParser()
	{}
    bool analyzeSrcFiles(char const * const srcRootDir, char const * const analysisDir,
	    char const * const incDepsFilename);

private:
    char const * mSrcRootDir;
    char const * mAnalysisDir;
    std::vector<std::string> mIncDirArgs;
    const ComponentFinder &mComponentFinder;

    virtual bool processFile(const std::string &filePath);
};
