#ifndef _AUTOTUNER_H_
#define _AUTOTUNER_H_

#include <math.h>

#define PI 3.1415926535897932384626433832795

template<class T>
class autotuner
{
	// tuner parameters
	T step, // step in output
	  dy; // threshold value to detect change
	int count;

	// state
	bool atp; // previous automode
	bool up; // signal state
	int steps;
	int counter;
	int t0;
	T ymax, ymin;
	T ff_;
	T cv0;

	// result
	T tc,kc;


public:

    autotuner() : step(0.2), dy(0.1), count(2), atp(false), ff_(0.995), cv0(0), tc(0), kc(0)
	{
	}

	~autotuner(void)
	{
	}

	bool operator()(T sp, T pv, T& cv, bool& automode)
	{
		bool ret = false;

		if (automode)
		{
			if (!atp) // starting
			{
				cv = cv0 + step;
				steps = 0;
				ymax = ymin = pv;
				up = false;
				tc = 0;
			}
			else
			{
				if (pv-sp > dy && !up) // crossing up!
				{
					up = true;
					cv -= 2*step;
				}
				else if (sp - pv > dy && up) // crossing down!
				{
					up = false;
					cv += 2*step;
					if (steps) tc += counter; // start counting
					counter = 0;
					steps++;
				}

				if (pv>ymax) ymax = pv;
				if (pv<ymin) ymin = pv;

				if (steps==count+1)
				{

					tc = tc/count;
					kc = 4*2*step/PI/(ymax-ymin);

					// ready
					automode = false;
					cv = cv0;
					ret = true;
				}
			}
		}

		cv0 = (1-ff_)*cv + ff_*cv0;
		atp = automode;
		counter++;
		return ret;
	}

	void get_par(T h, T& k, T& ti, T& td) const
	{
		k = 0.6*kc;
		ti = h*tc/2;
		td = h*tc/8;
	}
	void get_par(T h, T& k, T& ti) const
	{
		k = 0.4*kc;
		ti = h*tc/1.2;
	}
	void get_par(T h, T& k) const
	{
		h = h;
		k = 0.5*kc;
	}

	// getters
	T get_tc() const { return tc; }
	T get_kc() const { return kc; }
	T get_step() const { return step; }
	T get_dy() const { return dy; }
	int get_count() const { return count; }

	// setters
	void set_step(T v) { step = v; }
	void set_dy(T v) { dy = v; }
	void set_count(int v) { count = v; }
};

#endif



/*

      % Ziegler & Nichols (open loop)
      % FOPDT model parameters
      m = model.m; L = model.L; T = model.T;
      if L~=0
         if strcmp(regStruct,'PI')
            K = (0.9*T) / (m*L);
            Ti = 3*L;
            Td = 0;
         elseif strcmp(regStruct,'PID')
            K = (1.2*T) / (m*L);
            Ti = 2*L;
            Td = 0.5*L;
         end
      b  = 1;
      N  = 5;
      end
*/
