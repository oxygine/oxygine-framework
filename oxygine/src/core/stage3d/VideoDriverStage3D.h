#pragma once
#include "VideoDriver.h"
#include "AS3/AS3.h"
#include <Flash++.h>
using namespace AS3::ui;

namespace oxygine
{
	class VideoDriverStage3D: public IVideoDriver
	{
	public:
		VideoDriverStage3D();
		~VideoDriverStage3D();

		void init();
		
		spNativeTexture createTexture();

		void begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *clearColor);

		void	getViewport(Rect &r) const;
		int		getMaxVertices() const;
		bool	getScissorRect(Rect &) const;

		void drawBatch(const batch &b);

		void setScissorRect(const Rect *);
		void setDefaultSettings();
		void setRenderTarget(spNativeTexture);


		void present();
		flash::display3D::Context3D getContext(){return _ctx3d;}

	private:
		static var _initContext3D(void *arg, var as3Args);
		static var _context3DError(void *arg, var as3Args);
		static var _frame(void *arg, var as3Args);

		var initContext3D();
		var context3DError();
		var frame();

		bool _initialized;

		flash::display::Stage3D _s3d;
		flash::display::Stage _stage;
		flash::display3D::Context3D _ctx3d;
		flash::display3D::IndexBuffer3D _ib;
		//flash::display3D::VertexBuffer3D _vb; 

		Matrix _matrixVP;
	};
}