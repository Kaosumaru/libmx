#ifndef MXMXPARSER
#define MXMXPARSER
#include "RPN/Parser.h"


namespace MX
{
	class RPNParser : public RPN::Parser
	{
	public:
		RPNParser();


		static RPNParser& Default()
		{
			static RPNParser p;
			return p;
		}

	protected:

	};
}



#endif