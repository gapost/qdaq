#ifndef _LINEFIT_H_
#define _LINEFIT_H_

#include <cmath>

template<class T>
struct linefit
{
    T a, b;

	template<class V>
    linefit(const V& x, const V& y, int ndata)
	/*
	Fit to y = a + bx
	*/
	{
		int i;
		T ss,sx(0),sy(0),st2(0),t,sxoss;
		b=T(0); //Accumulate sums ...
        for (i=0; i<ndata; i++) {
			sx += x[i]; //...without weights.
			sy += y[i];
		}
		ss=ndata;
		sxoss=sx/ss;
		for (i=0;i<ndata;i++) {
			t=x[i]-sxoss;
			st2 += t*t;
			b += t*y[i];
		}
        b /= st2; //Solve for a, b
		a=(sy-sx*b)/ss;
	}
};

#endif
