#pragma once
#include <string>
#include "script/serialization/Serializer.h"

namespace MX
{
	namespace Serialization
	{
		class Node
		{
		public:
			Node() {}
			Node(const std::string& path) : _path(path) {}
			Node(Node* parent, const std::string& path) : _path(path) {}

			Node operator() (const std::string& path)
			{
				return { this, path };
			}

			template<typename T>
			void Sync(T &t)
			{
				Context<Serializer>::current().Sync(*this, t);
			}
			auto s()
			{
				return Context<Serializer>::current();
			}
		protected:
			std::string _path;
		};


		void operator & (bool& v, Node&& n)
		{
			n.Sync(v);
		}

		void operator & (int& v, Node&& n)
		{
			n.Sync(v);
		}

		void operator & (double& v, Node&& n)
		{
			n.Sync(v);
		}

		void operator & (std::string& v, Node&& n)
		{
			n.Sync(v);
		}
	}
}
