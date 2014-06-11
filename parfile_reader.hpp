#ifndef PARFILE_READER_HPP
#define PARFILE_READER_HPP

#include <string>
#include <map>
#include <sstream>

class ParfileReader {
public:
	ParfileReader( std::string filename );

	int getInt( std::string key );
	double getDouble( std::string key );
	std::string getString( std::string key );

private:

	std::string lookUp(const std::string& key);
	std::map<std::string, std::string> params;
};


#endif // PARFILE_READER_HPP
