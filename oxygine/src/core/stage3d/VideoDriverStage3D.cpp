#include "VideoDriverStage3d.h"
#include "NativeTextureStage3D.h"
#include "agal.h"
#include "Color.h"
#include "AS3/AS3.h"
#include <Flash++.h>
#include "sdl_timer.h"
#include <sys/time.h>

namespace oxygine
{

	flash::display3D::IndexBuffer3D i3dbuffer;
	flash::display3D::VertexBuffer3D v3dbuffer;



	// If we fail to create the Context3D we display a warning
	var VideoDriverStage3D::_context3DError(void *arg, var as3Args)
	{
		printf("initContext3D error\n");
		return ((VideoDriverStage3D*)arg)->context3DError();
	}

	var VideoDriverStage3D::_frame(void *arg, var as3Args)
	{
		return ((VideoDriverStage3D*)arg)->frame();
	}

	// After a Context3D is created this function will be called.
	var VideoDriverStage3D::_initContext3D(void *arg, var as3Args)
	{
		printf("initContext3D\n");
		return ((VideoDriverStage3D*)arg)->initContext3D();
	}

	var VideoDriverStage3D::initContext3D()
	{
		printf("ag0\n");    
		// We have a context, we just need to configure the backbuffer to fill the
		// stage.
		_ctx3d = _s3d->context3D;
		_ctx3d->enableErrorChecking = true;
		_ctx3d->configureBackBuffer(_stage->stageWidth, _stage->stageHeight, 2,
			true, false);

		com::adobe::utils::AGALMiniAssembler temp;
		printf("ag1 %d\n", temp->_id);    

		com::adobe::utils::AGALMiniAssembler vasm = 
			com::adobe::utils::AGALMiniAssembler::_new(false);

		/*
		vasm->assemble(flash::display3D::Context3DProgramType::VERTEX,
			"mov vt0.w, vc0.x\n"
			"mov vt0.xyz, va0.xyzz\n"
			"m44 op, vt0, vc1\n"
			"mov v0, va1\n"
			"mov v1, vc0\n"
			"mov v1.xy, va2.xyyy\n"
			"mov op, va0");

		//printf("ag2 %d\n", vasm->agalcode->_id);    
		com::adobe::utils::AGALMiniAssembler fasm =
			com::adobe::utils::AGALMiniAssembler::_new(false);
		fasm->assemble(flash::display3D::Context3DProgramType::FRAGMENT,
			"tex ft0, v1.xyyy, fs0 <linear mipdisable repeat 2d>\n"
			"mul oc, v0, ft0");
			*/

		vasm->assemble(flash::display3D::Context3DProgramType::VERTEX,
			//"m44 vt0, va0, vc0\n"
			"mov op, va0\n");

		com::adobe::utils::AGALMiniAssembler fasm =
			com::adobe::utils::AGALMiniAssembler::_new(false);
		fasm->assemble(flash::display3D::Context3DProgramType::FRAGMENT,
			"mov oc, fc0\n");

		//printf("ag3 %d\n", fasm->agalcode->_id);    
		flash::display3D::Program3D program = _ctx3d->createProgram();
		program->upload(vasm->agalcode, fasm->agalcode);
		_ctx3d->setProgram(program);

		/*
		printf("ag4\n");    
		_ib = _ctx3d->createIndexBuffer(_indices16.size());
		_ib->uploadFromByteArray(internal::get_ram(), 
			(int)&_indices16[0], 0, _indices16.size());
		printf("ag5\n");    
		
		*/
		
		//printf("ag6 %d\n", _vb->_id);    
		/*
		com::adobe::utils::PerspectiveMatrix3D projection =
			com::adobe::utils::PerspectiveMatrix3D::_new(internal::_null);
		projection->perspectiveFieldOfViewLH(45.0*M_PI/180.0, 1.2, 0.1, 512);
		_ctx3d->setProgramConstantsFromMatrix(
			flash::display3D::Context3DProgramType::VERTEX, 0, projection, false);
		var vc = internal::new_Vector_Number();
		vc[0] = internal::new_Number(0.5);
		vc[1] = internal::new_Number(0.5);
		vc[2] = internal::new_Number(0.5);
		vc[3] = internal::new_Number(0.5);
		_ctx3d->setProgramConstantsFromVector(flash::display3D::Context3DProgramType::FRAGMENT, 0, vc, -1);
		*/
		printf("ag7\n");    
		
		

		i3dbuffer = _ctx3d->createIndexBuffer(3);
		int indicies[] = {
			2,1,0
		};
		i3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&indicies[0], 0, 3, (void*)&indicies[0]);

		v3dbuffer = _ctx3d->createVertexBuffer(300, 3);
		float verticies[] = {
			-0.5, -0.5, 0,
			0, 0.5, 0,
			0.5, -0.5, 0,
		};
		v3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&verticies[0], 0, 3, (void*)&verticies[0]);
		_ctx3d->setVertexBufferAt(0, v3dbuffer, 0, flash::display3D::Context3DVertexBufferFormat::FLOAT_3);

		_stage->addEventListener(flash::events::Event::ENTER_FRAME, Function::_new(&VideoDriverStage3D::_frame, this));

		_initialized = true;

		return internal::_undefined;
	}

	

	var VideoDriverStage3D::context3DError()
	{
		return internal::_undefined;
	}

	/*
	var enterFrame(void *arg, var as3Args)
	{
		static int tc = 0;
		try {
			ctx3d->clear((sin(tc++ / 10.0f) * 0.5) + 0.5, 0, 0, 1, 1, 0, 0xffffffff);
			//ctx3d->drawTriangles(i3dbuffer, 0, -1);
			ctx3d->present();
		} catch(var e) {
			char *err = internal::utf8_toString(e);
			//std::cout << "Exception: " << err << std::endl;
			free(err);
		}
		return internal::_undefined;
	}
	*/
	
	void *handleFrame(void *arg) 
	{ 
		/*
		inline_as3(
			"import com.adobe.flascc.CModule;\n"
			"%0 = CModule.activeConsole.mx\n"
			"%1 = CModule.activeConsole.my\n"
			: "=r"(vgl_cur_mx),"=r"(vgl_cur_my) :
		);

		AS3_SendMetricInt("quake1.drawnpolycount", r_drawnpolycount);
		*/
		static int i = 0;
		++i;
	
		//printf("%d\n", i);
		return NULL;
	}

	void VideoDriverStage3D::present()
	{
		//_ctx3d->present();
	}

	spNativeTexture VideoDriverStage3D::createTexture()
	{
		return new NativeTextureStage3D;
	}

	void VideoDriverStage3D::init()
	{
		log::messageln("VideoDriverStage3D::init");

		_stage = internal::get_Stage();
		_stage->scaleMode = flash::display::StageScaleMode::NO_SCALE;
		_stage->align = flash::display::StageAlign::TOP_LEFT;
		_stage->frameRate = 60;

		_s3d = var(var(_stage->stage3Ds)[0]);
		_s3d->addEventListener(flash::events::Event::CONTEXT3D_CREATE, Function::_new(&VideoDriverStage3D::_initContext3D, this));
		_s3d->addEventListener(flash::events::ErrorEvent::ERROR, Function::_new(&VideoDriverStage3D::_context3DError, this));
		_s3d->requestContext3D(flash::display3D::Context3DRenderMode::AUTO,
			flash::display3D::Context3DProfile::BASELINE_CONSTRAINED);

		//log::messageln("VideoDriverStage3D::waiting cb");
		while (!_initialized)
		{
			log::messageln("VideoDriverStage3D::waiting cb");
			SDL_Delay(3);
			log::messageln("VideoDriverStage3D::waiting cb2");
			avm2_ui_thunk(handleFrame, NULL);
			log::messageln("VideoDriverStage3D::waiting cb3");
		}		
		log::messageln("s3d initialized");
	}


	VideoDriverStage3D::VideoDriverStage3D():_initialized(false)
	{
		
	}

	VideoDriverStage3D::~VideoDriverStage3D()
	{

	}

	void VideoDriverStage3D::begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *clearColor)
	{
		
		//glViewport(viewport.getX(), viewport.getY(), viewport.getWidth(), viewport.getHeight());

		if (clearColor)
		{
			log::messageln("clear");
			_ctx3d->clear(clearColor->r / 255.0f, clearColor->g / 255.0f, clearColor->b / 255.0f, clearColor->a / 255.0f);
			//glClearColor(clearColor->getRedF(), clearColor->getGreenF(), clearColor->getBlueF(), clearColor->getAlphaF());
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			//glClear(GL_DEPTH_BUFFER_BIT);
		}

		_matrixVP = view * proj;

		_ctx3d->setProgramConstantsFromByteArray(flash::display3D::Context3DProgramType::VERTEX, 0, 4, internal::get_ram(), (int)_matrixVP.m);

		log::messageln("clean end");
		/*
		_ctx3d->setProgramConstantsFromVector(
			flash::display3D::Context3DProgramType::VERTEX, 0, projection, false);
			*/
	}

	void	VideoDriverStage3D::getViewport(Rect &r) const
	{

	}

	int		VideoDriverStage3D::getMaxVertices() const
	{
		return 255;
	}

	bool	VideoDriverStage3D::getScissorRect(Rect &) const
	{

	}

	var VideoDriverStage3D::frame()
	{
		_ctx3d->drawTriangles(i3dbuffer, 0, -1);
		_ctx3d->present();

		return internal::_undefined;
	}

	void VideoDriverStage3D::drawBatch(const batch &b)
	{
		return;
		_ctx3d->drawTriangles(i3dbuffer, 0, -1);
		return;

		
		flash::display3D::textures::Texture t = ((NativeTextureStage3D*)b.texture.get())->getTexture();
		_ctx3d->setTextureAt(0, t);

		log::messageln("drawBatch1");
		int count = b.vertices.size();
		int indices = (count * 3)/2;
		const vertex *p = &b.vertices[0];

		flash::display3D::VertexBuffer3D _vb = _ctx3d->createVertexBuffer(count, sizeof(vertex) / sizeof(int));
		_vb->uploadFromByteArray(internal::get_ram(),
			(int)p, 0, b.vertices.size());
		log::messageln("drawBatch2");
		_ctx3d->setVertexBufferAt(0, _vb, 0, flash::display3D::Context3DVertexBufferFormat::FLOAT_3);
		int i = offsetof(vertex, color) / sizeof(int);
		//printf("vu %d", i);
		_ctx3d->setVertexBufferAt(1, _vb, i, flash::display3D::Context3DVertexBufferFormat::BYTES_4);
		i = offsetof(vertex, u) / sizeof(int);
		//printf("vu %d", i);
		_ctx3d->setVertexBufferAt(2, _vb, i, flash::display3D::Context3DVertexBufferFormat::FLOAT_2);
		log::messageln("drawBatch3");
		//unsigned int id = (unsigned int)b.texture->getHandle();
				
		//if (_blend != b.blend)
		{
			//_blend = b.blend;

			/*

			glBlendEquation(GL_FUNC_ADD);
			bool blend = true;
			switch(b.blend)
			{
			case blend_disabled:
				blend = false;
				break;

			case blend_premultiplied_alpha:
				glBlendFunc  (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case blend_alpha:
				glBlendFunc  (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case blend_add:
				glBlendFunc  (GL_ONE, GL_ONE);
				break;
			case blend_sub:
				glBlendFunc  (GL_ONE, GL_ONE);
				glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
				break;
			default:
				OX_ASSERT(!"unknown blend");
			}				

			if (blend)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
				*/
		}			


		//glBindTexture(GL_TEXTURE_2D, id);

		/*
		int p1 = glGetAttribLocation(_currentShaderProgram, "position");
		glVertexAttribPointer(p1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), &p->x);		
		glEnableVertexAttribArray(p1);

		int p2 = glGetAttribLocation(_currentShaderProgram, "color");
		glVertexAttribPointer(p2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex), &p->color);
		glEnableVertexAttribArray(p2);

		int p3 = glGetAttribLocation(_currentShaderProgram, "uv");
		glVertexAttribPointer(p3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &p->u);
		glEnableVertexAttribArray(p3);



		if (indices <= (int)_indices8.size())
			glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_BYTE, &_indices8[0]);
		else
			glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, &_indices16[0]);
			*/
		log::messageln("tris: %d", indices/3);
		_ctx3d->drawTriangles(_ib, 0, indices/3);
		log::messageln("drawBatch4");
	}

	void VideoDriverStage3D::setScissorRect(const Rect *)
	{

	}

	void VideoDriverStage3D::setDefaultSettings()
	{
		 _ctx3d->setCulling(flash::display3D::Context3DTriangleFace::NONE);
		 _ctx3d->setDepthTest(false, "always");
	}

	void VideoDriverStage3D::setRenderTarget(spNativeTexture)
	{

	}
}