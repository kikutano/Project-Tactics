#pragma once

#include "Shader.h"

#include <Libs/Resource/ResourceLoader.h>

#include <tuple>

namespace tactics::resource {

struct ShaderDescriptor {
	std::string vertexShader;
	std::string fragmentShader;

	JSON_SERIALIZE(ShaderDescriptor, vertexShader, fragmentShader);
};

class ShaderLoader : public ResourceLoader {
public:
	using ResourceLoader::ResourceLoader;
	[[nodiscard]] std::shared_ptr<Shader> load(const ShaderDescriptor& descriptor);

private:
	std::tuple<unsigned int, std::string, std::string> _loadProgram(const std::string& vertexShaderFilePath,
																	const std::string& fragmentShaderFilePath);
	std::tuple<unsigned int, std::string> _loadShader(unsigned int shaderType, const std::string& shaderFilePath);
};

} // namespace tactics::resource
