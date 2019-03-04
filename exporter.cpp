#include "exporter.hpp"

fs::path UberExporter::destination;
fs::path UberExporter::origin;
uintmax_t UberExporter::weight;
uintmax_t UberExporter::exported;
vector<string> UberExporter::dirs;
map<string, vector<string>> UberExporter::UberPattern;
bool UberExporter::rmemptydirs;


//inizializza il pattern
map<string, vector<string>> init_pattern(){
	ifstream pattern_file ("pattern.uber");
	string key;
	string values;
	int pos = 0;
	map<string, vector<string>> UberPattern;
	while(getline(pattern_file, key) && getline(pattern_file, values)){	//prendo la chiave(cartella) e i rispettivi valori(estensioni) 
		while((pos = values.find_last_not_of(' ')) != string::npos){	//finche trova una estensione (delimitate dallo spazio)
			UberPattern[key].push_back(values.substr(0 , pos));			//la aggiungo all'elenco
			values.erase(0, pos+1);										//cancello la prima estensione dalla stringa
		}
	}
	return UberPattern;
}

//inizializza le cartelle su cui NON lavorare
vector<string> init_dirs(){
	string t_dir;
	vector<string> dirs;
	ifstream list ("dirs.uber");
	while(getline(list, t_dir))
		if(t_dir.at(0) != '#')	//elimino i commenti
			dirs.push_back(t_dir);
	return dirs;
}

//constructor
UberExporter::UberExporter(fs::path origin_p, fs::path destination_p, bool rmemptydirs_p){
	origin = origin_p;
	destination = destination_p;
	rmemptydirs = rmemptydirs_p;
	UberPattern = init_pattern();
	dirs = init_dirs();
	exported = 0;
	weight = fs::space(origin).capacity-fs::space(origin).available;
	cout << "Dimension of filesystem: " << weight / 1024 << "KB" << endl;
}

//search UberPattern map for a value and return key
optional<string> UberExporter::uberFind(string ext){
	for(auto row : UberPattern){
		if(find(row.second.begin(), row.second.end(), ext) != row.second.end())
			return row.first;
	}
	return nullopt;
}

//exportfile to destination AKA copy and delete
void uberCut(fs::path file, string directory){
	//TODO: rivedere questa parte
	//il problema è che windows ha i file CRLF mentre linuc LF e getline si ferma al LF ma tiene il CR
	if(directory.back() == '\r')	
		directory = directory.substr(0, directory.length()-1);
	fs::path new_p = UberExporter::destination / directory;
	if(!fs::exists(new_p))
        fs::create_directories(new_p);
	fs::copy(file, new_p);
    fs::remove(file);
}

//exporTime
int UberExporter::uberExport(){
	optional<string> key;	
	fs::path temp;
    fs::recursive_directory_iterator item (origin);
    while(item != fs::recursive_directory_iterator()){
		cout << item->path() << endl;
		if(fs::is_regular_file(item->path())){                       					//se trovo un file
			if((key = uberFind(item->path().extension().string())) && key != nullopt){	//se l'estensione è contenuta nel pattern
				exported += fs::file_size(item->path());           						//aggiorno la quantità esportata
				temp = item->path();
                ++item;
				uberCut(temp, key.value());
                cout << ">> " << temp << " Exported!" << endl;
            } else ++item;
        }
        else if(fs::is_directory(item->path())){    //se è una cartella
            if(find(dirs.begin(), dirs.end(), item->path().string()) != dirs.end()){   //se il percorso della cartella è contenuto nella mia lista di cartelle
				item.disable_recursion_pending();	//la salto
				++item;
            }
            else if(rmemptydirs){ //se è stata passata l'opzione
                if(fs::is_empty(item->path())){ //se la cartella non contiene elementi
                    temp = item->path();
                    ++item;
                    fs::remove(temp);
                    cout << ">> " << temp << " Removed!" << endl;
                }
			} else ++item;
        } else ++item;
    }
	return exported;
}