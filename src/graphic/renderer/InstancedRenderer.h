#pragma once
#include "TextureRenderer.h"
#include "graphic/opengl/ProgramInstance.h"
#include "graphic/opengl/Buffer.h"
#include <string>

namespace MX{
namespace Graphic
{

class InstancedRenderer : public TextureRenderer
{
private:
	void InitData();
public:
    InstancedRenderer(const std::shared_ptr<gl::ProgramInstance>& instance);

	static std::shared_ptr<InstancedRenderer> Create(const char* fragmentPath, const char* vertexPath = nullptr);
    static std::shared_ptr<InstancedRenderer> Create(const std::shared_ptr<gl::ProgramInstance>& instance);

	void Flush() override;
	void Draw(const gl::Texture& tex, const Rectangle& srcArea, const glm::vec2& pos, const glm::vec2& relativeCenter, const glm::vec2& size, const MX::Color& color, float angle) override;

protected:
	void DrawBatched();
	void OnStarted() override;
	void OnEnded() override;
	void OnSetAsCurrent() override;

	const static int _maxInstances = 5000;
	int _minInstancesToOptimize = 5;

	struct InstanceData
	{
		glm::vec4 pos; //0
		glm::vec2 center; //4
		glm::vec4 uv; //6
		glm::vec4 color; //10
		float angle; //14
	};



	InstanceData &nextInstance() { return instances[_currentInstance++]; }

	static InstanceData instances[_maxInstances];
	static int _currentInstance;

	struct StaticOGData
	{
		gl::Buffer _vbo{gl::Buffer::Type::Array};
		gl::Buffer _vboVertices{gl::Buffer::Type::Array};
	};

	static std::shared_ptr<StaticOGData> _d1, _d2;
	static std::shared_ptr<StaticOGData> _data;

	std::shared_ptr<gl::ProgramInstance> _programInstance;

	GLuint _mvp_uniform;
	GLuint _vertices_attribute;
	GLuint _angle_attribute;
	GLuint _position_attribute;
	GLuint _center_attribute;
	GLuint _uv_attribute;
	GLuint _color_attribute;

	GLuint _lastTex = -1;
};





}
}
