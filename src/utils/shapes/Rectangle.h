#ifndef MXRECTANGLE
#define MXRECTANGLE
#include<memory>
#include "Shapes.h"
namespace MX
{

struct Rectangle : public Geometry
{
	float x1 = 0.0f, y1 = 0.0f;
	float x2 = 0.0f, y2 = 0.0f;

	Rectangle(){};
	Rectangle(float x2, float y2) : x2(x2), y2(y2) {}

	explicit Rectangle(const glm::vec2 &v1, const glm::vec2 &v2);
	explicit Rectangle(float x1, float y1,float x2, float y2);
	static Rectangle fromWH(float x1=0, float y1=0,float width=0, float height=0);

	bool intersects(const Rectangle& rectangle) const;
	bool contains(const Rectangle& rectangle) const;
	bool contains(const glm::vec2& point) const;
	void SetPoint(float x, float y);
	void SetPointWH(float x, float y, float w, float h);
	void SetWidth(float value);
	void SetHeight(float value);
	void Expand(float value);
	float width() const;
	float height() const;
	bool empty() const;

	//7 is lefttop, 1 is left down, 9 is rt, 5 is center, etc
	void NumLayoutIn(const Rectangle& container, int n, bool round = true);

	void CenterIn(const Rectangle& container);
	void MoveTo(float x, float y) { Shift(x - x1, y - y1); }
	void MoveTo(const glm::vec2& p) { MoveTo(p.x, p.y); }
	void Shift(float x, float y) { x1 += x; x2 += x; y1 += y; y2 += y; }
	void Shift(const glm::vec2& p) { Shift(p.x, p.y); }


	glm::vec2 closestPoint(const glm::vec2& point) const;
	std::pair < float, int > closestEdge(const glm::vec2& point) const;

	Rectangle intersection(const Rectangle& other);

	glm::vec2 pointTopLeft() const { return glm::vec2(x1, y1); }
	glm::vec2 pointTopRight() const { return glm::vec2(x2, y1); }
	glm::vec2 pointBottomRight() const { return glm::vec2(x2, y2); }
	glm::vec2 pointBottomLeft() const { return glm::vec2(x1, y2); }


	glm::vec2 center() const;
	glm::vec2 randomPointInside() const;

	float& point(int i);

	void Draw();
};


struct Margins
{
	float left = 0.0f, right = 0.0f;
	float top = 0.0f, bottom = 0.0f;

	Margins(float left=0, float right=0,float top=0, float bottom=0);

	static Margins fromRectangle(const Rectangle& r) { return Margins(r.x1, r.x2, r.y1, r.y2); }

	glm::vec2 topLeft() const { return { left, top }; }
	float vMargins() const;
	float hMargins() const;
};


}

#endif
