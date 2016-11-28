#include "ListFiles.h"
#include "Utils/Time.h"
#include "deps/dirent.h"

namespace MX
{
	std::vector<std::string> ListFiles(const std::string& path)
	{
		std::vector<std::string> files;
		DIR *dir = opendir(path.c_str());
		dirent *ent;
		if (dir == NULL) 
			return files;

		while ( (ent = readdir( dir )) != NULL )
		{
			std::string name = ent->d_name;
			if (name == "." || name == "..")
				continue;
			files.push_back(name);
		}

		closedir (dir);
		return files;
	}
}
