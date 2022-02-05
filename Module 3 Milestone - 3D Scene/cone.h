#pragma once
#include "common/staticMesh3D.h"

namespace static_meshes_3D {

	/**
	* Cone static mesh with given radius, number of slices and height.
	*/
	class Cone : public StaticMesh3D
	{
	public:
		Cone(float radius, int numSlices, float height,
			bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);

		void render() const override;
		void renderPoints() const override;

		/**
		 * Gets cone radius.
		 */
		float getRadius() const;

		/**
		 * Gets number of cone slices.
		 */
		int getSlices() const;

		/**
		 * Gets cone height.
		 */
		float getHeight() const;

	private:
		float _radius; // cone radius (distance from the center of cone to surface)
		int _numSlices; // Number of cone slices
		float _height; // Height of the cone

		int _numVerticesSide; // How many vertices to render side of the cone
		int _numVerticesTopBottom; // How many vertices to render top / bottom of the cone
		int _numVerticesTotal; // Just a sum of both numbers above

		void initializeData() override;
	};

} // namespace static_meshes_3D