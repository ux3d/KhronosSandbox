#include "HelperShader.h"

std::string HelperShader::getTexCoord(uint32_t texCoord)
{
	return "in_texCoord" + std::to_string(texCoord);
}
