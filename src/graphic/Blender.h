#pragma once
#include "Utils/Singleton.h"

namespace MX{
namespace Graphic
{
class Blender : public ScopeSingletonFunction<Blender>
{
public:
    friend class ScopeSingletonFunction<Blender>;
	Blender(int op, int src, int dst);
	Blender(int op, int src, int dst, int alpha_op, int alpha_src, int alpha_dst);
    ~Blender();

	bool operator == (const Blender &other) const;

	static Blender& defaultNormal();
	static Blender& defaultAdditive();
	static Blender& defaultCopy();
	static Blender& defaultPremultiplied();

	void Apply();
protected:
	static void CurrentWasSet(Blender* target, Blender* old_target);
    int _op, _src, _dst;
	int _alpha_op, _alpha_src, _alpha_dst;
};

}
}
