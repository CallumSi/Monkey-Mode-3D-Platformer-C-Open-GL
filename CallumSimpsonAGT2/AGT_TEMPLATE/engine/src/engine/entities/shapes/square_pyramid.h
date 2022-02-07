#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a square_pyramid object with a mesh of a specified size
	class square_pyramid
	{
	public:
		/// \brief Constructor
		square_pyramid(std::vector<glm::vec3> vertices);

		/// \brief Destructor
		~square_pyramid();

		/// \brief Getter methods
		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<square_pyramid> create(std::vector<glm::vec3> vertices);
	private:
		/// \brief Fields
		// vertices of the square_pyramid specified by a vector of positions.
		std::vector<glm::vec3> m_vertices;

		ref<engine::mesh> m_mesh;
	};
}
