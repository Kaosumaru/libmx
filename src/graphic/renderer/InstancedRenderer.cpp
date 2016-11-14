#include "InstancedRenderer.h"
#include <iostream>
#include "graphic/opengl/Uniform.h"
#include "graphic/opengl/MVP.h"

using namespace MX;
using namespace MX::Graphic;

InstancedRenderer::InstanceData InstancedRenderer::instances[InstancedRenderer::_maxInstances];
std::shared_ptr<InstancedRenderer::StaticOGData> InstancedRenderer::_data;

std::shared_ptr<InstancedRenderer::StaticOGData> InstancedRenderer::_d1;
std::shared_ptr<InstancedRenderer::StaticOGData> InstancedRenderer::_d2;

int InstancedRenderer::_currentInstance = 0;

void InstancedRenderer::InitData()
{
	auto initialize_data = []()
	{
		auto data = std::make_shared<StaticOGData>();

		static const GLfloat g_vertex_buffer_data[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
		};

		data->_vboVertices.Bind();
		data->_vboVertices.Data(g_vertex_buffer_data, GL_STATIC_DRAW);


		data->_vbo.Bind();
		data->_vbo.Data(nullptr, sizeof(instances), GL_STATIC_DRAW);

		return data;
	};

	if (!_data)
	{
		_d1 = initialize_data();
		_d2 = initialize_data();
		_data = _d1;
	}
}

InstancedRenderer::InstancedRenderer()
{
	InitData();
	std::string out;
	_program = gl::createProgramFromFiles("shader/instanced.vertex", "shader/basic.fragment", out);
	if (_program)
	{
		std::cout << "Shader compiled" << std::endl;

		_mvp_uniform = _program.GetUniformLocation("MVP");
		//shader->uniform("tex0", 0);
		_vertices_attribute = _program.GetAttribLocation("squareVertices");
		_position_attribute = _program.GetAttribLocation("xywh");
		_color_attribute = _program.GetAttribLocation("instance_color");
		_angle_attribute = _program.GetAttribLocation("instance_angle");
		_uv_attribute = _program.GetAttribLocation("instance_uv");
		_center_attribute = _program.GetAttribLocation("instance_center");
	}
	else
	{
		std::cout << "Shader error: " << out << std::endl;
	}
}

void InstancedRenderer::Flush()
{
	DrawBatched();
}
void InstancedRenderer::Draw(const Texture& tex, const Rectangle& srcArea, const glm::vec2& pos, const glm::vec2& relativeCenter, const glm::vec2& scale, const MX::Color& color, float angle)
{
	if (tex.getId() != _lastTex || _currentInstance + 4 > _maxInstances)
	{
		DrawBatched();
		_lastTex = tex.getId();
	}
	auto &instance = nextInstance();

	instance.pos = { pos.x, pos.y, scale.x, scale.y };
	instance.center = { relativeCenter.x, relativeCenter.y };

	instance.color[0] = color.r();
	instance.color[1] = color.g();
	instance.color[2] = color.b();
	instance.color[3] = color.a();

	instance.angle = angle;

#ifdef WIP
	//auto coords = tex->getAreaTexCoords(srcArea);
	//instance.uv = { coords.x1, coords.y1, coords.x2, coords.y2 };


	if (Image::Settings::flipX)
		std::swap(instance.uv[0], instance.uv[2]);
	if (Image::Settings::flipY)
		std::swap(instance.uv[1], instance.uv[3]);
#endif
}

void InstancedRenderer::DrawBatched()
{
	if (_currentInstance == 0)
		return;

#ifdef WIP
	//glBindTexture(GL_TEXTURE_2D, _lastTex);
#endif

	_program.Use();

	auto& mvp = MX::gl::MVP::get().mvp();
	MX::gl::Uniform(_mvp_uniform, mvp);



	glEnableVertexAttribArray(_vertices_attribute);
	glBindBuffer(GL_ARRAY_BUFFER, _data->_vboVertices);
	glVertexAttribPointer(
		_vertices_attribute,                  
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, _data->_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData)* _currentInstance, instances, GL_STREAM_DRAW);


	glEnableVertexAttribArray(_position_attribute);
	glVertexAttribPointer(
		_position_attribute,                  
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(InstanceData),                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(_center_attribute);
	glVertexAttribPointer(
		_center_attribute,
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(InstanceData),                  // stride
		(void*)(4 * 4)            // array buffer offset
	);

	glEnableVertexAttribArray(_uv_attribute);
	glVertexAttribPointer(
		_uv_attribute,
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(InstanceData),                  // stride
		(void*)(6 * 4)            // array buffer offset
	);

	glEnableVertexAttribArray(_color_attribute);
	glVertexAttribPointer(
		_color_attribute,                  
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(InstanceData),                  // stride
		(void*)(10*4)            // array buffer offset
	);

	glEnableVertexAttribArray(_angle_attribute);
	glVertexAttribPointer(
		_angle_attribute,
		1,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(InstanceData),                  // stride
		(void*)(14 * 4)            // array buffer offset
	);




	glVertexAttribDivisor(_vertices_attribute, 0);
	glVertexAttribDivisor(_position_attribute, 1);
	glVertexAttribDivisor(_center_attribute, 1);
	glVertexAttribDivisor(_uv_attribute, 1);
	glVertexAttribDivisor(_color_attribute, 1);
	glVertexAttribDivisor(_angle_attribute, 1);

	//glext_ARB_draw_instanced()
	//glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _currentInstance);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _currentInstance);


	glDisableVertexAttribArray(_vertices_attribute);
	glDisableVertexAttribArray(_position_attribute);
	glDisableVertexAttribArray(_uv_attribute);
	glDisableVertexAttribArray(_color_attribute);
	glDisableVertexAttribArray(_angle_attribute);
	glDisableVertexAttribArray(_center_attribute);



	glBufferSubData(GL_ARRAY_BUFFER, 0, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);



	if (_data == _d1)
		_data = _d2;
	else
		_data = _d1;

	_lastTex = -1;
	_currentInstance = 0;
}

void InstancedRenderer::OnStarted()
{

}

void InstancedRenderer::OnEnded()
{
	DrawBatched();
	_lastTex = -1;
}