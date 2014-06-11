#include "parfile_reader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

ParfileReader::ParfileReader( string filename ) {
	ifstream infile(filename);
	if( !infile ) {
		cerr << "Error reading file " << filename << "\n";
		exit(1);
	}
	string line;
	while (getline(infile, line)) {
		stringstream iss(line);
		string key;
		string value;
		if( !(iss >> key >> value)) {
			cerr << "Error reading file " << filename << "\n";
			exit(1);
		}
		params.insert( {key, value} );
	}
}

int ParfileReader::getInt( string key ) {
	return stoi( lookUp(key) );
}

double ParfileReader::getDouble( string key ) {
	return stod( lookUp(key) );
}

void ParfileReader::initDoubleList( vector<pair<string, double&>> pairs) {
	for( auto& it: pairs) {
		it.second = this->getDouble(it.first);
	}
}

string ParfileReader::getString( string key ) {
	return lookUp(key);
}

string ParfileReader::lookUp(const string& key) {
	auto it = params.find(key);
	if( it == params.end() ) {
		cout << "Key " << key << " not found\n";
		return "";
	}
	return it->second;
}
