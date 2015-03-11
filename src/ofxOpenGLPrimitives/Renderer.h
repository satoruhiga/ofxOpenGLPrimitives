#pragma once

#include "RendererCapability.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

template <
	typename Program,
	typename C0 = detail::NullCapability0,
	typename C1 = detail::NullCapability1,
	typename C2 = detail::NullCapability2,
	typename C3 = detail::NullCapability3,
	typename C4 = detail::NullCapability4,
	typename C5 = detail::NullCapability5,
	typename C6 = detail::NullCapability6,
	typename C7 = detail::NullCapability7
>
class Renderer_
	: public Program
	, public C0
	, public C1
	, public C2
	, public C3
	, public C4
	, public C5
	, public C6
	, public C7
{
public:
	Renderer_()
		: Program()
		, C0(this)
		, C1(this)
		, C2(this)
		, C3(this)
		, C4(this)
		, C5(this)
		, C6(this)
		, C7(this)
	{}
	
	template <typename T>
	void draw(const T& drawable, GLsizei primcount = 1)
	{
		Program::use();
		
		C0::preDraw();
		C1::preDraw();
		C2::preDraw();
		C3::preDraw();
		C4::preDraw();
		C5::preDraw();
		C6::preDraw();
		C7::preDraw();

		drawable.drawInstanced(primcount);

		C0::postDraw();
		C1::postDraw();
		C2::postDraw();
		C3::postDraw();
		C4::postDraw();
		C5::postDraw();
		C6::postDraw();
		C7::postDraw();
	}
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE