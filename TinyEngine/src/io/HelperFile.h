#ifndef IO_HELPERFILE_H_
#define IO_HELPERFILE_H_

#include <string>

class HelperFile {
public:

	static std::string getPath(const std::string& filename);

	static std::string getExtension(const std::string& filename);
};

#endif /* IO_HELPERFILE_H_ */
