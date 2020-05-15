#ifndef SHADER_COMPILER_H_
#define SHADER_COMPILER_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <shaderc/shaderc.hpp>

class Compiler
{
public:
	static bool buildShader(std::vector<uint32_t>& spirv, const std::string& source, const std::map<std::string, std::string>& macros, shaderc_shader_kind shaderKind, shaderc_optimization_level optimizationLevel = shaderc_optimization_level_zero);
};

#endif /* SHADER_COMPILER_H_ */
