#pragma once
#include <string>
#include "utils/Singleton.h"

namespace MX
{
	namespace Serialization
	{
		class Node;

		class Serializer : public ScopeSingleton<Serializer>
		{
		public:
			Serializer() {}
			virtual ~Serializer() {}

			template<typename T>
			void SyncRoot(const std::string& path, T& t)
			{
				MX::Serialization::Node n(path);
				auto g = Context<MX::Serialization::Serializer>::Lock(*this);
				t & std::move(n);
			}

			virtual bool loading() { return true; }
			bool saving() { return !loading(); }

			virtual void Sync(Node& n, int &v) {};
			virtual void Sync(Node& n, __int64 &v) {};
			virtual void Sync(Node& n, double &v) {};
			virtual void Sync(Node& n, std::string& v) {};
			virtual void Sync(Node& n, std::wstring& v) {};

		};
	}
}
