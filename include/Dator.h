/* 
 * Dator class
 * This is the implementation of the dator class.
 * The Interface is necessary so many dators can be held in a single list.
 * See IDator.h for more on dators.
 *
 * The code for this class is all copy pasta from
 * http://www.gamedev.net/page/resources/_/technical/game-programming/enginuity-part-iii-r1959
 * By Richard Fine

 */

#pragma once
#ifndef __DATOR
#define __DATOR

#include "IDator.h"
#include <sstream>
#include <memory>

namespace Timewhale {

	template<class T>
	class Dator : public IDator {

		T& mTarget;		//This is the data of the dator.
		
		/* toVal
		 * This method converts the string to the type of the dator.
		 *\param	string		The value in string form.
		 *\return	T			The value in data form.		*/
		T toVal(std::string &input) {
			std::stringstream str;
			str.unsetf(std::ios::skipws);

			str << input;
			T result;
			str >> result;

			return result;
		}

		/* toString
		 * This method converts the value to a string.
		 *\param	T			The value in data form.
		 *\return	string		The value in string form.	*/
		std::string toString(T &val) {
			std::stringstream str;
			str.unsetf(std::ios::skipws);

			str << val;
			std::string result;
			str >> result;

			return result;
		}

	public:
		/* These operators facilitate modifying the data
		 * using strings. Inherited from IDator.*/
		IDator &operator=  (std::string &str) { mTarget = toVal(str); return *this; }
		//IDator &operator+= (std::string &str) { mTarget += toVal(str); return *this; }
		//IDator &operator-= (std::string &str) { mTarget -= toVal(str); return *this; }
		bool    operator== (std::string &str) { return (str == toString(mTarget)); }
		bool    operator!= (std::string &str) { return (str != toString(mTarget)); }
		
		/* operator string
		 * This operator converts the dator to a string. 
		 * Inherited from IDator. */
		operator std::string() { return toString(mTarget); }

		/* multiValues 
		 * This dator does not have multiple values so it always
		 * returns false. Inherited from IDator.
		 *\return	bool		Always false. */
		bool multiValues() { return false; }

		/* Constructor */
		Dator(T& t) : mTarget(t) {}
	};
}

#endif
