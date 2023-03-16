#include "Constants.hh"
#ifndef Math_h
#define Math_h
double square(double a){ 
	return a*a;
}
double Norm(double x1,double x2=0,double x3=0,double x4=0){
	return sqrt(square(x1)+square(x2)+square(x3)+square(x4));
}
int PM(int a){
	if(a%2==0){
		return 1;
	}
	else{
		return -1;
	}
}
double Min(double a, double b){
	if(a>b){
		return b;
	}
	else{
		return a;
	}
}
double Max(double a, double b){
	if(a>b){
		return a;
	}
	else{
		return b;
	}
}
double Power(double x,int n){
		double value=1;
		for(int i=0;i<n;i++){
			value=value*x;
		}
		return value;
}
double Polynomial(double x, double* p,int n){
	double value=0;
	for(int i=0;i<n+1;i++){
		value+=p[i]*Power(x,i);
	}
	return value;
}
double SquareSum(double a, double b){
	return sqrt(a*a+b*b);
}
double NormGaussian(double x, double mean, double sigma, double amplitude){
	double par=(x-mean)/sigma;
	double val = amplitude*exp(-par*par/2)/sigma/sqrt(2*Pi());
	return val;
}
double Gaussian(double x, double mean, double sigma, double peak){
	double par=(x-mean)/sigma;
	double val = peak*exp(-par*par/2);
	return val;
}
double fGaussian(double* x,double* p){
	return Gaussian(x[0],p[0],p[1],p[2]);
}
double Step(double a){
	if(a>0){
		return 1;
	}
	else{
		return 0;
	}
}
//
double QuadRoot(double a,double b,double c, int conf){
	if(conf<0){
		return  (-b -sqrt(b*b-4*a*c))/(2*a);
	}
	else{
		return (-b +sqrt(b*b-4*a*c))/(2*a);
	}
}
double T1(double a,double b, double c, double d){
	double p0=1/(3*a);
	double p1=2*b*b*b-9*a*b*c+27*a*a*d;
	double p2 = b*b-3*a*c;
	double val= -b*p0-p0*(
			pow((p1+sqrt(p1*p1-4*p2*p2*p2))/2,1./3)
		 +pow((p1-sqrt(p1*p1-4*p2*p2*p2))/2,1./3)
			);
		return val;
}



//Randoms
double Rndm(double r1=0,double r2=1){
	double tmp = r2;
	if(r1>r2){
		r2=r1;
		r1=tmp;
	}
	double range = r2-r1;
	return r1+range*gRandom->Rndm();
}
void GenSphericalRandom(double &theta, double &phi){
	phi=Rndm(0,2*Pi());
	double cth=Rndm(-1,1.);
	theta=acos(cth);
}

TVector2 GenCircleRandom(double r, double phi1,double phi2){
	double pi = Pi();
	double phi=pi*Rndm(phi1/pi, phi2/pi);
	double x = r*cos(phi),y=r*sin(phi);
	return TVector2(x,y);
}

double chebyshev(double x, int n){
	double val;
	switch(n){
		case 0:
			val=1;
			break;
		case 1: 
			val=x;
			break;
		case 2: 
			val= 2*x*x-1;
			break;
		case 3:
			val= 4*x*x*x-3*x;
			break;
		case 4: 
			val= 8*x*x*x*x-8*x*x+1;
			break;
		case 5: 
			val= 16*x*x*x*x*x-20*x*x*x+5*x;
			break;
		case 6: 
			val= 32*x*x*x*x*x*x-48*x*x*x*x+18*x*x-1;
			break;
	}
	return val;
}
#endif
