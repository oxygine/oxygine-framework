#pragma once
#include "oxygine_include.h"
#include "ref_counter.h"
#include <vector>
#include <list>
#include "NativeTexture.h"
#include "MemoryTexture.h"
#include "Mutex.h"

namespace oxygine
{
	using namespace std;
	DECLARE_SMART(MemoryTexture, spMemoryTexture);
	DECLARE_SMART(NativeTexture, spNativeTexture);

	class Mem2Native
	{
	public:
		Mem2Native();
		~Mem2Native();
		
		bool isEmpty() const;

		void push(spMemoryTexture src, spNativeTexture dest);
		void update();

		void setUpdateSize(int size){_size = size;}

	private:
		struct texture
		{
			MemoryTexture *src;
			NativeTexture *dest;
		};

		int _size;

		typedef list<texture> textures;
		textures _textures;

		Point _prev;

		void updateNext();
		
		vector<unsigned char> _buffer;
		mutable Mutex _mutex;
	};
}
