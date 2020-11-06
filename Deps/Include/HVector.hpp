#pragma once


template <class Type>
union tagTMatrix4 {
	Type mat[4][4];
	Type _mat[16];

	void Init()
	{
		memset(mat, 0, sizeof(Type) * 16);
		mat[0][0] = Type(1.f);	mat[1][1] = Type(1.f);	mat[2][2] = Type(1.f);	mat[3][3] = Type(1.f);
	}
};

/*============================================================================*/
template<class Type, int iDim>
class HVector
{
public:
    Type vec[iDim];
	HVector<Type, iDim>(const Type &val1,const Type &val2, const Type &val3 = 0, const Type &val4 =0)
	{
		for (int i=0; i<iDim; i++)
		{
			vec[i] = 0;
		}
		if(iDim > 0)
			vec[0] = val1;
		if(iDim > 1)
			vec[1] = val2;
		if(iDim > 2)
			vec[2] = val3;
		if(iDim > 3)
			vec[3] = val4;
	}
    HVector<Type, iDim>(const Type& vi=0)
    {
        for (int i = 0; i<iDim; i++)
        {
            vec[i] = vi;
        }
    }

		    HVector<Type, iDim>(const HVector<Type, iDim>& src)
    {
        *this = src;
    }

    HVector<Type, iDim>(const HVector<Type, iDim>&& src)
    {
            *this = std::move(src);
    }

    void Init(const Type &v=0)
    {
        for (int i=0; i<iDim; ++i)
            vec[i] = v;
    }

    template<class Type1>
    HVector<Type, iDim>(const HVector<Type1, iDim>& src)
    {
        *this = src;
    }

    HVector<Type, iDim>& operator = (const HVector<Type, iDim> &src)
    {
        for (int i = 0; i<iDim; i++)
        {
            vec[i] = src.vec[i];
        }
        return *this;
    }
    HVector<Type, iDim>& operator = (HVector<Type, iDim>&& src)
    {
        if (this != &src)
        {
            std::copy(src.vec, src.vec + iDim, vec);
        }

        return *this;
    }

    template<class Type1>
    HVector<Type, iDim>& operator = (const HVector<Type1, iDim>& src)
    {
        for (int i = 0; i<iDim; i++)
        {
            vec[i] = (Type)src.vec[i];
        }
        return *this;
    }

    HVector<Type, iDim>& operator += (const HVector<Type, iDim> &src)
    {
        for (int i = 0; i<iDim ; i++)
        {
            vec[i] += src.vec[i];
        }
        return *this;
    }

    HVector<Type, iDim>& operator -= (const HVector<Type, iDim> &src)
    {
        for (int i = 0; i<iDim; i++)
        {
            vec[i] -= src.vec[i];
        }
        return *this;
    }

    HVector<Type, iDim> operator + (const HVector<Type, iDim> &src) const
    {
        HVector<Type, iDim> tmp = *this;
        tmp += src;
        return tmp;
    }

    HVector<Type, iDim> operator - (const HVector<Type, iDim> &src) const
    {
        HVector<Type, iDim> tmp = *this;
        tmp -= src;
        return tmp;
    }

    HVector<Type, iDim> operator - () const
    {
        HVector<Type, iDim> tmp = *this;

        for (int i = 0; i<iDim ; i++)
        {
            tmp.vec[i]= -tmp.vec[i];
        }
        return tmp;
    }

    HVector<Type,iDim> operator * (const Type &src) const
    {
        HVector<Type, iDim> tmp = *this;

        for (int i = 0; i<iDim; i++)
        {
            tmp.vec[i] *= src;
        }

        return tmp;
    }

	HVector<Type,iDim> operator - (const Type &src) const
	{
		HVector<Type, iDim> tmp = *this;

		for (int i = 0; i<iDim; i++)
		{
			tmp.vec[i] -= src;
		}

		return tmp;
	}
	HVector<Type,iDim> operator + (const Type &src) const
	{
		HVector<Type, iDim> tmp = *this;

		for (int i = 0; i<iDim; i++)
		{
			tmp.vec[i] += src;
		}

		return tmp;
	}
    HVector<Type,iDim> operator / (const Type &src) const
    {
        HVector<Type, iDim> tmp = *this;

        if (src == 0)
        {
            ASSERT(NULL);
            return tmp;
        }

        for (int i = 0; i<iDim; i++)
        {
            tmp.vec[i] /= src;
        }

        return tmp;
    }

    HVector<Type,iDim>& operator *= (const Type src)
    {
        for (int i = 0; i<iDim ; i++)
        {
            vec[i] *= src;
        }
        return *this;
    }

    HVector<Type,iDim>& operator /= (const Type src)
    {
        for (int i = 0; i<iDim ; i++)
        {
            vec[i] /= src;
        }
        return *this;
    }

    HVector<Type, iDim>& operator += (const Type src)
    {
        for (int i = 0; i<iDim; i++)
        {
            vec[i] += src;
        }
        return *this;
    }

    HVector<Type, iDim>& operator -= (const Type src)
    {
        for (int i = 0; i<iDim; i++)
        {
            vec[i] -= src;
        }
        return *this;
    }

    bool operator<(const HVector<Type, iDim> &src) const
    {
        return Distance()<src.Distance();
    }

    template<class Type1>
    bool operator == (const HVector<Type1, iDim> &src) const
    {
        long lMatch = 0;
        for (int i = 0; i<iDim ; i++)
        {
            lMatch += (long)(fabs(vec[i] - Type(src.vec[i]))<WZ_EPSILON);
        }

        return (bool)(lMatch == iDim);
    }

    template<class Type1>
    bool operator != (const HVector<Type1, iDim> &src) const
    {
        long lMatch = 0;
        for (int i = 0; i<iDim ; i++)
        {
            lMatch += (long)(fabs(vec[i] - Type(src.vec[i]))<WZ_EPSILON);
        }

        return (bool)(lMatch != iDim);
    }

    Type Distance() const
    {
        Type dist = 0;
        for (int i = 0; i<iDim ; i++)
        {
            dist += (vec[i] * vec[i]);
        }
        return sqrt(dist);
    }

	Type Length(const HVector<Type, iDim> &src) const
	{
		return (*this - src).Distance();
	}

	HVector<Type, iDim> GetByDist(const HVector<Type, iDim> &dirPt, Type dDist) const
	{
		double dLength = (dirPt - *this).Distance();
		if(dLength < 1e-9)
			return *this;
		HVector<Type, iDim> newPt = *this + (dirPt-*this)*(dDist/dLength);
		return newPt;
	}
	HVector<Type, iDim> GetByRatio(const HVector<Type, iDim> &dirPt, Type dRatio) const
	{
		double dDist = dRatio * Distance();
		return GetByDist(dirPt, dDist);
	}
    HVector<Type, iDim> & Normalize()
    {
        Type dist = Distance();
        if (fabs(dist)<1.0e-12)
		{
			*this;
		}

        for (int i = 0 ; i<iDim ; i++)
        {
            vec[i] /= dist;
        }

        return *this;
    }
	HVector<Type,3> Cross(const HVector<Type,3> &dir) const 
	{
		HVector<Type,3> Temp;

		Temp.vec[0] = Type(vec[1]*dir.vec[2]-vec[2]*dir.vec[1]);
		Temp.vec[1] = Type(vec[2]*dir.vec[0]-vec[0]*dir.vec[2]);
		Temp.vec[2] = Type(vec[0]*dir.vec[1]-vec[1]*dir.vec[0]);

		return Temp;
	} 
    Type Dot(const HVector<Type,iDim> &src) const
    {
        Type dotProd = 0;
        for (int i = 0; i<iDim; i++)
        {
            dotProd += vec[i] * src.vec[i];
        }

        return dotProd;
    }

    void Inverse()
    {
        for (int i = 0; i<iDim ; ++i)
        {
            this->vec[i] = -this->vec[i];
        }
    }
};

typedef HVector<int, 2>     HVector2i;
typedef HVector<long, 2>    HVector2l;
typedef HVector<float, 2>   HVector2f;
typedef HVector<double, 2>  HVector2d;
typedef HVector<int, 3>     HVector3i;
typedef HVector<long, 3>    HVector3l;
typedef HVector<float, 3>   HVector3f;
typedef HVector<double, 3>  HVector3d;
typedef HVector<int, 4>     HVector4i;
typedef HVector<long, 4>    HVector4l;
typedef HVector<float, 4>   HVector4f;
typedef HVector<double, 4>  HVector4d;


/*============================================================================*/
template<class Type, int iDim>
class HVectorPosKey : public HVector<Type, iDim>
{
    HVectorPosKey<Type, iDim>() : HVector<Type, iDim>() {    }
    virtual ~HVectorPosKey<Type, iDim>() { }

    HVectorPosKey<Type, iDim>(const HVector<Type, iDim>& src) { memcpy(vec, src.vec, sizeof(Type) * iDim); }

    bool operator<(const HVector<Type, iDim> &src) const
    {
        if (Distance() == src.Distance())
        {
            for (long idx = 0; idx<(iDim - 1); ++idx)
            {
                if (vec[idx] != src.vec[idx])
                    return (vec[idx]<(src.vec[idx] - WZ_EPSILON));
            }

            return (vec[iDim - 1]<(src.vec[iDim - 1] - WZ_EPSILON));
        }
        else
        {
            return Distance()<src.Distance();
        }
    }
};

typedef HVectorPosKey<double, 3> HVector3PosKey;

