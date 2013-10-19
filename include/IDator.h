/* Dator Interface
 * A dator is similar to a functor except you are wrapping a piece
 * of data in an object. The point is that when you change the object
 * it changes the data, but furthermore you can also set the data
 * using a string! Its good for loading in settings off a file and
 * scripting.
 *
 * The code for this class is all copy pasta from
 * http://www.gamedev.net/page/resources/_/technical/game-programming/enginuity-part-iii-r1959
 * By Richard Fine
*/

#pragma once
#ifndef __IDATOR
#define __IDATOR

#include <string>
#include <memory>

namespace Timewhale {

	class IDator {
	protected:
		/* Constructors */
		IDator() {}
		IDator(const IDator &that) {*this = that; }
	public:
		/* These operators facilitate modifying the data
		 * using strings. */
		virtual IDator &operator=  (std::string &) = 0;
		//virtual IDator &operator+= (std::string &) = 0;
		//virtual IDator &operator-= (std::string &) = 0;
		virtual bool    operator== (std::string &) = 0;
		virtual bool    operator!= (std::string &) = 0;

		/* multiValues 
		 *If the dator as more than one value such as a list
		 * or pair then this returns true.
		 *\return bool		True if has multiple values. */
		virtual bool multiValues() = 0;

		/* operator string
		 * This operator converts the dator to a string. */
		virtual operator std::string() = 0;
	};
	
	typedef std::shared_ptr<IDator> IDatorPtr;
}

#endif
