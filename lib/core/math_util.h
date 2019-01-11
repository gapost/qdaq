#ifndef _math_util_h_
#define _math_util_h_

// #include <cmath>

#include <QVector>
#include <QExplicitlySharedDataPointer>


namespace math {

/** A circular LIFO buffer class.

  \ingroup QDaqCore

  When the buffer becomes full, newer elements overwrite older ones.

  The allocated memory for the buffer is always 2^N for maximum efficiency.

  Data elements are inserted with push() or with the insertion operator<<().

  The stored elements can be recalled by the c-style array operator[].
  The elements are stored in LIFO order: at index 0 is
  the last inserted element, at 1 the one before last, etc.

  The class does not keep count of the number of elements that have been inserted.
  The user is responsible for the neccesary book-keeping.

  */
template<class T>
class circular_buffer
{
public:
    typedef circular_buffer<T> self_t;

private:
    // buffer capacity
    unsigned int cap_;
    // index of the first element
    unsigned int  head_;
    // index bit mask
    unsigned int mask_;
    // memory buffer
    T* buff_;

    void inc(unsigned int& i)
    {
        i++;
        i &= mask_;
    }
    void dec(unsigned int& i)
    {
        i--;
        i &= mask_;
    }
    T* at(int i) const
    {
        return buff_ + ((head_ + i) & mask_);
    }
public:
    /// Construct a circular_buffer with capacity nmax.
    explicit circular_buffer(unsigned int nmax = 1) : buff_(0)
    {
        if (nmax < 1) nmax = 1;
        alloc(nmax);
    }
    ~circular_buffer()
    {
        if (buff_) delete [] buff_;
    }
    /// allocate memory for nmax elements (nmax is adjusted to 2^N).
    /* Previously stored elements are lost! */
    void alloc(unsigned int nmax)
    {
        // find a number 2^N so that 2^N>nmax
        cap_ = 1;
        while (cap_ < nmax) cap_ <<= 1;
        // re-allocate memory
        if (buff_) delete [] buff_;
        buff_ = new T[cap_];
        mask_ = cap_ - 1;
        head_ = 0;
    }
    /// insert an element.
    void push(const T& v)
    {
        dec(head_);
        buff_[head_] = v;
    }
    /// insertion operator is the same as push()
    self_t& operator<< (const T& v)
    {
        push(v); return (*this);
    }
    /// remove the last inserted element
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


template<class T>
class buffer : public QSharedData
{
public:
    typedef QVector<T> container_t;

private:
    typedef buffer<T> _Self;

    /// memory buffer
    container_t mem;
    /// vector size
    int sz;
    /// vector capacity
    int cp;
    /// true for a circular buffer
    bool circular_;
    /// pointer to next position for circular vectors
    int tail;
    /// min & max values
    T x1, x2;
    /// flag set if min & max need recalc
    bool recalcBounds;


    // make the buffer continous in memory and starting at mem[0]
    void normalize_()
    {
        if (circular_ && sz && sz!=tail)
        {
            T* head = mem.data(); // pointer to buffer start address
            T* temp = head + cp; // pointer to start of temp area
            int ntemp = mem.size() - cp; // capacity of the temp area
            if (sz==cp) {
                int m = sz-tail;
                if (tail<=ntemp)
                {
                    memcpy(temp,head,tail*sizeof(T));
                    memmove(head,head+tail,m*sizeof(T));
                    memcpy(head + m,temp,tail*sizeof(T));
                }
                else
                {
                    memcpy(temp,head+tail,m*sizeof(T));
                    memmove(head+m,head,tail*sizeof(T));
                    memcpy(head,temp,m*sizeof(T));
                }
                tail = 0;
            } else if (tail > sz) { // contigous buffer
                memmove(head, head + tail - sz, sz*sizeof(T));
                tail = sz;
            } else if (tail == 0) { // contigous buffer
                memmove(head, head + cp - sz, sz*sizeof(T));
                tail = sz;
            } else { // split buffer
                int nfront = tail;
                int nback = sz - tail;
                if (nfront <= ntemp) {
                    memcpy(temp,head,nfront*sizeof(T)); // copy the front to the temp area
                    memmove(head,head + (cp - nback),nback*sizeof(T)); // move the back to the start
                    memcpy(head + nback,temp,nfront*sizeof(T)); // copy from temp to the end
                } else {
                    memcpy(temp, head + (cp - nback),nback*sizeof(T)); // copy the back to the temp area
                    memmove(head+nback,head,nfront*sizeof(T)); // move the front to the back
                    memcpy(head,temp,nback*sizeof(T));  // copy from temp to the start
                }
                tail = sz;
            }
        }
    }

    // index takes care of circular buffers
    int idx_(int i) const
    {
        return circular_ ? (tail - sz + i + cp) % cp : i;
    }
    // store a value
    void set_(int i, const T& v)
    {
        mem[i] = v;
    }
    // calculated min/max
    void calcBounds_()
    {
        int n(size());
        if (n>0)
        {
            x1 = x2 = get(0);
            for(int i=1; i<n; ++i)
            {
                double v = get(i);
                if (v<x1) x1 = v;
                if (v>x2) x2 = v;
            }
        }
        else x1 = x2 = 0.;
        recalcBounds = false;
    }

public:
    explicit buffer(int acap = 0) : mem((int)acap),
        sz(0), cp(acap), circular_(false), tail(0),
        x1(0), x2(0), recalcBounds(true)
    {
    }
    buffer(const _Self& rhs) : QSharedData(rhs), mem(rhs.mem),
        sz(rhs.sz), cp(rhs.cp), circular_(rhs.circular_), tail(rhs.tail),
        x1(rhs.x1), x2(rhs.x2), recalcBounds(rhs.recalcBounds)
    {
    }
    ~buffer(void)
    {
    }

    _Self& operator=(const _Self& rhs)
    {
        mem = rhs.mem;
        sz = rhs.sz;
        cp = rhs.cp;
        circular_ = rhs.circular_;
        tail = rhs.tail;
        x1 = rhs.x1;
        x2 = rhs.x2;
        recalcBounds = rhs.recalcBounds;
        return (*this);
    }

    bool operator == (const _Self& rhs) const
    {
        if (sz != rhs.sz) return false;

        if (mem.constData() == rhs.mem.constData()) return true;

        for(int i=0; i<size(); i++)
            if ((*this)[i]!=rhs[i]) return false;

        return true;
    }
    bool operator != (const _Self& rhs) const
    {
        return !((*this)==rhs);
    }

    int size() const { return (int)sz; }

    bool isCircular() const { return circular_; }
    void setCircular(bool on)
    {
        if (on==circular_) return;
        normalize_();
        mem.resize(on ? cp + cp/2 : cp); // extra 0.5 size needed to swap mem during normalize_()
        circular_ = on;
    }

    int capacity() const { return (int)cp; }

    void setCapacity(int c)
    {
        if (c==cp) return;

        normalize_();

        if (circular_) {
            mem.resize(c + c/2);
            if (c>cp)
            {
                if (sz==cp) tail = sz;
            }
            else // (c<cp)
            {
                if (sz>c)
                {
                    sz = c; tail = 0;
                    recalcBounds = true;
                }
            }
        } else {
            mem.resize(c);
            if (sz>c) sz = c;
            recalcBounds = true;
        }
        cp = c;
    }
    void setSize(int n)
    {
        if (n==sz) return;
        if (n>cp) setCapacity(n);
        else normalize_();
        if (n>sz) ::memset(mem.data()+sz,0,n*sizeof(T));
        sz = n;
        tail = sz % cp;
    }

    void clear()
    {
        sz = 0;
        tail = 0;
        recalcBounds = true;
    }


    T& operator[](int i)
    {
        return mem[idx_(i)];
    }
    const T& operator[](int i) const
    {
        return get(i);
    }
    const T& get(int i) const
    {
        return mem[idx_(i)];
    }
    void push(const T& v)
    {
        if (circular_) {
            set_(tail++,v);
            if (sz<cp) sz++;
            tail %= cp;
        } else {
            if (sz==cp) mem.resize(++cp);
            set_(sz++,v);
        }
        recalcBounds = true;
    }
    void push(const T* v, int n)
    {
        if (n<1) return;

        if (circular_) {
            if (n>=cp) {
                memcpy(mem.data(),v+n-cp,cp*sizeof(T));
                tail = 0;
                sz = cp;
            }
            else if (n<=cp-tail) {
                memcpy(mem.data()+tail,v,n*sizeof(T));
                tail += n;
                if (sz<cp) sz += n;
                if (sz==cp) tail %= cp;
            } else {
                int m = cp-tail;
                memcpy(mem.data()+tail,v,m*sizeof(T));
                v += m; n -= m;
                memcpy(mem.data(),v,n*sizeof(T));
                tail = n;
                sz = cp;
            }
        } else {
            if (sz+n>cp) { cp = sz+n; mem.resize(cp); }
            memcpy(mem.data()+sz,v,n*sizeof(T));
            sz += n;
        }
        recalcBounds = true;
    }
    void pop()
    {
        if (sz==0) return;
        sz--;
        if (circular_) tail = (tail - 1) % sz;
        recalcBounds = true;
    }
    const T* constData() const
    {
        const_cast< _Self * >( this )->normalize_();
        return mem.constData();
    }
    T* data()
    {
        normalize_();
        return mem.data();
    }
    container_t vector() const
    {
        const_cast< _Self * >( this )->normalize_();
        return mem.mid(0,sz);
    }
    double vmin() const
    {
        if (recalcBounds)
            const_cast< _Self * >( this )->calcBounds_();
        return x1;
    }
    double vmax() const
    {
        if (recalcBounds)
            const_cast< _Self * >( this )->calcBounds_();
        return x2;
    }
    double mean() const
    {
        double s(0.0);
        int n(size());
        for(int i=0; i<n; ++i) s += get(i);
        return s/n;
    }
    double std() const
    {
        double s1(0.0), s2(0.0);
        int n(size());
        for(int i=0; i<n; ++i) {
            double v = get(i);
            s1 += v; s2 += v*v;
        }
        s1 /= n;
        s2 /= n;
        s1 = s2 - s1*s1;
        if (s1<=0.0) return 0.0;
        else return sqrt(s1);
    }
};

} // namespace math
#endif

