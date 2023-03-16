#include "Utils.hh"
void Test(){
	ifstream f;
	f.open("TestParam.txt");
	double data[100];
	for(int i=0;i<5;++i){
		if(!ReadConfLine(f,data)) {
			cout<<i<<"th ends"<<endl;
			continue;
		}
		cout<<i<<" th"<<endl;
		for(int j=0;j<4;++j){
			cout<<data[j]<<endl;
		}
	}
	
}
