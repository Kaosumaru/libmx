#pragma once

#include<memory>
#include<vector>
#include<functional>
#include<ctime>

namespace MX
{
	struct FileData
	{
		std::string path;
		bool is_folder;

		std::string extension() const;
	};
	using ListFileCallback = std::function<void( const FileData& data )>;
	bool ListFiles(const std::string& path, const ListFileCallback& callback);
	bool ListFilesRecursively(const std::string& path, const ListFileCallback& callback);

	std::time_t FileModificationTime(const std::string& path);
	std::string FileExtension(const std::string& path);
}

