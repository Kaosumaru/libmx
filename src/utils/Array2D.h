#ifndef MXARRAY2D
#define MXARRAY2D
#include<memory>
#include<algorithm>
#include<vector>
#include "glm/vec2.hpp"



namespace MX
{

template<typename T>
class Array2D
{
protected:
	class Column
	{
	public:
		Column(Array2D& arr, int x) : _arr(arr), _x(x) {}


		T& operator [](int y) { return _arr.at(_x, y); }

	protected:
		Array2D& _arr;
		int _x;
	};
public:
	Array2D(int width, int height) : _width(width), _height(height) 
	{
		_data.resize(_width * _height);
	}
	
	bool contains(int x, int y) { return x >= 0 && y >= 0 && x < _width && y < _height; }
	T& at(int x, int y) { return _data[x + y*_width]; }
	const T& at(int x, int y) const { return _data[x + y*_width]; }

	bool contains(const glm::ivec2& v) { return v.x >= 0 && v.y >= 0 && v.x < _width && v.y < _height; }
	T& at(const glm::ivec2& v) { return _data[v.x + v.y*_width]; }
	const T& at(const glm::ivec2& v) const { return _data[v.x + v.y*_width]; }

	Column operator [](int x) { return Row(*this, x); }

	int width() const { return _width; }
	int height() const { return _height; }


	auto begin() { return _data.begin(); }
	auto end() { return _data.end(); }

    auto data() const { return _data.data(); }

    void Resize( int width, int height )
    {
        _width = width;
        _height = height;
        _data.resize(_width * _height);
    }
protected:
	int _width, _height;
	std::vector<T> _data;
};


}

#endif
