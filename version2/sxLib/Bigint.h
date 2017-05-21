/********************************************************************
	created:	2016/5/5
	filename: 	Bigint.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class to store a big integer number
				and perform basic mathematic operation on it.

				The class uses an array of bytes as digits to store big
				number. so every byte in that array can be 0..9 and other
				values are not valid. In this regard the array terminates
				with a invalid value of 10

	*********************************************************************/
#ifndef DEFINED_Bigint
#define DEFINED_Bigint

#include "Def.h"

//! use this class to store a big number and operate on it
template<uint max_digits>
class Bigint
{
public:
	Bigint()
	{
		m_number[0] = 0;
		m_number[1] = 10;
	}

	Bigint(const uint64& v)
	{
		_assign(v);
	}

	uint Length() const
	{
		uint res = 0;
		while (m_number[res] < 10) ++res;
		return res;
	}

	void print()
	{
		uint l = Length();
		for (int i = l - 1; i >= 0; --i)
			printf("%u", m_number[i]);
	}

	byte& operator[](const uint& index)
	{
		return m_number[index];
	}

	uint operator% (uint v) const 
	{
		uint res = 0;
		uint len = Length();
		for (int i = len - 1; i >= 0; --i)
			res = (m_number[i] + res * 10) % v;
		return res;
	}

	//! compute the power and store that in this
	Bigint& power(const uint64& i, const uint64& n)
	{	// original code from: https://discuss.codechef.com/questions/7349/computing-factorials-of-a-huge-number-in-cc-a-tutorial

		m_number[0] = 1;	// initializes array with only 1 digit, the digit 1.
		int	m = 1;			// initializes digit counter
		int k = 1;			// k is a counter that goes from 1 to n.
		uint64 temp = 0;		// initializes carry variable to 0.
		while (k <= n)
		{
			for (int j = 0; j < m; j++)
			{
				uint64 x = m_number[j] * i + temp;	// x contains the digit by digit product
				m_number[j] = x % 10;				// contains the digit to store in position j
				temp = x / 10;				// contains the carry value that will be stored on later indexes
			}
			while (temp > 0)	// while loop that will store the carry value on array.
			{
				m_number[m] = temp % 10;
				temp = temp / 10;
				m++; // increments digit counter
			}
			k++;
		}
		m_number[m] = 10;

		return *this;
	}

private:
	void _assign(uint64 v)
	{
		uint i = 0;
		while (v > 0)
		{
			m_number[i++] = v % 10;
			v /= 10;
		}
		m_number[i] = 10;
	}

public:
	byte	m_number[max_digits];
};

//////////////////////////////////////////////////////////////////////////
//	GLOBAL HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////

SEGAN_LIB_API Bigint<128> sx_power(const uint64& x, const uint64& y);

#endif // DEFINED_Bigint

