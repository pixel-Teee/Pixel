#pragma once

namespace Pixel {
	class StencilState {
	public:
		enum StencilFunc {
			NERVER,
			ALWAYS,
			LESS,
			LEQUAL,
			EQUAL
		};

		enum StencilOp {
			KEEP,
			INCREMENT,
			DECREMENT
		};

		enum Face {
			Front,
			Back
		};
		
		StencilState();
		virtual ~StencilState();
	public:
		/*------Stencil Func------*/
		StencilFunc m_func;
		int32_t m_ref;
		uint32_t m_mask;

		//glStencilFunc(GLenum func, GLint ref, GLuint mask)
		/*------Stencil Func------*/

		/*------Stencil Op------*/
		Face m_face;
		StencilOp m_fail;//stencil fail
		StencilOp m_zfail;
		StencilOp m_zpass;
		/*------Stencil Op------*/
	};
}
