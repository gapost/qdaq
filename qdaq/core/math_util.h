#ifndef _math_util_h_
#define _math_util_h_

#include <cmath>

namespace math {


template<class T>
class circular_buffer
{
	/// buffer capacity
	unsigned int cap_;
	/// index of the first element
	unsigned int  head_;
	/// index bit mask
	unsigned int mask_;
	/// memory buffer
	T* buff_;

	void inc(unsigned int& i)
	{
        i++;
        i = i & mask_;
	}
	void dec(unsigned int& i)
	{
        i--;
        i = i & mask_;
	}
	T* at(int i) const
	{
		return buff_ + ((head_ + i) & mask_);
	}
public:
	explicit circular_buffer(unsigned int nmax = 1) : buff_(0)
	{
		alloc(nmax);
	}
	~circular_buffer()
	{
		if (buff_) delete [] buff_;
	}
	void alloc(unsigned int nmax)
	{
		cap_ = 1;
		while (cap_ < nmax) cap_ <<= 1;
		//assert(cap_>1);
		if (buff_) delete [] buff_;
		buff_ = new T[cap_];
		mask_ = cap_ - 1;
		head_ = 0; 
	}
	/// insert an element at the front
	void push(const T& v)
	{
		dec(head_);
		buff_[head_] = v;
	}
	/// remove an element from the front
	void pop()
	{
		inc(head_);
	}
	/// ref to the i-th element
	T& operator[](int i)
	{
		return *(at(i));
	}
	/// const ref to the i-th element
	const T& operator[](int i) const
	{
		return *(at(i));
	}
	/// const ref to last element inserted
	const T& last() const
	{
		return buff_[head_];
	}
	/// return the buffer's capacity
	unsigned int capacity() const
	{
		return cap_; 
	}
};

template<class T>
class averager
{
	circular_buffer<T> buff_;
	unsigned int n; // depth of filter
	unsigned int k; // actuall number of values in buffer
	int sign; // sign of delta
	T ff_; // forgetting factor
	T ffw_; // normalization in case of ff
	T ffmean_; // mean value in case of ff
public:
	enum type {avNone, avRunning, avDelta, avForgetting};

public:
	explicit averager(unsigned int maxsize) : buff_(maxsize), n(1), k(0), sign(-1), ff_(0.95), ffw_(1), ffmean_(0)
	{
	}

	averager<T>& operator<<(const T& v) 
	{ 
		buff_.push(v); 
		if (k<capacity()) k++;
		sign = -sign;
		//ffmean_ = k==1 ? v : (1-ff_)*v + ff_*ffmean_;
		return *this;
	}

	void clear() { k=0; sign = -1; } 

	unsigned int capacity() const { return buff_.capacity(); }
	const circular_buffer<T>& buff() const { return buff_; }

	unsigned int N() const { return n; }
	void setN(unsigned int an)
	{
		if (an==0) an=1;
		if (an>capacity()) 
		{
			buff_.alloc(an);
			k=0;
		}
		n = an;
		setff(ff_);
	}
	T ff() const { return ff_; }
	void setff(T v) 
	{ 
		ff_ = v; 
		ffw_ = T(1)/(T(1) - pow(ff_,T(n)));
	}

	T mean(type t = avRunning)
	{
		int m = k<n ? k : n;
		if (m==1 || t==avNone) return buff_[0];
		if (t==avForgetting) return ffmean_;

		T s(0);

		if (t==avDelta)
		{
			T sm(0);
			bool iseven = m & 0x1;
			if (iseven)
			{
				int l = (m-1)/2, i=0;
				for(; i<2*l; i+=2) 
				{ 
					s += buff_[i]; 
					sm += buff_[i+1];
				}
				//i += 2;
				s += buff_[i];
				s = (s - (l+1)*sm/l)/(m+1);
			}
			else
			{
				for(int i=0; i<m; i+=2) 
				{ 
					s += buff_[i]; 
					sm += buff_[i+1];
				} 
				s = (s-sm)/m;
			}
			s = s*sign;
		}
		else 
		{
			for(int i=0; i<m; ++i) s += buff_[i];
			s /= m;
		}

		return s;
	}
	const T& last() const { return buff_.last(); }

	bool mean(type t, T& v, T& dv)
	{
		int m = k<n ? k : n;
		v = dv = 0;

		if (m==0) return false;

		if (t==avNone) 
		{
			v = buff_[0];
			return true;
		}

		if (m==1) return false;
		

		T wt;
		int sw(sign);

		switch(t)
		{
		case avRunning:
			for(int i=0; i<m; ++i)
			{
				T y = buff_[i];
				v += y;
				dv += y*y;
			}
			v /= m;
			dv /= m;
			break;
		case avDelta:
			for(int i=1; i<m; ++i)
			{
				T y = (buff_[i]-buff_[i-1]);
				v += y*sw;
				dv += y*y;
				sw = - sw;
			}
			v  /= (2*(m-1));
			dv /= (4*(m-1));
			break;
		case avForgetting:
			wt = 1-ff_;
			for(int i=0; i<m; ++i)
			{
				T y = buff_[i];
				v += y*wt;
				dv += y*y*wt;
				wt *= ff_;
			}
			v *= ffw_;
			dv *= ffw_;
			break;
        case avNone:
            break;
		}

		dv -= v*v;
		if (dv>0) dv=sqrt(dv);
		else dv = 0;

		return true;
	}

};



template<class T, int N>
class running_average
{
	T data[N];
	T* p,*pend;
public:
	explicit running_average(const T& v = 0)
	{
		p = &data[0];
		pend = p + N;
		while (p!=pend) *p++ = v;
		p = &data[0];
	}
	running_average& operator<<(const T& v)
	{
		*p++ = v;
		if (p==pend) p = &data[0];
		return *this;
	}
	T operator()()
	{
		T m = 0;
		for(int i = 0; i<N; ++i) m += data[i];
		return m/N;
	}
	void clear()
	{
		for(int i=0; i<N; ++i) data[i]=0;
	}
};

template<class T>
class vector_ref
{
	const T* p;
	int sz;

public:
	vector_ref() : p(0), sz(0)
	{}
	vector_ref(const T* ap, int asz) : p(ap), sz(asz)
	{}
	vector_ref(const vector_ref& other) : p(other.p), sz(other.sz)
	{}

	vector_ref& operator=(const vector_ref& rhs)
	{
		p = rhs.p;
		sz = rhs.sz;
	}

	int size() const { return sz; }
	//const T* data() const { return p; }
	bool isNull() const { return p==0; }

	const T& operator[](int i) const { return p[i]; }
	//T& operator[](int i) { return p[i]; }
};

template<class T>
class matrix_ref
{
	const T* p;
	int n,m;

public:
	matrix_ref() : p(0), n(0), m(0)
	{}
	matrix_ref(const T* ap, int an, int am) : p(ap), n(an), m(am)
	{}
	matrix_ref(const matrix_ref& other) : p(other.p), n(other.n), m(other.m)
	{}

	matrix_ref& operator=(const matrix_ref& rhs)
	{
		p = rhs.p;
		n = rhs.n;
		m = rhs.m;
	}

	int rows() const { return n; }
	int cols() const { return m; }
	//const T* data() const { return p; }
	bool isNull() const { return p==0; }

	const T* operator[](int i) const { return p + i*m; }
	//T& operator[](int i) { return p[i]; }
};

} // namespace math
#endif

