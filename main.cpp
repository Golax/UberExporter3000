#include "exporter.hpp"
#include <ctime>

int main(int argc, char *argv[]) {
	
	void check(int argc, char *argv[]);

	//check arguments
	try {
		check(argc, argv);
	} catch (const char* msg) {
		cout << endl << msg << endl << endl;
		cout << "Syntax: UberExporter3000 [path:from] [path:to] [-options]" << endl << endl;
		cout << "For more info type: UberExporter3000 --help";
		return 0;	
	}

	//getting the time and creating destination root folder
	time_t t = time(0);   // get time now
	tm* now = localtime(&t);
	fs::path source_root = argv[1];
	fs::path destination_root = argv[2];
	destination_root /= string("UberExported ") + to_string(now->tm_mday) + "-" + to_string(now->tm_mon+1) + "-" + to_string(now->tm_year+1900);
	fs::create_directories(destination_root);
	cout << ">>	root destination created!" << endl;

	bool rmemptydirs = (argc == 4 && string(argv[3]) == "-rmemptydirs");
	
	//creating the Exporter object
	UberExporter uberexporter (source_root, destination_root, rmemptydirs);

	//ExporTime
	cout <<  uberexporter.uberExport() / 1024 << "KB Succesfully Exported!" << endl;
	return 1;
}


//function checks argument and if the destination not exists it creates it
void check(int argc, char *argv[]){
	if(argc == 2 && string(argv[1]) == "--options")
		throw "-rmemptydirs: remove empty directories if found";
	if(argc == 2 && string(argv[1]) == "--version")
		throw "UberExporter3000 Alpha 1.0 https://github.com/Golax";
	if(argc == 2 && argv[1])
	if(argc < 3)
		throw "Too few arguments";
	if(argc > 4)
		throw "Too many arguments";
	if(argc == 4 && string(argv[3]) != "-rmemptydirs")
		throw "Invalid option";
	if(string(argv[1]) == string(argv[2]))
		throw "Invalid destination";
	if(!fs::exists(argv[1]))
		throw "Filesystem not found";
	if(!fs::exists(argv[2]))
		fs::create_directories(argv[2]);
}