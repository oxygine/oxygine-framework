#include "STDRenderer.h"
#include "core/UberShaderProgram.h"
#include "core/VertexDeclaration.h"
#include "RenderState.h"

namespace oxygine
{
	STDRenderer::STDRenderer(IVideoDriver *driver) :Renderer(driver), _uberShader(0), _shaderFlags(0), _blend(blend_disabled)
	{
		_uberShader = &uberShader;
	}

	void STDRenderer::setBlendMode(blend_mode blend)
	{
		if (_blend != blend)
		{
			drawBatch();

			switch (blend)
			{
			case blend_disabled:
				_driver->setState(IVideoDriver::STATE_BLEND, 0);
				break;
			case blend_premultiplied_alpha:
				_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
				break;
			case blend_alpha:
				_driver->setBlendFunc(IVideoDriver::BT_SRC_ALPHA, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
				break;
			case blend_add:
				_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE);
				break;
				//case blend_sub:
				//_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE);
				//glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
				//	break;
			default:
				OX_ASSERT(!"unknown blend");
			}

			if (_blend == blend_disabled)
			{
				_driver->setState(IVideoDriver::STATE_BLEND, 1);
			}
			_blend = blend;
		}
	}
		
	template <class T>
	void append(vector<unsigned char> &buff, const T &t)
	{
		const unsigned char *ptr = (const unsigned char *)&t;
		buff.insert(buff.end(), ptr, ptr + sizeof(t));
	}

	void STDRenderer::setTexture(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied)
	{
		if (base == 0 || base->getHandle() == 0)
			base = white;

		unsigned int shaderFlags = _shaderFlags;

		if (basePremultiplied)
			shaderFlags &= ~UberShaderProgram::ALPHA_PREMULTIPLY;
		else
			shaderFlags |= UberShaderProgram::ALPHA_PREMULTIPLY;

		if (alpha)
			shaderFlags |= UberShaderProgram::SEPARATE_ALPHA;
		else
			shaderFlags &= ~UberShaderProgram::SEPARATE_ALPHA;

		//##ifdef OX_DEBUG
#if 0
		if (_base != base){ OX_ASSERT(_alpha != alpha); }
		else{ OX_ASSERT(_alpha == alpha); }
#endif //OX_DEBUG

		//no reason to check changed alpha because it is in pair with base
		if (_base != base || /*_alpha != alpha || */_shaderFlags != shaderFlags)
		{
			drawBatch();
		}

		_shaderFlags = shaderFlags;

		_base = base;
		_alpha = alpha;
	}
	
	void STDRenderer::_begin()
	{
		_base = 0;
		_alpha = 0;
		_blend = blend_disabled;
	}

	void STDRenderer::_resetSettings()
	{
		_blend = blend_disabled;
	}

	void STDRenderer::_cleanup()
	{
		_base = 0;
		_alpha = 0;
	}

	void STDRenderer::preDrawBatch()
	{
		ShaderProgram *prog = _uberShader->getShaderProgram(_shaderFlags)->program;
		setShader(prog);

		_driver->setTexture(UberShaderProgram::SAMPLER_BASE, _base);

		if (_alpha)		
			_driver->setTexture(UberShaderProgram::SAMPLER_ALPHA, _alpha);

		UberShaderProgramBase::ShaderUniformsCallback cb = _uberShader->getShaderUniformsCallback();
		if (cb)
		{
			cb(_driver, prog);
		}
	}

	void STDRenderer::draw(const RState *rs, const Color &clr, const RectF &srcRect, const RectF &destRect)
	{
		Color color = clr;
		color.a = (int(color.a) * rs->alpha) / 255;
		if (_blend == blend_premultiplied_alpha)
			color = color.premultiplied();

		vertexPCT2 v[4];
		fillQuadT(v, srcRect, destRect, rs->transform, color.rgba());

		addVertices(v, sizeof(v));
	}

	void STDRenderer::setUberShaderProgram(UberShaderProgramBase* pr)
	{
		if (_uberShader != pr)
		{
			drawBatch();
		}

		_uberShader = pr;
	}

	PrimitiveRenderer::PrimitiveRenderer(STDRenderer *r) :_renderer(r)
	{

	}

	void PrimitiveRenderer::setTexture(spNativeTexture texture)
	{
		//_renderer->setTexture(texture);
	}

	void PrimitiveRenderer::setBlendMode(blend_mode mode)
	{
		//_renderer->setBlendMode(mode);
	}

	void PrimitiveRenderer::draw(const void *data, int size, bvertex_format format, bool worldTransform)
	{
		//_renderer->addVertices()
	}

	TextRenderer2::TextRenderer2(STDRenderer *r) :_renderer(r)
	{
		//_renderer->setBlendMode(Renderer::getPremultipliedAlphaRender() ? blend_premultiplied_alpha : blend_alpha);
	}

	void TextRenderer2::draw(const AffineTransform &tr, spNativeTexture texture, unsigned int color, const RectF &srcRect, const RectF &destRect)
	{
		if (_texture != texture)
		{			
			_renderer->setTexture(texture, 0, true);
			_texture = texture;
		}

		vertexPCT2 v[4];
		fillQuadT(v, srcRect, destRect, tr, color);

		_renderer->addVertices(v, sizeof(v));
	}
}
