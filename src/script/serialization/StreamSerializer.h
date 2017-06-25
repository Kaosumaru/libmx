#pragma once
#include <string>
#include "script/serialization/Serializer.h"
#include "script/serialization/Node.h"
#include <iostream>
#include <cstdint>
#include "utils/Utils.h"

namespace MX
{
	namespace Serialization
	{
		class OutputStreamSerializer : public Serializer
		{
		public:
			OutputStreamSerializer(std::ostream& stream) : _stream(stream) {}
			virtual ~OutputStreamSerializer() {}

			bool loading() override { return false; }

			void Sync(Node& n, int &v) override { GenericSync(n, v); };
			void Sync(Node& n, int64_t &v) override { GenericSync(n, v); };
			void Sync(Node& n, double &v) override { GenericSync(n, v); };
			void Sync(Node& n, std::string& v) override { GenericSync(n, v); }; //TODO \r\n is a delimiter
			void Sync(Node& n, std::wstring& v) override { GenericSync(n, wideToUTF(v) ); };
		protected:
			template<typename T>
			void GenericSync(Node& n, const T &v)
			{
				n.ApplyPath([&](const std::string& s) { _stream << s; });
				_stream << ": ";
				_stream << v;
				_stream << "\r\n";
			}

			std::ostream& _stream;
		};
	}
}
