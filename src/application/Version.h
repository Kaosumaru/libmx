#ifndef MXVERSION
#define MXVERSION

#include <string>
#include <sstream>

namespace MX
{

	class Version
	{
	public:
		Version(){}
		Version(int major, int minor, int patch)
		{
			_major = major;
			_minor = minor;
			_patch = patch;
		}


		std::string toString() const
		{
			std::stringstream ss;
			ss << _major << ".";
			ss << _minor << ".";
			ss << _patch;
			return ss.str();
		}

	protected:
		int _major = 0;
		int _minor = 0;
		int _patch = 0;

	};


}


#endif
