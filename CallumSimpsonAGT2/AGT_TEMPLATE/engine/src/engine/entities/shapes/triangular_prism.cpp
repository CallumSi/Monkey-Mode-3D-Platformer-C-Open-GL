#include "pch.h"
#include "triangular_prism.h"
#include <engine.h>		

engine::triangular_prism::triangular_prism(std::vector<glm::vec3> vertices) : m_vertices(vertices)
{
	std::vector<glm::vec3> normals;
	// front normal
	normals.push_back(glm::cross(vertices.at(0) - vertices.at(2), vertices.at(0) - vertices.at(1)));
	// left normal
	normals.push_back(glm::cross(vertices.at(2) - vertices.at(0), vertices.at(2) - vertices.at(5)));
	// right normal
	normals.push_back(glm::cross(vertices.at(1) - vertices.at(4), vertices.at(1) - vertices.at(0)));
	// bottom normal
	normals.push_back(glm::cross(vertices.at(2) - vertices.at(1), vertices.at(2) - vertices.at(5)));
	// back normal
	normals.push_back(glm::cross(vertices.at(1) - vertices.at(2), vertices.at(1) - vertices.at(4)));
	std::vector<mesh::vertex> triangular_prism_vertices
	{
		//front 
		//  position			normal			      tex coord       
		{ vertices.at(0),		normals.at(0),		{ 0.f,  0.f } },
		{ vertices.at(2),		normals.at(0),		{ 1.f,  0.f } },
		{ vertices.at(1),		normals.at(0),		{ 0.5f, 1.f } },

		//left 
		{ vertices.at(0),		normals.at(1),		{ 0.f,  0.f } },
		{ vertices.at(2),		normals.at(1),		{ 1.f,  0.f } },
		{ vertices.at(5),		normals.at(1),		{ 1.f, 1.f } },
		{ vertices.at(3),		normals.at(1),		{ 0.f, 1.f } },

		//right
		{ vertices.at(0),		normals.at(2),		{ 0.f,  0.f } },
		{ vertices.at(1),		normals.at(2),		{ 1.f,  0.f } },
		{ vertices.at(4),		normals.at(2),		{ 1.f, 1.f } },
		{ vertices.at(3),		normals.at(2),		{ 0.f, 1.f } },

		//bottom
		{ vertices.at(1),		normals.at(3),		{ 0.f,  0.f } },
		{ vertices.at(2),		normals.at(3),		{ 1.f,  0.f } },
		{ vertices.at(5),		normals.at(3),		{ 1.f, 1.f } },
		{ vertices.at(4),		normals.at(3),		{ 0.f, 1.f } },

		//back 
		{ vertices.at(3),		normals.at(4),		{ 0.f,  0.f } },
		{ vertices.at(5),		normals.at(4),		{ 1.f,  0.f } },
		{ vertices.at(4),		normals.at(4),		{ 0.5f, 1.f } },

	};

	const std::vector<uint32_t> triangular_prism_indices
	{
		0,  1,  2,						//front
		3,  5,  4,       3,   6,   5,	//left 
		7,	8,	9,       7,   9,  10,	//Right
		11,	12,	13,     11,  13,  14,	//Bottom
		15,	17,	16						//Back
	};

	m_mesh = engine::mesh::create(triangular_prism_vertices, triangular_prism_indices);
}

engine::triangular_prism::~triangular_prism() {}

engine::ref<engine::triangular_prism> engine::triangular_prism::create(std::vector<glm::vec3> vertices)
{
	return std::make_shared<engine::triangular_prism>(vertices);
}
