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
			UpdateMVP();
		}

		const auto& mvp()
		{
			return _mvp;
		}

		void Push()
		{
			auto c = current();
			_stack.push_back(c);
		}

		void Pop()
		{
			_stack.pop_back();
			UpdateMVP();
		}

	protected:
		void UpdateMVP()
		{
			_mvp = current()._projection * current()._view * current()._model;
		}

		struct Data
		{
			glm::mat4 _projection;
			glm::mat4 _view;
			glm::mat4 _model;
		};

		Data& current() { return _stack.back(); }

		std::vector<Data> _stack{ 1 };
		glm::mat4 _mvp;
	};

}
}
