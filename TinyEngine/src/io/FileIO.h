#ifndef IO_FILEIO_H_
#define IO_FILEIO_H_

#include <cstdint>
#include <string>
#include <vector>

class FileIO
{
public:

	static bool open(std::string& output, const std::string& filename);

};

#endif /* IO_FILEIO_H_ */
