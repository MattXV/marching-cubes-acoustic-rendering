#pragma once

#include "../rendering/RenderTools.h"
#include <memory>
#include <vector>

namespace unda {
	class BoundingBox : public unda::AABB, public unda::Transform {
	public:
		BoundingBox(const unda::AABB& aabb);
	private:
		size_t nFaces = 6, nTrianglesPerFace = 2;
		size_t toLinearVertexIndex(const std::array<size_t, 3>& ijk) {
			return ijk[0]; 
		}
		std::array<Vertex, 36> vertices;
	};


	class IBoundingBoxRenderer
	{
	public:

		virtual void render() = 0;
		virtual void cleanUp() = 0;

		std::vector<std::unique_ptr<BoundingBox>>& getBoundingBoxes() { return boundingBoxes; }
		const std::vector<std::unique_ptr<BoundingBox>>& getBoundingBoxes() const { return boundingBoxes; }
	private:
		std::vector<std::unique_ptr<BoundingBox>> boundingBoxes;
	};
}