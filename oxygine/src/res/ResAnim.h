#pragma once
#include "Resource.h"
#include "AnimationFrame.h"
#include "core/Texture.h"
#include <vector>

namespace oxygine
{
	using namespace std;
	DECLARE_SMART(Texture, spTexture);


	typedef vector<AnimationFrame> animationFrames;
	class MemoryTexture;

	class ResAnim: public Resource
	{
	public:
		ResAnim(Resource *atlas = 0);
		~ResAnim();

		void init(MemoryTexture *original, int columns = 1, int rows = 1, float scaleFactor = 1.0f);
		void init(animationFrames &frames, int columns, float scaleFactor = 1.0f);
		
		
		/*adds additional column. use it only if rows = 1*/
		//void addFrame(const AnimationFrame &frame);

		float					getScaleFactor() const {return _scaleFactor;}
		int						getColumns() const {return _columns;}
		int						getRows() const {return (int)_frames.size() / _columns;}
		int						getTotalFrames() const {return (int)_frames.size();}
		const AnimationFrame&	getFrame(int col, int row) const;
		/**returns frame by index ignoring cols and rows*/
		const AnimationFrame&	getFrame(int index) const;
		/**Returns atlas where this ResAnim was created*/
		Resource*				getAtlas(){return _atlas;}

		void setFrame(int col, int row, const AnimationFrame &frame);

		operator const AnimationFrame &();

	private:
		void _load(LoadResourcesContext *ctx = 0);
		void _unload();

		int _columns;
		Resource *_atlas;
		float _scaleFactor;

		animationFrames _frames;
	};
}