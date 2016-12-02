#include "ListFiles.h"
#include "utils/Time.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifndef __EMSCRIPTEN__
#include "deps/dirent.h"
#else
#include <dirent.h>
#endif

namespace MX
{
	bool ListFiles(const std::string& path, const ListFileCallback& callback)
	{
		DIR *dir = opendir(path.c_str());
		dirent *ent;
		if (dir == NULL) 
			return false;

		while ( (ent = readdir( dir )) != NULL )
		{
			std::string name = ent->d_name;
			if (name == "." || name == "..")
				continue;
			FileData data;
			data.path = path + name;
			data.is_folder = S_ISDIR( ent->d_type );
			callback(data);
		}

		closedir (dir);
		return true;

	}

	bool ListFilesRecursively( const std::string& path, const ListFileCallback& callback )
	{
		return ListFiles( path, [&](const FileData& data) 
		{
			if (data.is_folder)
			{
				ListFilesRecursively(data.path + "/", callback);
				return;
			}

			callback(data);
		});
	}

	std::time_t FileModificationTime(const std::string& path)
	{
		struct stat t_stat;
		t_stat.st_mtime = 0;
		stat( path.c_str(), &t_stat );
		return t_stat.st_mtime;
	}

	std::string FileExtension(const std::string& path)
	{
		auto idx = path.rfind('.');

		if(idx != std::string::npos)
			return path.substr(idx+1);
		return "";
	}

	std::string FileData::extension() const
	{
		return FileExtension(path);
	}
}
