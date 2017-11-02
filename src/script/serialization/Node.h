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
			Node(const std::string& id) : _id(id) {}
			Node(Node* parent, const std::string& id) : _parent(parent), _id(id) {}
			Node(Node* parent, int index) : _parent(parent), _index(index) {}

			Node operator() (const std::string& path)
			{
				return { this, path };
			}

			Node operator() (int index)
			{
				return { this, index };
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
					cb(".", _index);
				}
				cb(_id, _index);
			}
		protected:
			Serializer& s()
			{
				return Context<Serializer>::current();
			}

			Node* _parent = nullptr;
			int _index = -1;
			std::string _id;
		};

		template<typename T>
		void SyncObject(MX::Serialization::Serializer& s, const std::string& path, T& t)
		{
			MX::Serialization::Node n(path);
			auto g = Context<MX::Serialization::Serializer>::Lock(s);
			t & std::move(n);
		}


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
