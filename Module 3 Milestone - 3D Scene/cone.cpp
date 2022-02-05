// STL
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Project
#include "cone.h"

namespace static_meshes_3D {

	Cone::Cone(float radius, int numSlices, float height, bool withPositions, bool withTextureCoordinates, bool withNormals)
		: StaticMesh3D(withPositions, withTextureCoordinates, withNormals)
		, _radius(radius)
		, _numSlices(numSlices)
		, _height(height)
	{
		initializeData();
	}

	float Cone::getRadius() const
	{
		return _radius;
	}

	int Cone::getSlices() const
	{
		return _numSlices;
	}

	float Cone::getHeight() const
	{
		return _height;
	}

	void Cone::initializeData()
	{
		if (_isInitialized) {
			return;
		}

		// Calculate and cache numbers of vertices
		_numVerticesSide = (_numSlices + 1) * 2;
		_numVerticesTopBottom = _numSlices + 2;
		_numVerticesTotal = _numVerticesSide + _numVerticesTopBottom * 2;

		// Generate VAO and VBO for vertex attributes
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		_vbo.createVBO(getVertexByteSize() * _numVerticesTotal);

		// Pre-calculate sines / cosines for given number of slices
		const auto sliceAngleStep = 2.0f * glm::pi<float>() / float(_numSlices);
		auto currentSliceAngle = 0.0f;
		std::vector<float> sines, cosines;
		for (auto i = 0; i <= _numSlices; i++)
		{
			sines.push_back(sin(currentSliceAngle));
			cosines.push_back(cos(currentSliceAngle));

			// Update slice angle
			currentSliceAngle += sliceAngleStep;
		}

		if (hasPositions())
		{
			// Pre-calculate X and Z coordinates
			std::vector<float> x;
			std::vector<float> z;
			for (auto i = 0; i <= _numSlices; i++)
			{
				x.push_back(cosines[i] * _radius);
				z.push_back(sines[i] * _radius);
			}

			// Add cone side vertices
			for (auto i = 0; i <= _numSlices; i++)
			{
				const auto topPosition = glm::vec3(0, _height / 2.0f, 0);
				const auto bottomPosition = glm::vec3(x[i], -_height / 2.0f, z[i]);
				_vbo.addRawData(&topPosition, sizeof(glm::vec3));
				_vbo.addRawData(&bottomPosition, sizeof(glm::vec3));
			}

			// Add top cone cover
			glm::vec3 topCenterPosition(0.0f, _height / 2.0f, 0.0f);
			_vbo.addRawData(&topCenterPosition, sizeof(glm::vec3));
			for (auto i = 0; i <= _numSlices; i++)
			{
				const auto topPosition = glm::vec3(0, _height / 2.0f, 0);

				_vbo.addRawData(&topPosition, sizeof(glm::vec3));
			}

			// Add bottom cone cover
			glm::vec3 bottomCenterPosition(0.0f, -_height / 2.0f, 0.0f);
			_vbo.addRawData(&bottomCenterPosition, sizeof(glm::vec3));
			for (auto i = 0; i <= _numSlices; i++)
			{
				const auto bottomPosition = glm::vec3(x[i], -_height / 2.0f, -z[i]);
				_vbo.addRawData(&bottomPosition, sizeof(glm::vec3));
			}
		}

		if (hasTextureCoordinates())
		{
			// Pre-calculate step size in texture coordinate U
			// I have decided to map the texture twice around cone, looks fine
			const auto sliceTextureStepU = 2.0f / float(_numSlices);

			auto currentSliceTexCoordU = 0.0f;
			for (auto i = 0; i <= _numSlices; i++)
			{
				_vbo.addData(glm::vec2(currentSliceTexCoordU, 1.0f));
				_vbo.addData(glm::vec2(currentSliceTexCoordU, 0.0f));

				// Update texture coordinate of current slice 
				currentSliceTexCoordU += sliceTextureStepU;
			}

			// Generate circle texture coordinates for cone top cover
			glm::vec2 topBottomCenterTexCoord(0.5f, 0.5f);
			_vbo.addData(topBottomCenterTexCoord);
			for (auto i = 0; i <= _numSlices; i++) {
				_vbo.addData(glm::vec2(topBottomCenterTexCoord.x + sines[i] * 0.5f, topBottomCenterTexCoord.y + cosines[i] * 0.5f));
			}

			// Generate circle texture coordinates for cone bottom cover
			_vbo.addData(topBottomCenterTexCoord);
			for (auto i = 0; i <= _numSlices; i++) {
				_vbo.addData(glm::vec2(topBottomCenterTexCoord.x + sines[i] * 0.5f, topBottomCenterTexCoord.y - cosines[i] * 0.5f));
			}
		}

		if (hasNormals())
		{
			for (auto i = 0; i <= _numSlices; i++) {
				_vbo.addData(glm::vec3(cosines[i], 0.0f, sines[i]), 2);
			}

			// Add normal for every vertex of cone top cover
			_vbo.addData(glm::vec3(0.0f, 1.0f, 0.0f), _numVerticesTopBottom);

			// Add normal for every vertex of cone bottom cover
			_vbo.addData(glm::vec3(0.0f, -1.0f, 0.0f), _numVerticesTopBottom);
		}

		// Finally upload data to the GPU
		_vbo.bindVBO();
		_vbo.uploadDataToGPU(GL_STATIC_DRAW);
		setVertexAttributesPointers(_numVerticesTotal);

		_isInitialized = true;
	}

	void Cone::render() const
	{
		if (!_isInitialized) {
			return;
		}

		glBindVertexArray(_vao);

		// Render cone side first
		glDrawArrays(GL_TRIANGLE_STRIP, 0, _numVerticesSide);

		// Render top cover
		glDrawArrays(GL_TRIANGLE_FAN, _numVerticesSide, _numVerticesTopBottom);

		// Render bottom cover
		glDrawArrays(GL_TRIANGLE_FAN, _numVerticesSide + _numVerticesTopBottom, _numVerticesTopBottom);
	}

	void Cone::renderPoints() const
	{
		if (!_isInitialized) {
			return;
		}

		// Just render all points as they are stored in the VBO
		glBindVertexArray(_vao);
		glDrawArrays(GL_POINTS, 0, _numVerticesTotal);
	}

} // namespace static_meshes_3D