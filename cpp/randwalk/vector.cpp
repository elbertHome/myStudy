/*
 * vector.cpp
 *
 *  Created on: Dec 28, 2014
 *      Author: elbert
 */
#include <cmath>
#include "vector.hpp"

namespace VECTOR
{

	const double Rad_to_deg = 45.0 / std::atan(1.0);

	Vector::Vector()
	{
		x = 0.0;
		y = 0.0;
		mag = 0.0;
		ang = 0.0;
		mode = RECT;
	}

	Vector::Vector(double n1, double n2, Mode form)
	{
		mode = form;
		if (form == RECT)
		{
			x = n1;
			y = n2;
			set_mag();
			set_ang();
		}
		else if (form == POL)
		{
			mag = n1;
			ang = n2;
			set_x();
			set_y();
		}
		else
		{
			std::cout << "Incorrect 3rd argument to Vector() --";
			std::cout << "Vector set to 0" << std::endl;
			x = y = mag = ang = 0.0;
			mode = RECT;
		}
	}

	Vector::~Vector()
	{
	}

	void Vector::set_mag()
	{
		mag = std::sqrt(x * x + y * y);
	}

	void Vector::set_ang()
	{
		if (x == 0.0 && y == 0.0)
		{
			ang = 0.0;
		}
		else
		{
			ang = std::atan2(y, x);
		}
	}

	void Vector::set_x()
	{
		x = mag * std::cos(ang);
	}

	void Vector::set_y()
	{
		y = mag * std::sin(ang);
	}

	void Vector::reset(double n1, double n2, Mode form)
	{
		mode = form;
		if (form == RECT)
		{
			x = n1;
			y = n2;
			set_mag();
			set_ang();
		}
		else if (form == POL)
		{
			mag = n1;
			ang = n2;
			set_x();
			set_y();
		}
		else
		{
			std::cout << "Incorrect 3rd argument to Vector() --";
			std::cout << "Vector set to 0" << std::endl;
			x = y = mag = ang = 0.0;
			mode = RECT;
		}
	}

	void Vector::polar_mode()
	{
		mode = POL;
	}

	void Vector::rect_mode()
	{
		mode = RECT;
	}

	Vector Vector::operator +(const Vector& b) const
	{
		return Vector(x + b.x, y + b.y);
	}

	Vector Vector::operator -(const Vector& b) const
	{
		return Vector(x - b.x, y - b.y);
	}

	Vector Vector::operator -() const
	{
		return Vector(-x, -y);
	}

	Vector Vector::operator *(double n) const
	{
		return Vector(x * n, y * n);
	}

	// Friend Functions
	Vector operator *(double n, const Vector &a)
	{
		return a * n;
	}

	std::ostream & operator <<(std::ostream &os, const Vector &v)
	{
		if (v.mode == Vector::RECT)
		{
			os << "(x,y) = (" << v.x << "," << v.y << ")";
		}
		else if (v.mode == Vector::POL)
		{
			os << "(m,a) = (" << v.mag << "," << v.ang * Rad_to_deg << ")";
		}
		else
		{
			os << "Vector object mode is invalid";
		}
		return os;
	}

}
