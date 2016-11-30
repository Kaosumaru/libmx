#include <memory.h>
#include <iostream>
#include <sstream>
#include "json_cpp.h"


using namespace JSON;


Node::pointer Node::CreateFromMemory(char *sourceJSON, const std::string& info)
{
    char *errorPos = 0;
    char *errorDesc = 0;
    int errorLine = 0;
    block_allocator allocator(1 << 10);
    
    json_value *root = json_parse(sourceJSON, &errorPos, &errorDesc, &errorLine, &allocator);
    
	if (root)
    {
        return CreateFromJSONValue(root);
    }

	if (errorPos || errorDesc)
	{
		std::stringstream ss;
		ss << "Error in parsing file '" << info << "'" << std::endl;
		ss << "Pos: " << errorPos << " line: " << errorLine << " " << std::endl;
		ss << errorDesc;

		std::cout << "---------------------" << std::endl;
		std::cout << ss.str() << std::endl;
		std::cout << "---------------------" << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	}
	return nullptr;
}

Node::pointer Node::CreateFromFile(const std::string& path)
{
 	FILE *fp = fopen(path.c_str(), "rb");
    if (fp)
    {
            fseek(fp, 0, SEEK_END);
            int size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            std::vector<char> buffer(size + 1);
            fread (&buffer[0], 1, size, fp);
            fclose(fp);
            return CreateFromMemory(buffer.data(), path);
    }
	return nullptr;
}

 const Node& Node::operator[] (unsigned index) const
{ 
	auto &array = getArray();
	if (index >= array.size())
		return Null();

	return *(getArray()[index]); 
}

 const Node& Node::operator[] (const std::string &str)  const
{ 
	auto map = getObjects();
	auto fit = map.find(str);
	if (fit == map.end())
		return Null();

	return *(fit->second); 
}

const Node& Node::Null()
{
	static Node dummy;
	return dummy;
}







class NodeInt : public Node
{
public:
	NodeInt(json_value *value) { _value = value->int_value; }

	int getInt() const { return _value; }
	float getFloat() const { return (float) _value; }
	bool getBool() const { return _value != 0; }

protected:
	int _value;
};

class NodeFloat : public Node
{
public:
	NodeFloat(json_value *value) { _value = value->float_value; }

	int getInt() const { return (int)_value; }
	float getFloat() const { return (float) _value; }
	bool getBool() const { return _value != 0.0f; }

protected:
	float _value;
};

class NodeBool : public Node
{
public:
	NodeBool(json_value *value) { _value = value->int_value != 0; }

	bool getBool() const { return _value; }

protected:
	bool _value;
};

class NodeString : public Node
{
public:
	NodeString(json_value *value) : _value(value->string_value)  {  }


	int getInt() const { return stoi(_value); }
	float getFloat() const { return stof(_value); }
	bool getBool() const { return _value == "true"; }
	const std::string &getString() const { return _value; }

protected:
	std::string _value;
};


class NodeObject : public Node
{
public:
	NodeObject(json_value *value)  
	{
		for (auto it = value->first_child; it; it = it->next_sibling)
			_objects.insert(std::make_pair(it->name, CreateFromJSONValue(it)));
	}

	const map& getObjects() const { return _objects; }

protected:
	map _objects;
};

class NodeArray : public Node
{
public:
	NodeArray(json_value *value)  
	{
		for (auto it = value->first_child; it; it = it->next_sibling)
			_array.emplace_back(CreateFromJSONValue(it));
	}

	const array& getArray() const { return _array; }

protected:
	array _array;
};

Node::pointer Node::CreateFromJSONValue(json_value *root)
{
	static pointer dummy = std::make_shared<Node>();
	if (root == nullptr)
		return dummy;

	switch(root->type)
	{
		case JSON_nullptr:
			return dummy;
		case JSON_OBJECT:
			return std::make_shared<NodeObject>(root);
		case JSON_ARRAY:
			return std::make_shared<NodeArray>(root);
		case JSON_STRING:
			return std::make_shared<NodeString>(root);
		case JSON_INT:
			return std::make_shared<NodeInt>(root);
		case JSON_FLOAT:
			return std::make_shared<NodeFloat>(root);
		case JSON_BOOL:
			return std::make_shared<NodeBool>(root);
	}


	return nullptr;

}