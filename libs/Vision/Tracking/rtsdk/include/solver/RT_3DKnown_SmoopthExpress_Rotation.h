// RT_3DKnown_SmoopthExpress.h: interface for the RT_3DKnown_SmoopthExpress class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ZAnyScaleFunction.h"
#include "IRANSACModel.h"

//////////////////////////////////////////////////////////////////////////
/// \ingroup Solver
/// \brief Expression of known 3D structure with smoothment.
//////////////////////////////////////////////////////////////////////////
template <class T>
class RT_3DKnown_SmoopthExpress_Rotation  : public ZAnyScaleFunction<T>
{
public:
	RT_3DKnown_SmoopthExpress_Rotation();
	virtual ~RT_3DKnown_SmoopthExpress_Rotation();

	virtual T Value (const Wml::GVector<T>& x);
	
	virtual void Gradient (const Wml::GVector<T>& x, Wml::GVector<T>& grad);
	
	virtual void Hesse (const Wml::GVector<T>& x, Wml::GMatrix<T>& hesse);

	void SetXVar(const Wml::GVector<T>& x);

	void ReadPoint(MatchPoint& pt,T weight=1.0);

	int GetPointNumber(){return m_points.size();}

	int Refine3DPoint(const Wml::GVector<T>& x,
		T f,T threshold);

	int RemoveOutlier(const Wml::GVector<T>& x, T N=3.0);

	void SetSmoothWeight(T rw0);

	T RMSE(const Wml::GVector<T>& x);

	T ProjectionError(const Wml::GVector<T>& x);

	void SetOriginX(const Wml::GVector<T>& x0);

	void ClearData();

protected:
	T alpha,beta,gama;
	T alpha0,beta0,gama0;
	T sinAlpha,cosAlpha,sinBeta,cosBeta,sinGama,cosGama;
	T X,Y,Z;
	T u1,v1,w,rw;
	std::vector<int> m_LocalVariableIndex;
	std::vector<MatchPoint*>	m_points;
	std::vector<T>	m_weights;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T>
RT_3DKnown_SmoopthExpress_Rotation<T>::RT_3DKnown_SmoopthExpress_Rotation()
{
	for(int i=0;i<3;i++)
		m_LocalVariableIndex.push_back(i);

	w = 1.0,		rw = 0.0;
	alpha0 = 0.0,	beta0 = 0.0,	gama0 = 0.0;
}

template <class T>
RT_3DKnown_SmoopthExpress_Rotation<T>::~RT_3DKnown_SmoopthExpress_Rotation()
{

}

template <class T>
void RT_3DKnown_SmoopthExpress_Rotation<T>::SetXVar(const Wml::GVector<T>& x)
{
	alpha = x[0];
	beta =x[1];
	gama = x[2];

	sinAlpha = sin(alpha);
	cosAlpha = cos(alpha);
	sinBeta = sin(beta);
	cosBeta = cos(beta);
	sinGama = sin(gama);
	cosGama = cos(gama);
}

template <class T>
void RT_3DKnown_SmoopthExpress_Rotation<T>::ReadPoint(MatchPoint& p, T weight)
{
	m_points.push_back(&p);
	m_weights.push_back(weight);
}

template <class T>
void RT_3DKnown_SmoopthExpress_Rotation<T>::ClearData()
{
	m_points.clear();
	m_weights.clear();
}

template <class T>
void RT_3DKnown_SmoopthExpress_Rotation<T>::SetSmoothWeight(T rw0)
{
	rw = rw0;
}

template <class T>
void RT_3DKnown_SmoopthExpress_Rotation<T>::SetOriginX(const Wml::GVector<T>& x0)
{
	alpha0 = x0[0];
	beta0 = x0[1];
	gama0 = x0[2];
}

template <class T>
int RT_3DKnown_SmoopthExpress_Rotation<T>::Refine3DPoint(const Wml::GVector<T>& x, T f, T threshold)
{
	int i;
	T sum;
	std::vector<T>	m_valueList;
	std::vector<MatchPoint*>	m_points2;
	int removeCount;
	SetXVar(x);
	
	for(i=0;i<m_points.size();i++){
		u1=m_points[i]->v_2d.X();
		v1=m_points[i]->v_2d.Y();
		X=m_points[i]->v_3d.X();
		Y=m_points[i]->v_3d.Y();
		Z=m_points[i]->v_3d.Z();
		w = m_weights[i];

		sum =pow(((cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1),2)+pow(((sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+
			(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1),2); 
		sum = sqrt(sum)*f;
		m_valueList.push_back(sum);
	}

	//注意要剔除部分的点
	removeCount = 0;
	for(i=0;i<m_points.size();i++){
		if(m_valueList.at(i)<=threshold){
			m_points2.push_back(m_points[i]);
		}
		else{
			removeCount++;
		}
	}
	m_points = m_points2;
	
	return removeCount;
}

template <class T>
int RT_3DKnown_SmoopthExpress_Rotation<T>::RemoveOutlier(const Wml::GVector<T>& x, T N)
{
	int i;
	T v,sum;
	std::vector<T>	m_valueList;
	std::vector<MatchPoint*>	m_points2;
	std::vector<T>	m_weights2;
	int removeCount;
	SetXVar(x);
	T threshold;
	
	sum = 0.0;
	for(i=0;i<m_points.size();i++){
		u1=m_points[i]->v_2d.X();
		v1=m_points[i]->v_2d.Y();
		X=m_points[i]->v_3d.X();
		Y=m_points[i]->v_3d.Y();
		Z=m_points[i]->v_3d.Z();
		w = m_weights[i];

		v =pow(((cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1),2)+pow(((sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+
			(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1),2); 
		v = sqrt(v);
		m_valueList.push_back(v);
		sum += v;
	}

	threshold = N*sum/m_points.size();

	//注意要剔除部分的点
	removeCount = 0;
	for(i=0;i<m_points.size();i++){
		if(m_valueList.at(i)<=threshold){
			m_points2.push_back(m_points[i]);
			m_weights2.push_back(m_weights[i]);
		}
		else{
			removeCount++;
		}
	}
	m_points = m_points2;
	m_weights = m_weights2;

	printf("remove %d points\n",removeCount);
	
	return removeCount;
}

template <class T>
T RT_3DKnown_SmoopthExpress_Rotation<T>::RMSE(const Wml::GVector<T>& x)
{
	SetXVar(x);
	double sum = 0.0;

	for(int i=0;i<m_points.size();i++){
		u1=m_points[i]->v_2d.X();
		v1=m_points[i]->v_2d.Y();
		X=m_points[i]->v_3d.X();
		Y=m_points[i]->v_3d.Y();
		Z=m_points[i]->v_3d.Z();
		
		sum+=PowN(((cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1),2)+PowN(((sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+
			(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1),2); 
	}

	if(sum>=0.0)
		sum = sqrt(sum/m_points.size());
	else{
//		for(i=0;i<10000;i++)
//		printf("error");
	}


	return sum;
}

template <class T>
T RT_3DKnown_SmoopthExpress_Rotation<T>::ProjectionError(const Wml::GVector<T>& x)
{
	SetXVar(x);
	T sum = 0.0;

	for(int i=0;i<m_points.size();i++){
		u1=m_points[i]->v_2d.X();
		v1=m_points[i]->v_2d.Y();
		X=m_points[i]->v_3d.X();
		Y=m_points[i]->v_3d.Y();
		Z=m_points[i]->v_3d.Z();
		
		sum+=pow((w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1),2)+pow((w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1),2); 
	}
	
	return sum;
}

template <class T>
T RT_3DKnown_SmoopthExpress_Rotation<T>::Value (const Wml::GVector<T>& x)
{
	T val=0;
	SetXVar(x);
	
	for(int i=0;i<m_points.size();i++){
		u1=m_points[i]->v_2d.X();
		v1=m_points[i]->v_2d.Y();
		X=m_points[i]->v_3d.X();
		Y=m_points[i]->v_3d.Y();
		Z=m_points[i]->v_3d.Z();
		w=m_weights[i];
		
		val+=pow((w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1),2)+pow((w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1),2); 
	}

	if(rw!=0.0)
		val += pow(rw,2)*(pow((alpha-alpha0),2)+pow((beta-beta0),2)+pow((gama-gama0),2)); 
	
	return val;
}

template <class T>
void RT_3DKnown_SmoopthExpress_Rotation<T>::Gradient (const Wml::GVector<T>& x, Wml::GVector<T>& grad)
{
	int i,j;
	SetXVar(x);
	if( grad.GetSize() != m_LocalVariableIndex.size() )
		grad.SetSize( m_LocalVariableIndex.size() );
	
	for(i=0;i<m_LocalVariableIndex.size();i++){
		grad[i]=0.0;
	}
	
	for(j=0;j<m_points.size();j++){
		u1=m_points[j]->v_2d.X();
		v1=m_points[j]->v_2d.Y();
		X=m_points[j]->v_3d.X();
		Y=m_points[j]->v_3d.Y();
		Z=m_points[j]->v_3d.Z();
		w=m_weights[j];

		for(i=0;i<m_LocalVariableIndex.size();i++) 
		{
			switch (m_LocalVariableIndex[i])
			{
		case 0:
			grad[i]+=2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*(w*((sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Y+(sinGama*cosAlpha-cosGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+
			cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z))+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*(w*((-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Y+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+
			cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z));
			break;
		case 1:
			grad[i]+=2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*(w*(-cosGama*sinBeta*X+cosGama*cosBeta*sinAlpha*Y+cosGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z))+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*(w*(-sinGama*sinBeta*X+sinGama*cosBeta*sinAlpha*Y+sinGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z));
			break;
		case 2:
			grad[i]+=2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*w*(-sinGama*cosBeta*X+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+
			(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z);
			break;
			}//end of switch(i)
		}//end of for
	}

	if(rw!=0.0){		
		for(int i=0;i<m_LocalVariableIndex.size();i++) 
		{
			switch (m_LocalVariableIndex[i])
			{
			case 0:
				grad[i]+=pow(rw,2)*(2*alpha-2*alpha0);
				break;
			case 1:
				grad[i]+=pow(rw,2)*(2*beta-2*beta0);
				break;
			case 2:
				grad[i]+=pow(rw,2)*(2*gama-2*gama0);
				break;
			}//end of switch(i)
		}//end of for
	}
}

template <class T>
void RT_3DKnown_SmoopthExpress_Rotation<T>::Hesse (const Wml::GVector<T>& x, Wml::GMatrix<T>& hesse)
{
	int i,j,k;
	SetXVar(x);
//	if( hesse.GetSize()!=m_LocalVariableIndex.size() )
//		hesse.SetSize(m_LocalVariableIndex.size());
	
	for(i=0;i<m_LocalVariableIndex.size();i++){
		for(j=0;j<m_LocalVariableIndex.size();j++)
			hesse(i,j)=0.0;
	}
	
	for(k=0;k<m_points.size();k++){
		u1=m_points[k]->v_2d.X();
		v1=m_points[k]->v_2d.Y();
		X=m_points[k]->v_3d.X();
		Y=m_points[k]->v_3d.Y();
		Z=m_points[k]->v_3d.Z();
		w=m_weights[k];

	for( int i=0;i<m_LocalVariableIndex.size();i++)
		for ( int j=i;j<m_LocalVariableIndex.size();j++)
	{
		switch (m_LocalVariableIndex[i])
		{
		case 0:
			switch(m_LocalVariableIndex[j])
			{
			case 0:
				hesse(i,j)+=2*pow((w*((sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Y+(sinGama*cosAlpha-cosGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)),2)+2*(w*(cosGama*cosBeta*X+(-
			sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*(w*((sinGama*cosAlpha-cosGama*sinBeta*sinAlpha)*Y+(-sinGama*sinAlpha-cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-2*w*((sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Y+(sinGama*cosAlpha-cosGama*sinBeta*sinAlpha)*Z)/pow((-sinBeta*X+
			cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)+2*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),3)*pow((cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z),2)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z
			)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*sinAlpha*Y-cosBeta*cosAlpha*Z))+2*pow((w*((-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Y+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z
			),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)),2)+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*(w*((-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-2*w*((-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Y+
			(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)+2*w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),3)*pow((cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z),2)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+
			(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*sinAlpha*Y-cosBeta*cosAlpha*Z));
				break;
			case 1:
				hesse(i,j)+=2*(w*((sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Y+(sinGama*cosAlpha-cosGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z))*(w*(-cosGama*sinBeta*X+cosGama*cosBeta*sinAlpha*Y+
			cosGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z))+2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*(w*(cosGama*cosBeta*cosAlpha*Y-cosGama*cosBeta*sinAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(-cosGama*sinBeta*X+cosGama*cosBeta*sinAlpha*Y+cosGama*cosBeta*cosAlpha*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)-w*((sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Y+(sinGama*cosAlpha-
			cosGama*sinBeta*sinAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)+2*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),3)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)-w*(cosGama*cosBeta*X+
			(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-sinBeta*cosAlpha*Y+sinBeta*sinAlpha*Z))+2*(w*((-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Y+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+
			(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z))*(w*(-sinGama*sinBeta*X+sinGama*cosBeta*sinAlpha*Y+sinGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+
			cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z))+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*(w*(sinGama*cosBeta*cosAlpha*Y-sinGama*cosBeta*sinAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(-sinGama*sinBeta*X+sinGama*cosBeta*sinAlpha*Y+sinGama*cosBeta*cosAlpha*Z)/pow((-
			sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)-w*((-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Y+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)+2*w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),3)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-sinBeta*cosAlpha*Y+sinBeta*sinAlpha*Z));
				break;
			case 2:
				hesse(i,j)+=2*(w*((sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Y+(sinGama*cosAlpha-cosGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z))*w*(-sinGama*cosBeta*X+(-cosGama*cosAlpha-
			sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)+2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*w*((cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Y+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+
			cosBeta*cosAlpha*Z)-2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*w*(-sinGama*cosBeta*X+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z)+2*(w*((-
			cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Y+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z))*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+
			(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*w*((sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Y+(sinGama*cosAlpha-cosGama*sinBeta*sinAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z
			)-2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(cosBeta*cosAlpha*Y-cosBeta*sinAlpha*Z);
				break;
			}//end of switch(j)
			break;
		case 1:
			switch(m_LocalVariableIndex[j])
			{
			case 1:
				hesse(i,j)+=2*pow((w*(-cosGama*sinBeta*X+cosGama*cosBeta*sinAlpha*Y+cosGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)),2)+2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+
			cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*(w*(-cosGama*cosBeta*X-cosGama*sinBeta*sinAlpha*Y-cosGama*sinBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-2*w*(-cosGama*sinBeta*X+cosGama*cosBeta*sinAlpha*Y+cosGama*cosBeta*cosAlpha*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-
			sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)+2*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),3)*pow((-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z),2)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+
			cosBeta*cosAlpha*Z),2)*(sinBeta*X-cosBeta*sinAlpha*Y-cosBeta*cosAlpha*Z))+2*pow((w*(-sinGama*sinBeta*X+sinGama*cosBeta*sinAlpha*Y+sinGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-
			sinBeta*cosAlpha*Z)),2)+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*(w*(-sinGama*cosBeta*X-sinGama*sinBeta*sinAlpha*Y-sinGama*sinBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-2*w*(-sinGama*sinBeta*X+sinGama*cosBeta*sinAlpha*Y+sinGama*cosBeta*cosAlpha*Z)/pow((-
			sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)+2*w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),3)*pow((-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z),2)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+
			sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(sinBeta*X-cosBeta*sinAlpha*Y-cosBeta*cosAlpha*Z));
				break;
			case 2:
				hesse(i,j)+=2*(w*(-cosGama*sinBeta*X+cosGama*cosBeta*sinAlpha*Y+cosGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z))*w*(-sinGama*cosBeta*X+(-cosGama*cosAlpha-
			sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)+2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*w*(sinGama*sinBeta*X-sinGama*cosBeta*sinAlpha*Y-sinGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z
			)-2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*w*(-sinGama*cosBeta*X+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z)+2*(w*(-sinGama*sinBeta*X+
			sinGama*cosBeta*sinAlpha*Y+sinGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z))*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+
			cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*w*(-cosGama*sinBeta*X+cosGama*cosBeta*sinAlpha*Y+cosGama*cosBeta*cosAlpha*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+
			sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)*(-cosBeta*X-sinBeta*sinAlpha*Y-sinBeta*cosAlpha*Z);
				break;
			}//end of switch(j)
			break;
		case 2:
			switch(m_LocalVariableIndex[j])
			{
			case 2:
				hesse(i,j)+=2*pow(w,2)*pow((-sinGama*cosBeta*X+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z),2)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)+2*(w*(cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-u1)*w*(-cosGama*cosBeta*X+(sinGama*cosAlpha-
			cosGama*sinBeta*sinAlpha)*Y+(-sinGama*sinAlpha-cosGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)+2*pow(w,2)*pow((cosGama*cosBeta*X+(-sinGama*cosAlpha+cosGama*sinBeta*sinAlpha)*Y+(sinGama*sinAlpha+cosGama*sinBeta*cosAlpha)*Z),2)/pow((-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z),2)+2*(w*(sinGama*cosBeta*X+(cosGama*cosAlpha+sinGama*sinBeta*sinAlpha)*Y+(-cosGama*sinAlpha+sinGama*sinBeta*cosAlpha)*Z
			)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z)-v1)*w*(-sinGama*cosBeta*X+(-cosGama*cosAlpha-sinGama*sinBeta*sinAlpha)*Y+(cosGama*sinAlpha-sinGama*sinBeta*cosAlpha)*Z)/(-sinBeta*X+cosBeta*sinAlpha*Y+cosBeta*cosAlpha*Z);
				break;
			}//end of switch(j)
			break;
		}//end of switch(i)
		hesse(j,i)=hesse(i,j);
	}//end of for
	}
	

	if(rw!=0.0){
		
		for( int i=0;i<m_LocalVariableIndex.size();i++)
			for ( int j=i;j<m_LocalVariableIndex.size();j++)
			{
				switch (m_LocalVariableIndex[i])
				{
				case 0:
					switch(m_LocalVariableIndex[j])
					{
					case 0:
						hesse(i,j)+=2*pow(rw,2);
						break;
					case 1:
						hesse(i,j)+=0;
						break;
					case 2:
						hesse(i,j)+=0;
						break;
					}//end of switch(j)
					break;
					case 1:
						switch(m_LocalVariableIndex[j])
						{
						case 1:
							hesse(i,j)+=2*pow(rw,2);
							break;
						case 2:
							hesse(i,j)+=0;
							break;
						}//end of switch(j)
						break;
						case 2:
							switch(m_LocalVariableIndex[j])
							{
							case 2:
								hesse(i,j)+=2*pow(rw,2);
								break;
							}//end of switch(j)
							break;
				}//end of switch(i)
				hesse(j,i)=hesse(i,j);
			}//end of for
	}
}