void Write(){
	
	double xpos[7] = {-50,-40,-20,0,20,40,50};
	double t1peak[7] = {0.541017  ,0.334399 ,0.0267804,-0.0148132,-0.181149 ,-0.272138 ,0.341807 };
	double t1width[7] ={0.203481  ,0.206105 ,0.179149 ,0.184505  ,0.164844  ,0.208446  ,0.249715 };
	double t2peak[7] = {-0.0723448,-0.105811,0.0360269,-0.0057214,-0.19526  ,0.69839   ,0.6996   };
	double t2width[7] ={0.227957  ,0.197349 ,0.19772  ,0.190695  ,0.185354  ,0.165576  ,0.151986 };
	double tdpeak[7] = {-0.60538  ,-0.442321,0.0064409,0.0102057 ,-0.0193351,0.975521  ,0.331371 };
	double tdwidth[7] ={0.217259  ,0.214804 ,0.194268 ,0.173525  ,0.180311  ,0.235628  ,0.21547  };
//	fstream f;
//	f.open("Params/TimeParam.txt",fstream::out);
//	f<<"#pos[ch]	t1	t1sig	t2	t2sig	t2-t1	t2-t1sig"<<endl;
	for(int i=0;i<7;++i){
		TString pars = Form("%f, %f, %f, %f, %f, %f, %f",
				xpos[i],t1peak[i],t1width[i],
				t2peak[i],t2width[i],
				tdpeak[i],tdwidth[i]
				);
//		f<<	pars.Data()<<endl;
	}
}
