#include "HIMACAnal.hh"
#include "Utils.hh"
void PositionAnal(){
	TString Dir = "./PositionStudy/";
	TString Ped = "Pedestal1.root";
	TString Pos[7] = { "VME_44_cor.root","VME_45_cor.root","VME_46_cor.root","VME_43_cor.root","VME_47_cor.root","VME_48_cor.root","VME_49_cor.root"};
	TFile* PedFile = new TFile(Dir+Ped,"read");
	TTree* PedTree = (TTree*)PedFile->Get("tree");
	TFile* PosFile[7];
	TTree* PosTree[7];
	for(int i=0;i<7;++i){
		PosFile[i] = new TFile(Dir+Pos[i],"read");
		PosTree[i] = (TTree*)PosFile[i]->Get("tree");
	}
	TString Titles[4] = {"BTOFUL","BTOFUR","BTOFDL","BTOFDR"};
	TString XPos[7] = {"-50cm","-40cm","-20cm","+00cm","+20cm","+40cm","+50cm"};
	double t1peak[7];
	double t1width[7];
	double t2peak[7];
	double t2width[7];
	double tdpeak[7];
	double tdwidth[7];
	double xpos[2][7] ={
		{-50,-40,-20,0,20,40,50},
		{-50,-40,-20,0,20,40,50},
	};
	double xposErr[7] = {1,1,1,1,1,1,1};
	int ch[4] = {2,3,4,5};	
	TH1D* PedHist[4];
	TH1D* PosHist[4][7];
	double PedPeak[4];
	double PedWidth[4];
	double PosPeak[4][7];
	double PosPeakErr[4][7];
	double PosPeakRat[2][7];
	double PosPeakRatErr[2][7]; double PosPeakGMean[2][7];
	double PosPeakGMeanErr[2][7];
	double PHCp0[6];
	double PHCp1[6];
	double PHCp2[6];
	double data[10];
	ifstream f;
	f.open("params/PHCParam.txt");
	ReadConfLine(f,data);	
	for(int i=0;i<6;++i){
		ReadConfLine(f,data);	
		PHCp0[i]=data[1];
		PHCp1[i]=data[2];
		PHCp2[i]=data[3];
	}
	ifstream f2;
	f2.open("params/TimeParam.txt");
	ReadConfLine(f2,data);	
	for(int i=0;i<7;++i){
		ReadConfLine(f2,data);	
		t1peak[i]= data[1];
		t1width[i]= data[2];
		t2peak[i]= data[3];
		t2width[i]= data[4];
		tdpeak[i]= data[5];
		tdwidth[i]= data[6];
		cout<<data[1]<<endl;
	}
	TCanvas* c1 = new TCanvas("c1","c1",00,00,800,600);
	c1->Divide(2,2);
	for(int i=0;i<4;++i){
		c1->cd(i+1);
		PedHist[i] = new TH1D(Titles[i]+"ped",Titles[i]+"ped",300,0,300);
		TString command = Form("qdc[%d]>>"+Titles[i]+"ped",ch[i]);	
		PedTree->Draw(command);
		PedHist[i]->Fit("fGaussian","QR");
		PedPeak[i] = fGaussian->GetParameter(1);
		PedWidth[i] = fGaussian->GetParameter(2);
	}
	int ent = LoadBranches(	PosTree[3]);
	
	TCanvas* c2 = new TCanvas("c2","c2",50,50,800,600);
	c2->Divide(7,4);







	TCanvas* c3 = new TCanvas("c3","c3",100,100,800,600);
	c3->Divide(7,4);
	int nsig = 5;
	TGraphErrors* AttenGraph[6];
	for(int i=0;i<4;++i){
//		AttenGraph[i]->SetMarkerStyle(8);
//		AttenGraph[i]->SetMarkerSize(1);
		for(int j=0;j<7;++j){
			c3->cd(7*i+j+1);
			PosHist[i][j] = new TH1D(Titles[i]+XPos[j],Titles[i]+XPos[j],1500,0,1500);
			TString command = Form("qdc[%d]>>"+Titles[i]+XPos[j],ch[i]);	
			TCut cut = "tdc[2]+tdc[3]>0&&tdc[4]+tdc[5]>0";
			TCut cutT1 =Form("abs(%f*(tdc_cor[2]+tdc_cor[3]-tdc_cor[8]-tdc_cor[9])/2-%f)<2*%f",lsb,t1peak[j],t1width[j]);
			TCut cutT2 =Form("abs(%f*(tdc_cor[4]+tdc_cor[5]-tdc_cor[8]-tdc_cor[9])/2-%f)<2*%f",lsb,t2peak[j],t2width[j]);
			TCut cutTD =Form("abs(%f*(tdc_cor[4]+tdc_cor[5]-tdc_cor[2]-tdc_cor[3])/2-%f)<2*%f",lsb,tdpeak[j],tdwidth[j]);
			cut=cut&&cutT1&&cutT2&&cutTD;
			PosTree[j]->Draw(command,cut);
			fGaussian->SetRange(PedPeak[i]+nsig*PedWidth[i],1500);
			PosHist[i][j]->Fit("fGaussian","QR");
			double peakpos = fGaussian->GetParameter(1);
			double peaksig = fGaussian->GetParameter(2);
			fGaussian->SetRange(peakpos-peaksig,peakpos+peaksig);
			PosHist[i][j]->Fit("fGaussian","QR");
			PosPeak[i][j] = fGaussian->GetParameter(1)-PedPeak[i];
//			PosPeak[i][j] = GetPeakPosition(PosHist[i][j])-PedPeak[i];
			if(i==0 and j == 4){
	//			PosPeak[i][j]+=15;
			}
			PosPeakErr[i][j] = fGaussian->GetParError(1);
			if(i%2==1 ){
				PosPeakRat[i/2][j] = PosPeak[i][j]/PosPeak[i-1][j];
				PosPeakRatErr[i/2][j] = PosPeakRat[i/2][j] * sqrt(square(PosPeakErr[i-1][j] /PosPeak[i-1][j])+square(PosPeakErr[i][j]/PosPeak[i-1][j]));
			}
		}
	}
	for(int i=0;i<4;++i){
		AttenGraph[i] = new TGraphErrors(7,xpos[0],PosPeak[i],xposErr,PosPeakErr[i]);
	}
	for(int i=0;i<2;++i){
		AttenGraph[i+4] = new TGraphErrors(7,xpos[i/2],PosPeakRat[i],xposErr,PosPeakRatErr[i]);
	}
	TCanvas* c4 = new TCanvas("c4","c4",150,150,800,600);
	c4->Divide(3,2);
	c4->cd(1);
	AttenGraph[0]->Draw();
	AttenGraph[0]->Fit("expo","");
	c4->cd(2);
	AttenGraph[1]->Draw();
	AttenGraph[1]->Fit("expo","");
	c4->cd(3);
	AttenGraph[2]->Draw();
	AttenGraph[2]->Fit("expo","");
	c4->cd(4);
	AttenGraph[3]->Draw();
	AttenGraph[3]->Fit("expo","");
	c4->cd(5);
	fExpo->SetParLimits(0,0.5,1.5);
	fExpo->SetParLimits(1,50,150);
	fExpo->SetParName(0,"Constant");
	fExpo->SetParName(1,"Attenuation");
	AttenGraph[4]->Draw("AP");
	AttenGraph[4]->SetTitle("BToFU_Attenuation");
	AttenGraph[4]->GetXaxis()->SetTitle("Position [cm]");
	AttenGraph[4]->GetYaxis()->SetTitle("GainL/GainR");
	AttenGraph[4]->GetYaxis()->SetRangeUser(0,5);
	AttenGraph[4]->Fit("fExpo","R");//No Reason to fit double expo
	c4->cd(6);
	AttenGraph[5]->Draw("AP");
	AttenGraph[5]->SetTitle("BToFU_Attenuation");
	AttenGraph[5]->GetXaxis()->SetTitle("Position [cm]");
	AttenGraph[5]->GetYaxis()->SetTitle("GainL/GainR");
	AttenGraph[5]->GetYaxis()->SetRangeUser(0,5);
	AttenGraph[5]->Fit("fExpo","R");
}
