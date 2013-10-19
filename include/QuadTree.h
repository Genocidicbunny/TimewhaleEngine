#ifndef __TIMEWHALE_QUADTREE_H_
#define __TIMEWHALE_QUADTREE_H_

#include "WhaleMath.h"

namespace Timewhale {

	template<class T>
	class QuadTreeWrapper {
		T 
	};

	class QuadTree {
	private:
		enum {
			ParentQuad =	   -1,
			ULQuad =			0,
			URQuad =			1,
			DLQuad =			2,
			DRQuad =			3
		};
		static const size_t MaxObjects = 5;
		static const size_t MaxLevels = 5;

		size_t mLevel;
		size_t mQuad;
		Rectangle mBounds;

	public:
		QuadTree(const size_t level, const size_t quadrant, const Rectangle& bounds)
			:mLevel(level),
			mQuad(quadrant),
			mBounds(bounds)
		{}
	};

}


#endif