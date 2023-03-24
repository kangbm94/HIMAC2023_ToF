int qdc[16],tdc[16];
double lsb = 0.025;
double square(double x){
	return x*x;
}
double Resolution(double s1, double s2, double s3){
	return sqrt(s2*s2+s3*s3-s1*s1)/sqrt(2);
}
double Correction(double x, double p0, double p1,double p2){
	return p0 + p1 * 1./sqrt(x-p2);
}
double Correction_(double* x, double* p){
	return Correction(x[0],p[0],p[1],p[2]);
}
double DoubleExpo(double x, double p0,double p1,double p2,double p3){
	return p0 * exp(x/p1)+p2*exp(x/p3);
}
double DoubleExpo_(double* x,double* p){
	return DoubleExpo(x[0],p[0],p[1],p[2],p[3]);
}
double Expo_(double* x,double* p){
	return p[0]*exp(2*x[0]/p[1]);
}
double ConjExpo_(double* x,double* p){
	return sqrt(DoubleExpo(x[0],p[0],p[1],p[2],p[3])*DoubleExpo(x[0],p[0],-p[1],p[2],-p[3]));
}
TF1* fPol1 = new TF1("fPol1","pol1",-100,100);
TF1* fCorrection = new TF1("fCorrection","Correction_",0,1000,3);
TF1* fGaussian = new TF1("fGaussian","gaus",0,300);
TF1* fLandau = new TF1("fLandau","landau",0,1500);
TF1* fExpo = new TF1("fExpo","Expo_",-60,60,2);
TF1* fDoubleExpo = new TF1("fDoubleExpo","DoubleExpo_",-60,60,4);
TF1* fConjExpo = new TF1("fConjExpo","ConjExpo_",-60,60,4); int LoadBranches(TTree* tree){ tree->SetBranchAddress("qdc",qdc);
	tree->SetBranchAddress("tdc",tdc);
	return tree->GetEntries();
}


