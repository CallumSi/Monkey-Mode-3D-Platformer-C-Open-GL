#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a triangular_prism object with a mesh of a specified size
	class triangular_prism
	{
	public:
		/// \brief Constructor
		triangular_prism(std::vector<glm::vec3> vertices);

		/// \brief Destructor
		~triangular_prism();

		/// \brief Getter methods
		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<triangular_prism> create(std::vector<glm::vec3> vertices);
	private:
		/// \brief Fields
		// vertices of the triangular_prism specified by a vector of positions.
		std::vector<glm::vec3> m_vertices;

		ref<engine::mesh> m_mesh;
	};
}
