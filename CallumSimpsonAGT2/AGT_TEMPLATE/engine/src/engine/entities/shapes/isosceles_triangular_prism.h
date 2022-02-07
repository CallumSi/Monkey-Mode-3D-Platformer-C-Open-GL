#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a isosceles_triangular_prism object with a mesh of a specified size
	class isosceles_triangular_prism
	{
	public:
		/// \brief Constructor
		isosceles_triangular_prism(std::vector<glm::vec3> vertices);

		/// \brief Destructor
		~isosceles_triangular_prism();

		/// \brief Getter methods
		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<isosceles_triangular_prism> create(std::vector<glm::vec3> vertices);
	private:
		/// \brief Fields
		// vertices of the isosceles_triangular_prism specified by a vector of positions.
		std::vector<glm::vec3> m_vertices;

		ref<engine::mesh> m_mesh;
	};
}
