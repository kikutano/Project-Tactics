#pragma once

#include <Libs/Resource/Resource.h>

namespace tactics {

class Shader: public Resource<Shader> {
public:
	static const ResourceType TYPE = ResourceType::Shader;
	unsigned int rendererId;
};

}
