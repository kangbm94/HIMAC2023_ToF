#include "HIMACAnal.hh"
#include "Utils.hh"
void DoPHC(TString filename);
void TimeCallibration(){
	cout<<"FitLandau(TString filename)"<<endl;//Pulse Height Correction
	cout<<"DoPHC(TString filename)"<<endl;//Pulse Height Correction
	cout<<"DoPHCCallibration(TString filename)"<<endl;//Pulse Height Correction
	fGaussian->SetParName(0,"constant");
	fGaussian->SetParName(1,"mean");
	fGaussian->SetParName(2,"sigma");
}
void DoPHCCallibration(TString filename){
	TFile* file = new TFile(filename,"read");
	TTree* tree = (TTree*)file->Get("tree");
	int ent = LoadBranches(tree);
	double tdc_cor[16];
	TString filename_calib = filename.ReplaceAll(".root","");
	filename_calib+="_cor.root";
	TFile* file_calib = new TFile(filename_calib,"recreate");
	TTree* tree_calib = new TTree("tree","tree");
	tree_calib-> Branch("qdc",qdc,"qdc[16]/I");
	tree_calib-> Branch("tdc",tdc,"tdc[16]/I");
	tree_calib-> Branch("tdc_cor",tdc_cor,"tdc_cor[16]/D");

	int nch = 4;
//	int ch[6] = {2,3,4,5,8,9};
	int ch[6] = {2,3,4,5};
	double p0[6],p1[6],p2[6],data[6];
	
	ifstream f2;
	f2.open("Params/PHCParam.txt");
	ReadConfLine(f2,data);

	for(int i=0;i<6;++i){
		ReadConfLine(f2,data);
		cout<<data[0]<<endl;
		cout<<data[1]<<endl;
		cout<<data[2]<<endl;
		cout<<data[3]<<endl;
		p0[i]=data[1];
		p1[i]=data[2];
		p2[i]=data[3];
	}

	for(int i=0;i<ent;++i){
		tree->GetEntry(i);
		for(int j=0;j<16;++j){
			tdc_cor[j]=tdc[j];
		}
		for(int j=0;j<nch;++j){
			tdc_cor[ch[j]]=tdc[ch[j]]-Correction(qdc[ch[j]],p0[j],p1[j],p2[j]);
		}
		tree_calib->Fill();
	}
	file_calib->Write();
}

void DoPHC(TString filename){
	gStyle->SetOptStat(1);	
	TFile* file = new TFile(filename,"read");
	TTree* tree = (TTree*)file->Get("tree");
	int ent = LoadBranches(tree);
	TString Titles[6] = { "BTOFL","BTOFUR","BTOFDL","BTOFDR","SCL","SCR"};
	int ch[6] = {2,3,4,5,8,9};
	

	double nsig = 2.1;
	TH1D* QDCHists[6];
	TCanvas*c1 = new TCanvas("c1","c1",1200,600);
	c1->Divide(3,2);
	double QMean[6],QSig[6];
	for(int i = 0;i<6;++i){
		QDCHists[i] = new TH1D(Titles[i]+"_QDC",Titles[i]+"_QDC",1000,0,4000);
		TString command = Form("qdc[%d]>>",ch[i])+Titles[i]+"_QDC";
		TString cut;
		if(i%2==0) cut= Form("tdc[%d]>0&&tdc[%d]>0",ch[i],ch[i]+1);
		else cut= Form("tdc[%d]>0&&tdc[%d]>0",ch[i],ch[i]-1);
		if (i> 3) cut = cut + Form("&&qdc[%d]>105",ch[4]);
		c1->cd(i+1);
		tree->Draw(command,cut);
		double peak = GetPeakPosition(QDCHists[i]);
		fGaussian->SetRange(peak-200,peak+200);
		QDCHists[i]->Fit("fGaussian","QR");
		QMean[i]=fGaussian->GetParameter(1);
		QSig[i]=fGaussian->GetParameter(2);
	}
/*--------------------------------------------*/

	

	TH2D* PHCHists[6];
	TCanvas*c2 = new TCanvas("c2","c2",1200,600);
	c2->Divide(3,2);
	
	double QRange1[6] = {00 ,00 ,00 ,00 ,00 ,00};
	double QRange2[6] = {2000 ,2000 ,2000 ,2000 ,500 ,700};
	double TRange1[6] = {-500,-500,-500,-500,-1000,-1000};
	double TRange2[6] = {500 ,500 ,500 ,500 ,00 ,00};
	double p1Min[6] = {0.5,0.5,0.5,0.5,0.5,0.5};
	double p1Max[6] = {5  ,5  ,5  ,5  ,5  ,10 };
	double p2Min[6] = {-900,-900,-900,-900 ,-00 ,00};
	double p2Max[6] = {300,300,300,300,150,200 };
	double p0[6],p1[6],p2[6];
	TGraph* PHCGraph[6];
	const int nSlice = 15;
	double tdcpnt[6][nSlice];
	double tdcpnterr[6][nSlice];
	double qdcpnt[6][nSlice];
	double qdcpnterr[6][nSlice];
//
/*
	fstream f;
	f.open("Params/PHCParam.txt",fstream::out);
	f<<"#ch	p0	p1	p2"<<endl;
	*/
	ifstream f2;
	f2.open("Params/PHCParam.txt");
	double data[6];
	ReadConfLine(f2,data);
	
	for(int i=0;i<6;++i){
		ReadConfLine(f2,data);
		cout<<data[0]<<endl;
		cout<<data[1]<<endl;
		cout<<data[2]<<endl;
		cout<<data[3]<<endl;
		p0[i]=data[1];
		p1[i]=data[2];
		p2[i]=data[3];
	}

	TH2D* CorHists[6];
	
	for(int i = 0;i<6;++i){
		PHCHists[i] = new TH2D(Titles[i],Titles[i],300,QRange1[i],QRange2[i],100,TRange1[i],TRange2[i]);
		TString command = Form("(tdc[%d]-(tdc[2]+tdc[3]+tdc[4]+tdc[5])/4):qdc[%d]>>",ch[i],ch[i])+ Titles[i];
		//		TString command = Form("(tdc[%d]-(tdc[0]):qdc[%d]>>",ch[i],ch[i])+ Titles[i];
		TCut cut = "tdc[2]+tdc[3]>0&&tdc[4]+tdc[5]>0&&tdc[8]+tdc[9]>0";
		TCut QDCCut = "";
		for(int j=0;j<6;++j){
			QDCCut = QDCCut &&(TCut) Form("abs(qdc[%d]-%f)<30.*%f",ch[j],QMean[j],QSig[j]);
		}
		c2->cd(i+1);
		tree->Draw(command,cut&&QDCCut,"colz");
		int bin_first = Bin(300,QRange1[i],QRange2[i],QMean[i]-nsig*QSig[i]); 
		int bin_last = Bin(300,QRange1[i],QRange2[i],QMean[i]+nsig*QSig[i]);
		double bw = (bin_last - bin_first)*1. /nSlice;
		for(int j=0;j<nSlice;++j){
			int nb1 = (bin_first+bw*(j)); 
			int nb2 = (bin_first+bw*(j+1)); 
			auto* h = PHCHists[i]->ProjectionY(Titles[i]+Form("slice%d",j),nb1,nb2,"0");
			h->Fit("fGaussian","Q0");
			tdcpnt[i][j]=fGaussian->GetParameter(1);
			tdcpnterr[i][j]=fGaussian->GetParError(1);
//			tdcpnt[i][j]=h->GetMean();
//			tdcpnterr[i][j]=h->GetMeanError();
			qdcpnt[i][j]=BinPos(300,QRange1[i],QRange2[i],(nb1+nb2)/2);
			qdcpnterr[i][j]=2.*nsig*QSig[i]/nSlice/sqrt(12);
		}

		PHCGraph[i] = new TGraphErrors(nSlice,qdcpnt[i],tdcpnt[i],qdcpnterr[i],tdcpnterr[i]);
		PHCGraph[i]->Draw("same");
		double window = TRange2[i]-TRange1[i];
		fCorrection->SetRange(QMean[i]-3.5*QSig[i],QMean[i]+3.5*QSig[i]);
		cout<<Titles[i]<<endl;
		cout<<Form("Pars : %f , %f , %f",p0[i],p1[i],p2[i])<<endl;
		fCorrection->SetParameter(0,p0[i]);
		fCorrection->SetParameter(1,p1[i]);
		fCorrection->SetParameter(2,p2[i]);
		fCorrection->Draw("same");
		c2->Modified();
		c2->Update();
#if 0
		PHCGraph[i]->Fit("fCorrection","R");
		p0[i] = fCorrection->GetParameter(0); 
		p1[i] = fCorrection->GetParameter(1); 
		p2[i] = fCorrection->GetParameter(2); 

		TString pars = Form("%d, %f, %f, %f",ch[i],p0[i],p1[i],p2[i]);
		f<<pars.Data()<<endl;
#endif
	}


#if 1
	for(int i=0;i<6;++i){
		CorHists[i] = new TH2D(Titles[i]+"Cor",Titles[i]+"Cor",100,QRange1[i],QRange2[i],100,-500,500);
	}
	TCanvas*c3 = new TCanvas("c3","c3",1200,600);
	c3->Divide(3,2);
	TH1D* ToFHistU = new TH1D("HistToFU","HistToFU",500,-2,2);
	TH1D* ToFHistD = new TH1D("HistToFD","HistToFD",500,-2,2);
	TH1D* ToFHist = new TH1D("HistToF","HistToFD-U",500,-2,2);

	TH1D* ToFHistUAll = new TH1D("HistAllToFU","HistAllToFU",500,-5,5);
	TH1D* ToFHistDAll = new TH1D("HistAllToFD","HistAllToFD",500,-5,5);
	TH1D* ToFHistAll = new TH1D("HistAllToFD-U","HistAllToFD-U",500,-5,5);

	TH1D* TDHistU = new TH1D("TimeDifferenceU","TimeDifferenceU",500,-5,5);
	TH1D* TDHistD = new TH1D("TimeDifferenceD","TimeDifferenceD",500,-5,5);
	TH1D* TDHistUAll = new TH1D("TimeDifferenceUAll","TimeDifferenceUAll",500,-5,5);
	TH1D* TDHistDAll = new TH1D("TimeDifferenceDAll","TimeDifferenceDAll",500,-5,5);

	TH2D* ToFCorAll = new TH2D("HistToFU vs DAll","HistToFU vs DAll",100,-5,5,100,-5,5);
	TH2D* ToFCor = new TH2D("HistToFU vs D","HistToFU vs D",100,-5,5,100,-5,5);

	double tdc_cor[16];
	TFile* file_out = new TFile("ToF_Cor.root","recreate");
	TTree* tree_out = new TTree("tree","tree");
	tree_out->Branch("qdc",qdc);
	tree_out->Branch("tdc",tdc_cor);
	for(int i=0;i<ent;++i){
		tree->GetEntry(i);
		for(int j=0;j<16;++j){
			tdc_cor[j]=tdc[j];
		}
		for(int j=0;j<6;++j){
			tdc_cor[ch[j]]=tdc[ch[j]]-Correction(qdc[ch[j]],p0[j],p1[j],p2[j]);
		}
		tree_out->Fill();
		double TrigTime = (tdc_cor[ch[4]]+tdc_cor[ch[5]])/2;
//		double TrigTime = (tdc[ch[4]]+tdc[ch[5]])/2+600;
//		double TrigTime = (tdc_cor[ch[0]]+tdc_cor[ch[1]]+tdc_cor[ch[2]]+tdc_cor[ch[3]])/4;
		if(tdc[ch[0]]+tdc[ch[1]]>0 and tdc[ch[2]]+tdc[ch[3]]>0 and tdc[ch[4]]+tdc[ch[5]]>0){
			ToFHistUAll->Fill(lsb*((tdc_cor[ch[0]]+tdc_cor[ch[1]])/2-TrigTime));
			ToFHistDAll->Fill(lsb*((tdc_cor[ch[2]]+tdc_cor[ch[3]])/2-TrigTime));
			ToFHistAll ->Fill(lsb*((tdc_cor[ch[2]]+tdc_cor[ch[3]])/2-(tdc_cor[ch[0]]+tdc_cor[ch[1]])/2));
			TDHistUAll->Fill(lsb*((tdc_cor[ch[0]]-tdc_cor[ch[1]])/2));
			TDHistDAll->Fill(lsb*((tdc_cor[ch[2]]-tdc_cor[ch[3]])/2));
			ToFCorAll ->Fill(
					lsb*((tdc_cor[ch[0]]+tdc_cor[ch[1]])/2-TrigTime),
					lsb*((tdc_cor[ch[2]]+tdc_cor[ch[3]])/2-TrigTime)
					);
			if((1
					and	abs(qdc[ch[0]]-QMean[0])<nsig*QSig[0] 
					and abs(qdc[ch[1]]-QMean[1])<nsig*QSig[1] 
					and abs(qdc[ch[2]]-QMean[2])<nsig*QSig[2] 
					and abs(qdc[ch[3]]-QMean[3])<nsig*QSig[3] 
					and qdc[ch[4]]-QMean[4]>-nsig*QSig[4] )
					or 0){
				for(int k = 0; k < 6;++k){
					CorHists[k]->Fill(qdc[ch[k]],tdc_cor[ch[k]]-TrigTime);
				}
				ToFHistU->Fill(lsb*((tdc_cor[ch[0]]+tdc_cor[ch[1]])/2-TrigTime));
				ToFHistD->Fill(lsb*((tdc_cor[ch[2]]+tdc_cor[ch[3]])/2-TrigTime));
				ToFHist ->Fill(lsb*((tdc_cor[ch[2]]+tdc_cor[ch[3]])/2-(tdc_cor[ch[0]]+tdc_cor[ch[1]])/2));
				TDHistU->Fill(lsb*((tdc_cor[ch[0]]-tdc_cor[ch[1]])/2));
				TDHistD->Fill(lsb*((tdc_cor[ch[2]]-tdc_cor[ch[3]])/2));
				ToFCor ->Fill(
						lsb*((tdc_cor[ch[0]]+tdc_cor[ch[1]])/2-TrigTime),
						lsb*((tdc_cor[ch[2]]+tdc_cor[ch[3]])/2-TrigTime)
						);
			}
		}
	}
	for(int i = 0; i < 6;++i){
		c3->cd(i+1);
		CorHists[i]->Draw("colz");
	}
#endif

	ToFHistU->SetLineColor(kBlack);
	ToFHistD->SetLineColor(kBlack);
	ToFHist ->SetLineColor(kBlack);
	TDHistU->SetLineColor(kBlack);
	TDHistD->SetLineColor(kBlack);

#if 1
	TCanvas*c4 = new TCanvas("c4","c4",1200,600);
	double peaks[3],widths[3];
	c4->Divide(3,1);
	c4->cd(1);
	ToFHistUAll->Draw();
	ToFHistU->Draw("same");
	double peak = GetPeakPosition(ToFHistUAll);
	fGaussian->SetRange(peak-0.3,peak+0.3);
	ToFHistU->Fit("fGaussian","QR");
	peak = fGaussian->GetParameter(1);
	double width = fGaussian->GetParameter(2);
	fGaussian->SetRange(peak-width,peak+width);
	ToFHistU->Fit("fGaussian","R");
	peaks[0] = peak;
	widths[0] = width;
	c4->cd(2);
	ToFHistDAll->Draw();
	ToFHistD->Draw("same");
	peak = GetPeakPosition(ToFHistDAll); fGaussian->SetRange(peak-0.3,peak+0.3); ToFHistD->Fit("fGaussian","QR");
	peak = fGaussian->GetParameter(1);
	width = fGaussian->GetParameter(2);
	fGaussian->SetRange(peak-width,peak+width);
	ToFHistD->Fit("fGaussian","R");
	peaks[1] = peak;
	widths[1] = width;
	c4->cd(3);
	ToFHistAll ->Draw();
	ToFHist ->Draw("same");
	peak = GetPeakPosition(ToFHistAll);
	fGaussian->SetRange(peak-0.2,peak+0.2);
	ToFHist ->Fit("fGaussian","QR");
	peak = fGaussian->GetParameter(1);
	width = fGaussian->GetParameter(2);
	fGaussian->SetRange(peak-width,peak+width);
	ToFHist->Fit("fGaussian","R");
	peaks[2] = peak;
	widths[2] = width;
#endif
	
	TH1D* ToFHistUCut = new TH1D("HistToFUCut","HistToFUCut",500,-2,2);
	TH1D* ToFHistDCut = new TH1D("HistToFDCut","HistToFDCut",500,-2,2);
	TH1D* ToFHistCut = new TH1D("HistToFCut","HistToFD-UCut",500,-2,2);
	TH1D* TDHistUCut = new TH1D("TimeDifferenceUCut","TimeDifferenceUCut",500,-5,5);
	TH1D* TDHistDCut = new TH1D("TimeDifferenceDCut","TimeDifferenceDCut",500,-5,5);
	
	TH2D* ToFCorCut = new TH2D("HistToFU vs DCut","HistToFU vs DCut",100,-5,5,100,-5,5);

	for(int i=0;i<ent;++i){
		tree->GetEntry(i);
		for(int j=0;j<16;++j){
			tdc_cor[j]=tdc[j];
		}
		for(int j=0;j<6;++j){
			tdc_cor[ch[j]]=tdc[ch[j]]-Correction(qdc[ch[j]],p0[j],p1[j],p2[j]);
		}
		double TrigTime = (tdc_cor[ch[4]]+tdc_cor[ch[5]])/2;
		double t1 = lsb*(tdc_cor[ch[0]]+tdc_cor[ch[1]])/2;	
		double t2 = lsb*(tdc_cor[ch[1]]+tdc_cor[ch[2]])/2;	
		double tt = lsb* TrigTime;
	
		if(tdc[ch[0]]+tdc[ch[1]]>0 and tdc[ch[2]]+tdc[ch[3]]>0 and tdc[ch[4]]+tdc[ch[5]]>0){
			if(1
					and	(abs(qdc[ch[0]]-QMean[0])<nsig*QSig[0] 
					and abs(qdc[ch[1]]-QMean[1])<nsig*QSig[1] 
					and abs(qdc[ch[2]]-QMean[2])<nsig*QSig[2] 
					and abs(qdc[ch[3]]-QMean[3])<nsig*QSig[3] 
					and abs(t1-tt-peaks[0])<2*widths[0] 
					and abs(t2-tt-peaks[1])<2*widths[1] 
					and abs(t2-t1-peaks[2])<2*widths[2]) 
//					and qdc[ch[4]]-QMean[4]>-nsig*QSig[4] )
					or 0){
				ToFCorCut->Fill(t1-tt,t2-tt);
				TDHistUCut->Fill(lsb*(tdc_cor[ch[0]]-tdc_cor[ch[1]])/2);
				TDHistDCut->Fill(lsb*(tdc_cor[ch[2]]-tdc_cor[ch[3]])/2);
			}
		}
	}
	for(int i=0;i<3;++i){
		cout<<"params : "<<peaks[i]<<" , "<<widths[i]<<endl;
	}
	TCanvas*c5 = new TCanvas("c5","c5",1200,600);
	c5->Divide(3,2);
	c5->cd(1);
	ToFCor ->Draw("colz");
	c5->cd(2);
	TDHistUAll ->Draw("");
	TDHistU ->Draw("same");
	c5->cd(3);
	TDHistDAll ->Draw("");
	TDHistD ->Draw("same");
	c5->cd(4);
	ToFCorCut->Draw("colz");
	c5->cd(5);
	TDHistUCut->Draw("colz");
	c5->cd(6);
	TDHistDCut->Draw("colz");
}
