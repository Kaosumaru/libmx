#pragma once
#include "graphic/OpenGL.h"
#include "glm/glm.hpp"
#include "utils/Singleton.h"
#include <vector>

namespace MX
{
namespace gl
{
	class MVP : public Singleton<MVP>
	{
	public:
		void SetProjection(const glm::mat4& m)
		{
			current()._projection = m;
			current().UpdateMVP();
		}

		const auto& mvp()
		{
			return current()._mvp;
		}

		void Push()
		{
			auto c = current();
			_stack.push_back(c);
		}

		void Pop()
		{
			_stack.pop_back();
		}

	protected:

		struct Data
		{
			glm::mat4 _projection;
			glm::mat4 _view;
			glm::mat4 _model;
			glm::mat4 _mvp;

			void UpdateMVP()
			{
				_mvp = _projection * _view * _model;
			}
		};

		Data& current() { return _stack.back(); }

		std::vector<Data> _stack{ 1 };
	};

}
}
