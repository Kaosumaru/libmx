#pragma once
#include "TextureRenderer.h"


namespace MX{
namespace Graphic
{

class InstancedRenderer : public TextureRenderer
{
public:
	void Flush() override;
	void Draw(const Texture& tex, const Rectangle& srcArea, const glm::vec2& pos, const glm::vec2& relativeCenter, const glm::vec2& size, const MX::Color& color, float angle) override;

protected:
	void OnStarted() override;
	void OnEnded() override;

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
};





}
}
