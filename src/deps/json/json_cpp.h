#ifndef JSONCPP_H
#define JSONCPP_H

#include "json.h"
#include <memory>
#include <map>
#include <vector>

namespace JSON
{

class Node
{
public:
	Node() {}
	virtual ~Node() {}


	typedef std::shared_ptr<Node> pointer;
	typedef std::vector<pointer> array;
	typedef std::multimap<std::string, pointer> map;

	static pointer CreateFromMemory(char *sourceJSON, const std::string& info = "");
	static pointer CreateFromFile(const std::string& path);

	const Node& operator[] (unsigned index) const;
	const Node& operator[] (const std::string &str) const;

	array::const_iterator begin() const { return getArray().begin(); }
	array::const_iterator end() const { return getArray().end(); }

	virtual int getInt() const { return 0; }
	virtual float getFloat() const { return 0.0f; }
	virtual bool getBool() const { return false; }
	virtual const std::string &getString() const { static std::string dummy; return dummy; }
	virtual const map& getObjects() const { static map dummy; return dummy; }
	virtual const array& getArray() const { static array dummy; return dummy; }

	static const Node& Null();
protected:
	static pointer CreateFromJSONValue(json_value *root);
};

}

#endif
