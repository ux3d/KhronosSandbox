#include "../shader/HelperCompiler.h"

std::string HelperCompiler::getTexCoord(uint32_t texCoord)
{
	return "in_texCoord" + std::to_string(texCoord);
}
