#ifndef PARFILE_READER_HPP
#define PARFILE_READER_HPP

#include <string>
#include <map>
#include <vector>

class ParfileReader {
public:
	ParfileReader( std::string filename );

	int getInt( std::string key );
	double getDouble( std::string key );
	std::string getString( std::string key );

	void initDoubleList( std::vector<std::pair< std::string, double&>> pairs);

private:

	std::string lookUp(const std::string& key);
	std::map<std::string, std::string> params;
};


#endif // PARFILE_READER_HPP
