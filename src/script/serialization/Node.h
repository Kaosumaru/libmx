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
			Node(Node* parent, const std::string& path) : _parent(parent), _path(path) {}

			Node operator() (const std::string& path)
			{
				return { this, path };
			}

			template<typename T>
			void Sync(T &t)
			{
				s().Sync(*this, t);
			}

			bool saving()
			{
				return s().saving();
			}

			bool loading()
			{
				return s().loading();
			}

			template<typename C>
			void ApplyPath(C cb)
			{
				if (_parent)
				{
					_parent->ApplyPath(cb);
					cb(".");
				}
				cb(_path);
			}
		protected:
			Serializer& s()
			{
				return Context<Serializer>::current();
			}

			Node* _parent = nullptr;
			std::string _path;
		};


		inline void operator & (bool& v, Node&& n)
		{
			int i = v ? 1 : 0;
			n.Sync(i);
			v = i != 0;
		}

		inline void operator & (int& v, Node&& n)
		{
			n.Sync(v);
		}

		inline void operator & (double& v, Node&& n)
		{
			n.Sync(v);
		}

		inline void operator & (std::string& v, Node&& n)
		{
			n.Sync(v);
		}

		inline void operator & (std::wstring& v, Node&& n)
		{
			n.Sync(v);
		}
	}
}
