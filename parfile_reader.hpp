#ifndef PARFILE_READER_HPP
#define PARFILE_READER_HPP

#include <string>
#include <map>
#include <sstream>

class ParfileReader {
public:
	ParfileReader( std::string filename ) {
		std::ifstream infile(filename);
		if( !infile ) {
			std::cerr << "Error reading file " << filename << "\n";
			exit(1);
		}
		std::string line;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			std::string key;
			std::string value;
			if( !(iss >> key >> value)) {
				std::cerr << "Error reading file " << filename << "\n";
				exit(1);
			}
			params.insert( {key, value} );
		}
	}

	int getInt( std::string key ) {
		return std::stoi( lookUp(key) );
	}

	double getDouble( std::string key ) {
		return std::stod( lookUp(key) );
	}

	std::string getString( std::string key ) {
		return lookUp(key);
	}
private:

	std::string lookUp(const std::string& key) {
		auto it = params.find(key);
		if( it == params.end() ) {
			std::cout << "Key " << key << " not found\n";
			return "";
		}
		return it->second;
	}
	std::map<std::string, std::string> params;
};


#endif // PARFILE_READER_HPP
