#ifndef IO_FILEIO_H_
#define IO_FILEIO_H_

#include <string>

class FileIO
{
public:

	static std::string getPath(const std::string& filename);

	static std::string getExtension(const std::string& filename);

	static bool openText(std::string& output, const std::string& filename);
};

#endif /* IO_FILEIO_H_ */
