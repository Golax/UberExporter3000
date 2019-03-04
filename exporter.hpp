#include <iostream>
#include <exception>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

class UberExporter{
	
	public:
		static fs::path origin;									//percorso per la cartella
		static fs::path destination;                            //percorso di destinazione
		static bool rmemptydirs;     							//opzione per eliminare una cartella vuota
		static uintmax_t weight;                                // quanto pesa la cartella
		static uintmax_t exported;                           	//quanto "peso della cartella" ho già visitato
		static map<string, vector<string>> UberPattern;			//pattern logico
		static vector<string> dirs;								//lista whitelist cartelle
		
		UberExporter(fs::path origin_p, fs::path destination_p, bool rmemptydirs_p);
		~UberExporter(){};

		int uberExport();

	private:
		optional<string> uberFind(string ext);
};