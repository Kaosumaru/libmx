#pragma once
#include <string>
#include "script/serialization/Serializer.h"
#include "script/serialization/Node.h"
#include <iostream>
#include <sstream>
#include <map>
#include "utils/Utils.h"

namespace MX
{
	namespace Serialization
	{
		namespace
		{
			template<typename Stream>
			void AddPathToStream(Stream &ss, Node& n)
			{
				n.ApplyPath([&](const std::string& s, int index) { ss << s; if (index>=0) ss << index; });
			}
		}

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
				AddPathToStream(_stream, n);
				_stream << ": ";
				_stream << v;
				_stream << "\r\n";
			}

			std::ostream& _stream;
		};

		class MapDeserializer : public Serializer
		{
		public:
			MapDeserializer() {}
			virtual ~MapDeserializer() {}

			bool loading() override { return true; }

			void Sync(Node& n, int &v) override { v = std::stoi(valueOf(n)); };
			void Sync(Node& n, int64_t &v) override { v = std::stoll(valueOf(n)); };
			void Sync(Node& n, double &v) override { v = std::stod(valueOf(n)); };
			void Sync(Node& n, std::string& v) override { v = valueOf(n); };
			void Sync(Node& n, std::wstring& v) override { v = stringToWide(valueOf(n)); };
		protected:
			void LoadFromStream(std::istream& stream)
			{
				while (stream && !stream.eof())
				{
					//TODO escaping, errors
					std::string k;
					std::getline(stream, k, ':');
					stream.ignore(1);
					std::string v;
					std::getline(stream, v, '\r');
					stream.ignore(1);

					_map.emplace(std::move(k), std::move(v));
				}
			}

			void AddValue(std::string n, std::string v)
			{
				_map.emplace(std::move(n), std::move(v));
			}

			const std::string& valueOf(Node& n)
			{
				static std::string v;
				std::stringstream ss;
				AddPathToStream(ss, n);
				auto it = _map.find(ss.str());
				if (it == _map.end()) return v;
				return it->second;
			}

			std::map<std::string, std::string> _map;
		};

		class SyncStreamDeserializer : public MapDeserializer
		{
		public:
			SyncStreamDeserializer(std::istream& stream) { LoadFromStream(stream); }
		};
	}
}
