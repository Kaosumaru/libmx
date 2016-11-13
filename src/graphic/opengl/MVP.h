#pragma once
#include "graphic/OpenGL.h"
#include "glm/glm.hpp"
#include "utils/Singleton.h"

namespace MX
{
namespace gl
{
	class MVP : public Singleton<MVP>
	{
	public:
		void SetProjection(const glm::mat4& m)
		{
			_projection = m;
			UpdateMVP();
		}

		const auto& mvp()
		{
			return _mvp;
		}

	protected:
		void UpdateMVP()
		{
			_mvp = _projection * _view * _model;
		}

		glm::mat4 _projection;
		glm::mat4 _view;
		glm::mat4 _model;

		glm::mat4 _mvp;
	};

}
}
