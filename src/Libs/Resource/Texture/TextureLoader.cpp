#include "TextureLoader.h"

#include <Libs/FileSystem/FileSystem.h>
#include <Libs/Utility/Exception.h>

#define STB_IMAGE_IMPLEMENTATION
#include <glad/gl.h>
#include <stb/stb_image.h>

namespace tactics::resource {

std::shared_ptr<Texture> TextureLoader::load(const HashId& name, const TextureDescriptor& descriptor) {
	auto texture = _loadTexture(name,
								// TODO(Gerark) FileSystem: We should have more control on how we load the textures
								// through filesystem and not relying on stb to do the dirty job we should be able to
								// load by providing a buffer like for stbi_load_from_memory
								_getFileSystem().getPathHelper().makeAbsolutePath(descriptor.path),
								descriptor);
	return texture;
}

std::shared_ptr<Texture>
TextureLoader::_loadTexture(const HashId& name, const std::string& filename, const TextureDescriptor& descriptor) {
	if (filename.empty()) {
		TACTICS_EXCEPTION("Can't load texture. Filename is empty while trying to load texture [{}]", name);
	}

	if (!std::filesystem::exists(filename)) {
		TACTICS_EXCEPTION("Can't load texture. File [{}] does not exist", filename);
	}

	auto texture = std::make_shared<Texture>(name);
	texture->info.useTransparency = descriptor.useTransparency;
	texture->info.filter = descriptor.filter;

	auto& info = texture->info;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* textureData = stbi_load(filename.data(), &info.width, &info.height, &info.channelsCount, 0);

	glGenTextures(1, &texture->rendererId);
	glBindTexture(GL_TEXTURE_2D, texture->rendererId);

	glTexParameteri(GL_TEXTURE_2D,
					GL_TEXTURE_MIN_FILTER,
					texture->info.filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,
					GL_TEXTURE_MAG_FILTER,
					texture->info.filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA8,
				 info.width,
				 info.height,
				 0,
				 info.channelsCount > 3 ? GL_RGBA : GL_RGB,
				 GL_UNSIGNED_BYTE,
				 textureData);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(textureData);

	return texture;
}

} // namespace tactics::resource
