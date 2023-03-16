#include <chrono>
#define MatchEvent 1

using namespace chrono;
int32_t evheader = 0xffffffff;
int32_t evfooter = 0xfffffffe;
int32_t QDCHeader = 0xFFFFFFF0;
int32_t TDCHeader = 0xFFFFFFF1;
int32_t Feof = 0xfffffffd;
bool force_quit = false;
bool decoding = true;
int refreshh = 1000;
bool PrintErr = false;
double sleeping = 5;
void IntHandler(int sig){
	force_quit = true;
	decoding = false;
	cout<<"Quitting..."<<endl;
}
void Decoder(){
	signal(SIGINT, IntHandler);// interrupt signal(ctrl + c)
	cout<<"DecodeEvent(Tstring filename)"<<endl;	
	cout<<"EventDisplay(Tstring filename)"<<endl;	
}
int DecodeTime(FILE* file, int32_t*data){
	bool reading = true;
	int32_t datum;
	int ch;
	int evnum = 0;
	while(reading and decoding){
		fread(&datum,sizeof(int32_t),1,file);
		if(datum == Feof) return -9999;
		if(feof(file)){
			cout<<"Reached at the end of file"<<endl;
			//			file = fopen(name,"rb");
			return -1;
		}
		if(datum==evheader){
			ch = -1;
			continue;
		}
		else{
			ch++;
			data[ch]=datum;	
		}
		if(datum == evfooter){
			break;
		}
	}
	return evnum;
}

int Decode(FILE* file, int32_t*data, const char* name){
	bool reading = true;
	int32_t datum;
	int ch;
	int evnum = 0;
	while(reading and decoding){
		fread(&datum,sizeof(int32_t),1,file);
		if(datum == Feof){
			fread(&datum,sizeof(int32_t),1,file);
			cout<<"Run End! "<<datum<<endl;
			data[0] = datum;
			return -9999;
		}
		if(feof(file)){
			cout<<"Reached at the end of file"<<endl;
			//			file = fopen(name,"rb");
			return -1;
		}
		if(datum==evheader){
			ch=-1;
			continue;
		}
		else{
			if(ch<0){
				evnum = datum;
			}
			else{
				data[ch] = datum;
			}
			ch++;
		}
		if(datum == evfooter){
			break;
		}
	}
	return evnum;
}
void DecodeEvent(TString filename){
	force_quit = false;
	decoding=true;
	auto decode_start = duration_cast<seconds>(system_clock::now().time_since_epoch()); 
	const char* qname = (filename+".qdc").Data();
	const char* tname = (filename+".tdc").Data();
	FILE* fqdc = fopen((filename+".qdc").Data(),"rb");
	FILE* ftdc = fopen((filename+".tdc").Data(),"rb");
	FILE* ftime = fopen((filename+".timetag").Data(),"rb");
	int qdc[16];
	int tdc[16];
	int timetag[6];
	int evnum=0;
	int qdcid=0;
	int tdcid=0;
	TFile* file = new TFile(filename+".root","recreate");
	TTree* tree = new TTree("tree","tree");
	tree->Branch("evnum",&evnum,"evnum/I");
	tree->Branch("qdcid",&qdcid,"qdcid/I");
	tree->Branch("tdcid",&tdcid,"tdcid/I");
	tree->Branch("qdc",&qdc,"qdc[16]/I");
	tree->Branch("tdc",&tdc,"tdc[16]/I");
	tree->Branch("time",&timetag,"timetag[6]/I");
	TString titles[4] = {"BTOFUL","BTOFUR","BTOFDL","BTOFDR"};
	int ch[4]={2,3,4,5};
	int nev = -1;
	while(decoding and not force_quit){
		qdcid = Decode(fqdc,qdc,qname);   
		tdcid = Decode(ftdc,tdc,tname); 
		DecodeTime(ftime,timetag);
#if MatchEvent
		if(qdcid > -1 and tdcid > -1) {
			while(qdcid!=tdcid and not force_quit){
				while(qdcid<tdcid and not force_quit){
					if( PrintErr) cout<<"QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					int buf = qdcid;
					qdcid=Decode(fqdc,qdc,qname);
					if(PrintErr){
						cout<<buf-qdcid<<endl;
						cout<<"Fixed : QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					}
				}
				while(qdcid>tdcid and not force_quit){
					if(PrintErr)cout<<"QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					int buf = tdcid;
					tdcid=Decode(ftdc,tdc,tname);
					if(PrintErr){
						cout<<buf-tdcid<<endl;
						cout<<"Fixed : QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					}
				}
			}
		}
#endif
		if(qdcid == -1 or tdcid == -1){
			cout<<"Entries : "<<tree->GetEntries()<<endl;
			cout<<"Refreshing file..."<<endl;
			clearerr(fqdc);
			clearerr(ftdc);
			fclose(fqdc);
			fclose(ftdc);
			fqdc = fopen((filename+".qdc").Data(),"rb");
			ftdc = fopen((filename+".tdc").Data(),"rb");
			cout<<"Reading..."<<endl;
			continue;
		}
		evnum = qdcid;
		if(nev<evnum){
			nev++;
			tree->Fill();
		}
		if(qdcid < -1 or tdcid < -1) {
			break;
		}
	}
	if(evnum == -9999){
		TString comment = Form("Elapsed time = %d ms",qdc[0]);
		cout<<comment<<endl;
		auto tag = new TNamed("Elapsed time",comment.Data());	
		tag->Write();
	}
	cout<<tree->GetEntries()<<endl;
	file->Write();
}

void EventDisplay(TString filename){
	force_quit = false;
	decoding=true;
	auto decode_start = duration_cast<seconds>(system_clock::now().time_since_epoch()); 
	const char* qname = (filename+".qdc").Data();
	const char* tname = (filename+".tdc").Data();
	FILE* fqdc = fopen((filename+".qdc").Data(),"rb");
	FILE* ftdc = fopen((filename+".tdc").Data(),"rb");
	int qdc[16];
	int tdc[16];
	int evnum=0;
	int qdcid=0;
	int tdcid=0;
	TFile* file = new TFile(filename+".root","recreate");
	TTree* tree = new TTree("tree","tree");
	tree->Branch("evnum",&evnum,"evnum/I");
	tree->Branch("qdcid",&qdcid,"qdcid/I");
	tree->Branch("tdcid",&tdcid,"tdcid/I");
	tree->Branch("qdc",&qdc,"qdc[16]/I");
	tree->Branch("tdc",&tdc,"tdc[16]/I");
	TCanvas* c1 = new TCanvas("c1","c1",1200,600);
	c1->Divide(2,2);
	TH1D* h_raw[4];
	TH1D* h_cut[4];
//	TString titles[4] = {"BTOFUL","BTOFUR","BTOFDL","BTOFDR"};
//	int ch[4]={2,3,4,5};
	TString titles[4] = {"SCUL","SCUR","SCDL","SCDR"};
	int ch[4]={8,9,10,11};
	TF1* func[4];
	TF1* fung[4]; TH1D* gain[4];
	for(int i=0;i<4;++i){
		h_raw[i] = new TH1D(titles[i]+"raw",titles[i]+"raw",1000,0,1000); h_cut[i] = new TH1D(titles[i]+"cut",titles[i]+"cut",1000,0,1000);
		h_cut[i]->SetLineColor(kRed);
		func[i] = new TF1(Form("land%d",i),"landau",0,4000);
		fung[i] = new TF1(Form("gaus%d",i),"gaus",0,200);
		c1->cd(i+1);
		h_raw[i]->Draw();
		h_cut[i]->Draw("same");
	}
	int nev = -1;
	while(decoding and not force_quit){
		qdcid = Decode(fqdc,qdc,qname);   
		tdcid = Decode(ftdc,tdc,tname); 
		if(qdcid > -1 and tdcid > -1) {
#if MatchEvent
			while(qdcid!=tdcid and not force_quit){
				while(qdcid<tdcid and not force_quit){
					if( PrintErr) cout<<"QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					int buf = qdcid;
					qdcid=Decode(fqdc,qdc,qname);
					if(PrintErr){
						cout<<buf-qdcid<<endl;
						cout<<"Fixed : QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					}
				}
				while(qdcid>tdcid and not force_quit){
					if(PrintErr)cout<<"QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					int buf = tdcid;
					tdcid=Decode(ftdc,tdc,tname);
					if(PrintErr){
						cout<<buf-tdcid<<endl;
						cout<<"Fixed : QDCID = "<<qdcid<<", TDCID = "<<tdcid<<endl;
					}
				}
			}
#endif	
		}
		if(qdcid == -1 or tdcid == -1){
			cout<<"Entries : "<<tree->GetEntries()<<endl;
			cout<<"Refreshing file..."<<endl;
			clearerr(fqdc);
			clearerr(ftdc);
			fclose(fqdc);
			fclose(ftdc);
			fqdc = fopen((filename+".qdc").Data(),"rb");
			ftdc = fopen((filename+".tdc").Data(),"rb");
			for(int i=0;i<4;++i){
				c1->cd(i+1);
				TString Com = Form("qdc[%d]>>",ch[i]);
				//	TString Cut1 = Form("tdc[%d]<0",ch[i]);
				TString Cut1 = Form("");
				TString Cut2 = Form("tdc[%d]>0",ch[i]);
				//				TString Cut2 = Form("");
				//				tree->Draw(Com+titles[i]+"raw",Cut1);
				//				tree->Draw(Com+titles[i]+"cut",Cut2,"same");
				int ped = h_raw[i]->GetBinCenter(h_raw[i]->GetMaximumBin());
				func[i]->SetRange(ped+20,500);
				h_raw[i]->Fit(Form("land%d",i),"RQ");
				cout<<"gain : "<<i<<" : "<<func[i]->GetParameter(1)-ped<<endl;
			}
			c1->Modified();
			c1->Update();
			gSystem->ProcessEvents();
			sleep(sleeping);
			ftdc;
			fqdc;
			cout<<"Reading..."<<endl;
			continue;
		}
		if(qdcid < -1 or tdcid < -1) {
			for(int i=0;i<4;++i){
				c1->cd(i+1);
				TString Com = Form("qdc[%d]>>",ch[i]);
				//	TString Cut1 = Form("tdc[%d]<0",ch[i]);
				TString Cut1 = Form("");
				//	TString Cut2 = Form("tdc[%d]>0",ch[i]);
				TString Cut2 = Form("");
				tree->Draw(Com+titles[i]+"raw",Cut1);
				tree->Draw(Com+titles[i]+"cut",Cut2,"same");
				int ped = h_raw[i]->GetBinCenter(h_raw[i]->GetMaximumBin());
				h_cut[i]->Fit(Form("land%d",i),"RQ");
				cout<<"gain : "<<i<<" : "<<func[i]->GetParameter(1)-ped<<endl;
			}
			break;
		}
		evnum = qdcid;
		if(nev<evnum){
			nev++;
			tree->Fill();
			for(int i=0;i<4;++i){
				h_raw[i]->Fill(qdc[ch[i]]);
				if(tdc[ch[i]]>0)h_cut[i]->Fill(qdc[ch[i]]);
			}
			if(tdc[2]<0 or tdc[3]<0){
				//			cout<<Form("Ev %d, tdc = %d , %d",evnum,tdc[2],tdc[3])<<endl;
			}
			//		nev=evnum;
		}
	}
	cout<<tree->GetEntries()<<endl;
	file->Write();
	file->Close();
	delete file;
}
void DecodeAll(){
	for(int i = 18; i < 83;++i){
		TString filename = Form("VME_%d",i);
		cout<<filename<<endl;
		DecodeEvent(filename);
	}
}
