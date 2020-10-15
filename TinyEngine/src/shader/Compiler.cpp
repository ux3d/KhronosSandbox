#include "Compiler.h"

#include "../common/Common.h"

bool Compiler::buildShader(std::vector<uint32_t>& spirv, const std::string& source, const std::map<std::string, std::string>& macros, shaderc_shader_kind shaderKind, shaderc_optimization_level optimizationLevel)
{
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  for (const auto& it : macros)
  {
	  if (it.second == "")
	  {
		  options.AddMacroDefinition(it.first);
	  }
	  else
	  {
		  options.AddMacroDefinition(it.first, it.second);
	  }
  }
  options.SetOptimizationLevel(optimizationLevel);

  auto result = compiler.CompileGlslToSpv(source, shaderKind, "", options);
  if (result.GetCompilationStatus() != shaderc_compilation_status_success)
  {
	  Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "Compiler error:\n%s", result.GetErrorMessage().c_str());

	  return false;
  }

  spirv = {result.cbegin(), result.cend()};

  return true;
}
