#ifndef _ISA_PID_H_
#define _ISA_PID_H_

/*
 * GA, July 2018
 *
 * Removed the "tracking" and "bumpless param change" options
 * They were causing problems in automatic annealing program
 *
 */

template<class T>
class isa_pid
{
public:
	T h; // sampling time

    // loop parameters
	T k, // gain
	  ti, // integration time
	  td, // diff. time
	  tr, // reset time const
	  b; // set-point scaling
	int N; // diff. filter time const (=td/N)

	// output limits
	T umin, umax;

	// state variables
	T ui; // integral action value
	T ud; // derivative action value
	T pvp; // previous process var value
	T cvp; // previous control var value
	bool atp; // previous auto mode

	// internal constants
	T a1,a2,b1,b2;

	T limitOut(T u)
	{
		if (u>umax) return umax;
		else if (u<umin) return umin;
		else return u;
	}

	void update_par()
	{
		a1=a2=b1=b2=0;

		// enable I term
		if (ti != T(0)) a1 = k*h/ti;
		if (tr != T(0)) a2 = h/tr;
		
		// D term. if td=0 b1=b2=0
		b1 = td/(td + N*h);
		b2 = k*N*b1;
	}



    isa_pid(T ats = 1) : h(ats), k(1), ti(0), td(0), tr(0), b(1), N(5), umin(0), umax(1)
	{
		update_par();
        ui = ud = pvp = cvp = 0;
        atp = false;
	}

	~isa_pid(void)
	{
	}

	void operator()(T sp, T pv, T& cv, bool automode = true)
	{
		// PID algorithm
		// compute control actions

		// P term
        T up = k*(b*sp - pv);

		// D term
		ud = b1*ud - b2*(pv-pvp);

        if (automode && !atp) // auto switched on
        { // reset the state
            ui = ud = 0;
		}

		T v = up + ui + ud;
		T u = limitOut(v);
		
        if (automode) cv = u;

        // update integrator
        ui += a1*(sp-pv) + a2*(cv-v);

		// save status
		pvp = pv; cvp = cv; atp = automode;
	}

	// setters
	void set_h(T v)  { h = v; update_par(); }
	void set_k(T v)  { k = v; update_par(); }
	void set_ti(T v) { ti = v; update_par(); }
	void set_td(T v) { td = v; update_par(); }
	void set_tr(T v) { tr = v; update_par(); }
	void set_N(int v) { N = v; update_par(); }
	void set_b(T v)  { b = v; update_par(); }
	void set_umin(T v)  { umin = v; }
	void set_umax(T v)  { umax = v; }

	// getters
	T get_h()  const { return h; }
	T get_k()  const { return k; }
	T get_ti() const { return ti; }
	T get_td() const { return td; }
	T get_tr() const { return tr; }
	int get_N() const { return N; }
	T get_b()  const { return b; }
	T get_umin()  const { return umin; }
	T get_umax()  const { return umax; }

};

#endif




