//
// C++ Interface: DirList
//
//  \copyright 2013 DCBlaha.  Distributed under the GPL.

#ifndef DirList_h
#define DirList_h

#include "FilePath.h"   // For FilePaths
#include <string>
#include <vector>

/// Delete the last leaf of a directory.
/// @param path The path to delete.
OovStatusReturn deleteDir(OovStringRef const path);

/// Recursively delete directories.  This deletes all files in the directories.
/// @param path The path to delete.
OovStatusReturn recursiveDeleteDir(OovStringRef const path);

/// Get all filenames with a matching extensions.
/// @param path The path to search.
/// @param extensions The extensions to match.
/// @param fn The returned list of filenames.
OovStatusReturn getDirListMatchExt(OovStringRef const path, const FilePaths &extensions,
        std::vector<std::string> &fn);

/// Get all filenames with a matching extension.
/// @param path The path to search.
/// @param ext The extension to match.
/// @param fn The returned list of filenames.
OovStatusReturn getDirListMatchExt(OovStringRef const path, const FilePath &ext,
        std::vector<std::string> &fn);

/// Get all filenames with a matching extensions.
/// @param paths The paths to search.
/// @param extensions The extensions to match.
/// @param fn The returned list of filenames.
OovStatusReturn getDirListMatchExt(const std::vector<std::string> &paths,
        const FilePaths &extensions, std::vector<std::string> &fn);

/// This can only accept a wild card "*" in the last subdir.
OovStatusReturn const getDirListMatch(OovStringRef const path,
    std::vector<std::string> &fn);

/// Find directories that match the wildcard string.
/// The wildcardStr can have an asterisk, but must be at the end of
/// a subdirectory. For example: "\Qt*\mingw*\"
/// @param startingDirs A list of directories to find the closest match.
/// @param wildCardStr The wildcard string to use to match.
FilePaths findMatchingDirs(FilePaths const &startingDirs,
        OovStringRef const wildCardStr);

enum eDirListTypes { DL_Files=0x01, DL_Dirs=0x02, DL_Both=DL_Files|DL_Dirs };
/// Get a list of directories, files or both.
/// @param path The search path.
/// @param dt The type of directory entry to list.
/// @param fn A vector of full path names. The names are appended to the vector.
OovStatusReturn getDirList(OovStringRef const path, eDirListTypes dt,
    std::vector<std::string> &fn);

/// Recursivley walks a directory, and calls the processFile
/// function as each file is found.
class dirRecurser
{
public:
    virtual ~dirRecurser();
    /// Do a recursive search starting from the path.
    /// @param path The search path.
    OovStatusReturn recurseDirs(OovStringRef const path);
    /// Override to get called for each file.
    /// Return true while success.
    virtual bool processFile(OovStringRef const filePath) = 0;
};

#endif
