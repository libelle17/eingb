#include <iostream>
#include <string>
#include <memory>
#include <vector>
using namespace std;
#include <stdio.h>
struct zcl
{
	int nr;
	string name;
	zcl(int nr,string name);
	zcl(){cout<<"Standarderstellung\n"<<endl;}
	zcl(const zcl& q):nr(q.nr),name(q.name){cout<<"copy contructor\n";}
	~zcl(){cout<<"dekonstruiere\n";};
};
zcl::zcl(int nr, string pname):nr(nr),name(string(200,' '))
{
	sprintf(&name[0],"Name: %s: %i",pname.c_str(),nr);
	name.resize(name.find('\0'));
}

int main()
{
	zcl zeile[]{{1,"Eins"},{2,"Zwei"}};
	cout<<" nach Konstruktion"<<endl;
	size_t zgr{sizeof(zeile)/sizeof(*zeile)};
	cout<<" nach Groessenbestimmung"<<endl;
	for(auto i=0;i<zgr;i++) {
		cout<<zeile[i].nr<<" "<<zeile[i].name<<endl;
	}
	cout<<" nach Ausgabe Struktur"<<endl;
//	vector<zcl*>::iterator beg{(zcl**)zeile};
//	vector<zcl*>::iterator end{(zcl**)(zeile+sizeof(zeile)/sizeof(*zeile)-1)};
//	vector<zcl*> zv1{beg,end}; // trickst den Compiler aus und erstellt etwas anderes
	
	if (1) {
		vector<zcl*> zv2;
		cout<<"vor Befuellung zv2"<<endl;
		//	for(auto i:zeile) zv2.push_back(&i); // konsturiert, dekonstruiert mehrfach und gibt zuletzt 2 x das Element 2 aus
		for(int i=0;i<sizeof(zeile)/sizeof(*zeile);i++) zv2.push_back(&zeile[i]);
		cout<<"nach Befuellung zv2"<<endl;
		for(int i=0;i<zv2.size();i++) {
			cout<<zv2[i]->nr<<" "<<zv2[i]->name<<endl;
		}
		cout<<"nach Ausgabe 1"<<endl;
		for(auto i:zv2) {
			cout<<i->nr<<" "<<i->name<<endl;
		}
		cout<<"nach Ausgabe 2"<<endl;
	}
	cout<<"nach Gueltigkeit zv2"<<endl;
	if (0) {
		cout<<"vor vector"<<endl;
		vector<shared_ptr<zcl>> zv; // {zeile,zeile+sizeof(zeile)/sizeof(*zeile)};
		zv.push_back(shared_ptr<zcl>(&zeile[0]));
		cout<<"nach vector"<<endl;
	}
}
