/*
 * vector.hpp
 *
 *  Created on: Dec 28, 2014
 *      Author: elbert
 */

#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <iostream>

namespace VECTOR
{
	class Vector
	{
		public:
			// The mode of the coordinate
			enum Mode
			{
				RECT, POL
			};
		private:
			double x;
			double y;
			double mag;
			double ang;
			Mode mode;
			// private functions
			void set_mag();
			void set_ang();
			void set_x();
			void set_y();
		public:
			// Structers & Distructer
			Vector();
			Vector(double n1, double n2, Mode form = RECT);
			~Vector();
			// member functions
			void reset(double n1, double n2, Mode form = RECT);
			double xval() const
			{
				return x;
			}
			double yval() const
			{
				return y;
			}
			double magval() const
			{
				return mag;
			}
			double angval() const
			{
				return ang;
			}
			void polar_mode();
			void rect_mode();
			// operator overloading
			Vector operator +(const Vector & b) const;
			Vector operator -(const Vector & b) const;
			Vector operator -() const;
			Vector operator *(double n) const;
			// friend fucntions
			friend Vector operator *(double n, const Vector &a);
			friend std::ostream & operator <<(std::ostream & os, const Vector &v);
	};

}

#endif /* VECTOR_HPP_ */

