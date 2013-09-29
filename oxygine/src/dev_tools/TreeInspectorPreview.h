#pragma once
#include "oxygine_include.h"
#include "Sprite.h"
#include "core/VertexDeclaration.h"

namespace oxygine
{
	class TreeInspector;

	class VertexDeclarationNull: public VertexDeclaration
	{
	public:
		void init(bvertex_format fmt)
		{
			size = getVertexSize(fmt);
			bformat = fmt;
		}
	};

	class VideoDriverCache: public VideoDriverNull
	{
	public:
		typedef vector<batch> batches;
		batches _batches;
		RectF _bounds;
		AffineTransform _transform;
		mutable VertexDeclarations<VertexDeclarationNull> _declarations;

		VideoDriverCache():_bounds(0,0,0,0){}

		spNativeTexture createTexture(){return 0;}

		void begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *color)
		{

		}
		int getMaxVertices() const
		{
			return IVideoDriver::instance->getMaxVertices();
		}

		const VertexDeclaration *getVertexDeclaration(bvertex_format fmt) const
		{
			return _declarations.get(fmt);
		}

		void setDefaultSettings()
		{

		}

		void setRenderTarget(spNativeTexture)
		{

		}

		

		void drawBatch(const batch &b)
		{
			const vertexPCT2 *v = (const vertexPCT2*)(&b.vertices.front());
			if (_batches.empty())
			{
				OX_ASSERT(b.vertices.size());				
				_bounds = RectF(v->x, v->y, 0, 0);				
			}

			_batches.push_back(b);
			size_t num = b.vertices.size() / b.vdecl->size;
			
			for (size_t i = 0; i != num; ++i)
			{				
				v = (const vertexPCT2*)(&b.vertices.front() + b.vdecl->size * i);
				RectF f(v->x, v->y, 0, 0);
				_bounds.unite(f);				
			}
		}

		void render(const AffineTransform &transform)
		{
			static batch modified;
			

			for (batches::iterator i = _batches.begin(); i != _batches.end(); ++i)
			{
				const batch &b = *i;

				modified = b;
				
				size_t num = b.vertices.size() / b.vdecl->size;

				for (size_t i = 0; i != num; ++i)
				{				
					vertexPCT2* v = (vertexPCT2*)(&modified.vertices.front() + b.vdecl->size * i);
					Vector2 np = transform.transform(Vector2(v->x, v->y));
					v->x = np.x;
					v->y = np.y;
				}

				modified.vdecl = IVideoDriver::instance->getVertexDeclaration(modified.vdecl->bformat);

				IVideoDriver::instance->drawBatch(modified);
			}

			modified.base = 0;
			modified.alpha = 0;
			modified.mask = 0;
		}
	};


	class TreeInspectorPreview: public Sprite
	{
	public:
		TreeInspectorPreview(TreeInspector *tree);
		~TreeInspectorPreview();

		void init(spActor item);

		void doRender(RenderState const& parentRenderState);

	private:
		friend class TreeInspector;
		//bool _onEvent(const EventState &es);

		VideoDriverCache _videoCache;
		AffineTransform _cacheTransform;

		Rect _getItemRect();

		Actor *_prevParent;

		TreeInspector *_tree;
		bool _drawChildren;
	};
}