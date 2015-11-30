/*
 * randwalk.cpp
 *
 *  Created on: Dec 28, 2014
 *      Author: elbert
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "vector.hpp"

int main(int argc, char ** argv)
{
	using namespace std;
	using VECTOR::Vector;
	srand(time(0));
	double direction;
	Vector step;
	Vector result(0.0,0.0);
	unsigned long stepCount = 0;
	double target;
	double dstep;
	cout << "Enter target distance (q to quit)";

	while(cin >> target)
	{
		cout << "Enter step length: ";
		if (! (cin >> dstep))
		{
			break;
		}

		while(result.magval() < target)
		{
			direction = rand() % 360;
			step.reset(dstep, direction, Vector::POL);
			result = result + step;
			stepCount ++;
		}

		cout << "After " << stepCount << " steps, the subject has the following location";
		cout << result << endl;
		result.polar_mode();
		cout << " or " << endl << result << endl;
		cout << "Average outward distance per step = ";
		cout << result.magval() / stepCount << endl;
		stepCount = 0;
		result.reset(0.0,0.0);
		cout << "Enter target distance (q to quit)";
	}

	cout << "Bye!!" << endl;
	cin.clear();
	while (cin.get() != '\n')
	{
		continue;
	}

	return 0;
}



