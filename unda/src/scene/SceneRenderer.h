#pragma once

#include "../rendering/Texture.h"
#include <memory>
#include <vector>

namespace unda {
	class IBoundingBox : public unda::Transform, public AABB {
	public:
		IBoundingBox(const AABB& aabb) : AABB(aabb) {}
		virtual const std::array<Vertex, 36>& getVertices() = 0;
		virtual unsigned int getTextureLocation() = 0;
		virtual void doPatch(TexturePatch& patch, CubeMap::Face face) = 0;

	private:
	};


	class IBoundingBoxRenderer
	{
	public:

		virtual void render() = 0;
		virtual void cleanUp() = 0;

		//std::vector<std::unique_ptr<IBoundingBox>>& getBoundingBoxes() { return boundingBoxes; }
		static std::vector<std::unique_ptr<IBoundingBox>>& getBoundingBoxes() { return boundingBoxes; }
	private:
		static std::vector<std::unique_ptr<IBoundingBox>> boundingBoxes;
	};
}