/*
 * Copyright © 2017 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of λcommon.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../../include/lambdacommon/system/filesystem/filesystem.h"
#include "../../../include/lambdacommon/string.h"
#include <stdexcept>
#include <sstream>

#ifdef LAMBDA_WINDOWS

#include <windows.h>

#define STAT_STRUCT _stati64
#define STAT_METHOD _stati64

#else

#include <unistd.h>
#include <cstring>
#include <limits.h>

#define STAT_STRUCT stat
#define STAT_METHOD stat

#endif

#include <sys/stat.h>

using namespace std;

namespace lambdacommon
{
    namespace filesystem
    {
        FilePath::FilePath() : Path(), _type(NATIVE), _absolute(false)
        {

        }

        FilePath::FilePath(const char *path) : Path()
        {
            set(string(path));
        }

        FilePath::FilePath(string path) : Path()
        {
            set(path);
        }

#ifdef LAMBDA_WINDOWS

        FilePath::FilePath(const wchar_t *path) : Path()
        {
            set(lambdastring::convertWStringToString(wstring(path)));
        }

        FilePath::FilePath(wstring path) : Path()
        {
            set(lambdastring::convertWStringToString(path));
        }

#endif

        FilePath::FilePath(const FilePath &path) : _type(path._type), Path(*path._path),
                                                   _absolute(path._absolute)
        {}

        FilePath::FilePath(FilePath &&path) : _type(path._type), Path(vector<string>(move(*path._path))),
                                              _absolute(path._absolute)
        {}

        void FilePath::set(const string &str, PathType type)
        {
            _type = type;
            if (type == WINDOWS)
            {
                *_path = serializable::tokenize(str, "/\\");
                _absolute = str.size() > 2 && isalpha(str[0]) && str[1] == ':';
            }
            else
            {
                *_path = serializable::tokenize(str, "/");
                _absolute = !str.empty() && str[0] == '/';
            }
        }

        bool FilePath::remove()
        {
#ifdef LAMBDA_WINDOWS
            return DeleteFileA(toString().c_str()) != 0;
#else
            return std::remove(toString().c_str()) == 0;
#endif
        }

        bool FilePath::mkdir(bool recursive)
        {
            if (recursive)
            {
                auto parent = getParent();
                if (!parent.exists())
                    parent.mkdir(true);
            }
#ifdef LAMBDA_WINDOWS
            return CreateDirectoryA(toString().c_str(), NULL) != 0;
#else
            return ::mkdir(toString().c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
#endif
        }

        bool FilePath::empty() const
        {
            return _path->empty();
        }

        bool FilePath::isAbsolute() const
        {
            return _absolute;
        }

        bool FilePath::exists() const
        {
#ifdef LAMBDA_WINDOWS
            return GetFileAttributesA(toString().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
            struct STAT_STRUCT sb;
            return stat(toString().c_str(), &sb) == 0;
#endif
        }

        bool FilePath::isDirectory() const
        {
#ifdef LAMBDA_WINDOWS
            auto attr = GetFileAttributesA(toString().c_str());
            return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else
            struct STAT_STRUCT sb;
            if (stat(toString().c_str(), &sb))
                return false;
            return S_ISDIR(sb.st_mode);
#endif
        }

        bool FilePath::isFile() const
        {
#ifdef LAMBDA_WINDOWS
            auto attr = GetFileAttributesA(toString().c_str());
            return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
            struct STAT_STRUCT sb;
            if (stat(toString().c_str(), &sb))
                return false;
            return S_ISREG(sb.st_mode);
#endif
        }

        string FilePath::getFileName() const
        {
            if (empty())
                return "";
            const string &last = (*_path)[_path->size() - 1];
            return last;
        }

        string FilePath::getExtension() const
        {
            const string &name = getFileName();
            size_t pos = name.find_last_of(".");
            if (pos == string::npos)
                return "";
            return name.substr(pos + 1);
        }

        size_t FilePath::getFileSize() const
        {
            if (!exists())
                return 0;
            struct STAT_STRUCT sb;
            if (STAT_METHOD(toString().c_str(), &sb) != 0)
                throw runtime_error(
                        "lambdacommon::path::FilePath.getFileSize(): cannot stat file \"" + toString() + "\"!");
            return (size_t)
            sb.st_size;
        }

        FilePath FilePath::toAbsolute() const
        {
#ifdef LAMBDA_WINDOWS
            if (toString().empty())
                return getCurrentWorkingDirectory();
            char temp[MAX_PATH];
            auto length = GetFullPathNameA(toString().c_str(), MAX_PATH, temp, NULL);
            if (length == 0)
                throw runtime_error(
                        "Internal error in lambdacommon::path::FilePath.toAbsolute(): " + to_string(GetLastError()));
            return FilePath(temp);
#else
            char temp[PATH_MAX];
            if (realpath(toString().c_str(), temp) == NULL)
                throw runtime_error(
                        "Internal error in lambdacommon::path::FilePath::toAbsolute(): " + string(strerror(errno)));
            return FilePath(temp);
#endif
        }

        FilePath FilePath::getParent() const
        {
            FilePath result;
            result._type = _type;
            result._absolute = _absolute;

            if (empty())
            {
                if (!_absolute)
                    result._path->push_back("..");
            }
            else
                for (size_t i = 0; i < _path->size() - 1; ++i)
                    result._path->push_back((*_path)[i]);
            return result;
        }

        string FilePath::toString(PathType type) const
        {
            ostringstream oss;

            if (type == COMMON && _absolute)
                oss << "/";

            for (size_t i = 0; i < _path->size(); i++)
            {
                oss << (*_path)[i];
                if (i + 1 < _path->size())
                {
                    if (type == COMMON)
                        oss << '/';
                    else
                        oss << '\\';
                }
            }

            return oss.str();
        }

        /*
         * OPERATORS
         */

        FilePath FilePath::operator/(const FilePath &other)
        {
            if (other._absolute)
                throw runtime_error("lambdacommon::path::FilePath::operator/(): Expected a relative path!");

            FilePath result(*this);

            for (auto part : *other._path)
                (*result._path).push_back(part);

            return result;
        }

        FilePath &FilePath::operator=(const FilePath &path)
        {
            _type = path._type;
            delete _path;
            _path = new vector<string>(*path._path);
            _absolute = path._absolute;
            return *this;
        }

        FilePath &FilePath::operator=(FilePath &&path)
        {
            if (this != &path)
            {
                _type = path._type;
                delete _path;
                _path = new vector<string>(move(*path._path));
                _absolute = path._absolute;
            }
            return *this;
        }

        bool FilePath::operator==(const FilePath &_p)
        {
            return _p._path == _path;
        }

        bool FilePath::operator!=(const FilePath &_p)
        {
            return _p._path != _path;
        }

#ifdef LAMBDA_WINDOWS

        wstring getCurrentWorkingDirectoryWStr()
        {
            wchar_t temp[MAX_PATH];
            if (!_wgetcwd(temp, MAX_PATH))
                throw runtime_error(
                        "Internal error in lambdacommon::path::getCurrentWorkingDirectoryWStr(): " +
                        to_string(GetLastError()));
            return wstring(temp);
        }

#endif

        string getCurrentWorkingDirectoryStr()
        {
#ifdef LAMBDA_WINDOWS
            return lambdastring::convertWStringToString(getCurrentWorkingDirectoryWStr());
#else
            char temp[PATH_MAX];
            if (getcwd(temp, PATH_MAX) == NULL)
                throw runtime_error("Internal error in lambdacommon::path::getCurrentWorkingDirectoryStr(): " +
                                    string(strerror(errno)));
            return string(temp);
#endif
        }

        FilePath getCurrentWorkingDirectory()
        {
            return FilePath(getCurrentWorkingDirectoryStr());
        }

        FilePath LAMBDACOMMON_API mkdir(const char *path, bool recursive)
        {
            return mkdir(string(path), recursive);
        }

        FilePath LAMBDACOMMON_API mkdir(std::string path, bool recusrive)
        {
            FilePath result{path};
            result.mkdir(recusrive);
            return result;
        }
    }
}

#undef STAT_STRUCT
#undef STAT_METHOD
