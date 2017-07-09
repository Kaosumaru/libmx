#pragma once
#include <string>
#include "script/serialization/StreamSerializer.h"
#include "script/serialization/archive/Archive.h"
#include <iostream>
#include <sstream>
#include <map>
#include "utils/Utils.h"

namespace MX
{
	namespace Serialization
	{
		template<typename T>
		class RootArchiveObject
		{
		public:
			using Callback = Archive::SaveCallback;

			RootArchiveObject(const std::string& path) : _serializationPath(path) {}

			void Save(const Callback& cb)
			{
				std::stringstream ss;
				MX::Serialization::OutputStreamSerializer s1{ ss };
				MX::Serialization::SyncObject(s1, "R", (T&)*this);

				auto archive = Archive::CreateDefault(_serializationPath);
				archive->Save(ss.str(), [cb](Archive::Result r) 
				{
					if (cb) cb(r);
				});
			}

			void Load(const Callback& cb)
			{
				auto archive = Archive::CreateDefault(_serializationPath);
				//TODO this should addref our object
				archive->Load([this, cb](std::string&& str, Archive::Result r) 
				{
					if (r != Archive::Result::OK)
					{
						cb(r);
						return;
					}

					std::stringstream ss{ str };
					MX::Serialization::SyncStreamDeserializer d1{ ss };
					MX::Serialization::SyncObject(d1, "R", (T&)*this);
					if (cb) cb(r);
				});

			}

		private:
			std::string _serializationPath;
		};
	}
}
