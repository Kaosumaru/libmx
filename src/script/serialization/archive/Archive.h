#pragma once
#include <string>
#include <memory>
#include <functional>
#include "utils/Singleton.h"

namespace MX
{
	namespace Serialization
	{
		class Archive
		{
		public:
			enum class Result
			{
				OK,
				PathNotFound,
				ErrorTemporary,
				ErrorFatal
			};

			using SaveCallback = std::function<void (Result)>;
			using LoadCallback = std::function<void (std::string&&, Result)>;

			Archive(const std::string& path) : _path(path) {}
			virtual ~Archive() {}

			const auto& path() { return _path; }

			virtual void Load(const LoadCallback& cb) = 0;
			virtual void Save(std::string&& data, const SaveCallback& cb) = 0;

			std::unique_ptr<Archive> CreateDefault(const std::string& path);
		protected:
			std::string _path;
		};
	}
}
