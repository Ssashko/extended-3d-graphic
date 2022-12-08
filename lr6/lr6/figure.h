#pragma once

#include <map>
#include <vector>
#include <string>
#include <fstream>

#include <glm.hpp>

inline uint32_t float_comp(const float v1, const float v2) {
	const float TOL = 1e-6;
	if (glm::abs(v1 - v2) > TOL)
	{
		if (v1 < v2)
			return 1;
		return -1;
	}
	return 0;
}
struct Comparator
{
	template<typename T>
	bool operator() (const T& v1, const T& v2) const {
		uint32_t val;
		if ((val = float_comp(v1.x, v2.x)) != 0)
			return val == 1 ? true : false;
		if ((val = float_comp(v1.y, v2.y)) != 0)
			return val == 1 ? true : false;
		if ((val = float_comp(v1.z, v2.z)) != 0)
			return val == 1 ? true : false;
		return false;
	};
};

class Mesh {
public:
	virtual const std::vector<glm::vec3>& getVertices() const = 0;
	virtual const std::vector<glm::vec3>& getNormals() const = 0;
	virtual const std::vector<uint32_t>& getIndexes() const = 0;
	virtual const std::vector<glm::vec3>& getFaceNormals() const = 0;
};
class Icosaedr : public Mesh {
private:
	std::vector<glm::vec3> vertex;
	std::vector<uint32_t> index;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> normalFace;
public:
	Icosaedr()
	{
		reinit();
		initSmoothNormal();

	}
	void reinit() {
		reinit_vertex();
		reinit_index();
	}

	void initSmoothNormal() {

		normal = std::vector<glm::vec3>();

		std::vector<std::vector<uint32_t>> vertex_to_triangle_links(vertex.size());
		normalFace = std::vector<glm::vec3>(index.size() / 3);

		for (int i = 0; i < normalFace.size(); i++)
		{
			glm::vec3 points[3] = {
				vertex[index[i * 3]],
				vertex[index[i * 3 + 1]],
				vertex[index[i * 3 + 2]],
			};
			glm::vec3 vect1 = points[2] - points[0];
			glm::vec3 vect2 = points[1] - points[0];
			normalFace[i] = -glm::normalize(glm::cross(vect1, vect2));

		}


		for (uint32_t i = 0; i < index.size(); i++)
			vertex_to_triangle_links[index[i]].push_back(i - i % 3);


		for (int i = 0; i < vertex_to_triangle_links.size(); i++)
		{
			glm::vec3 sum(0.0f);
			for (auto curr_index : vertex_to_triangle_links[i])
				sum += normalFace[curr_index / 3];

			normal.push_back(sum / glm::vec3(vertex_to_triangle_links[i].size()));

		}

	}

	void increaseApproximation(int count_passes = 1)
	{

		for (uint32_t k = 0; k < count_passes; k++)
		{
			std::map<glm::vec3, uint32_t, Comparator> edge_points;
			uint32_t initial_index_count = index.size();
			for (int i = 0; i < initial_index_count; i += 3)
				subdivideTriangle(i, edge_points);
			//assert(edge_points.size() == 0, "should be 0");
		}
		initSmoothNormal();
	}

	const std::vector<glm::vec3>& getVertices() const override {
		return vertex;
	}
	const std::vector<glm::vec3>& getNormals() const override {
		return normal;
	}
	const std::vector<uint32_t>& getIndexes() const override {
		return index;
	}

	const std::vector<glm::vec3>& getFaceNormals() const {
		return normalFace;
	}
private:
	void subdivideTriangle(uint32_t ind_triangle, std::map<glm::vec3, uint32_t, Comparator>& edge_points) {
		assert(ind_triangle % 3 == 0, "The index must be a multiple of 3");
		uint32_t old_indexes[3] = { index[ind_triangle], index[ind_triangle + 1], index[ind_triangle + 2] };
		glm::vec3 tringle[3] = {
			vertex[old_indexes[0]],
			vertex[old_indexes[1]],
			vertex[old_indexes[2]]
		};
		auto projToSphere = [](const glm::vec3& v) {
			float t = (glm::sqrt(5) / 2) / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

			return glm::vec3(v.x * t, v.y * t, v.z * t);
		};

		uint32_t new_vert_ind = vertex.size();

		uint32_t mid_index[3];

		for (int i = 0; i < 3; i++)
		{
			glm::vec3 mid = (tringle[i] + tringle[(i + 1) % 3]) / glm::vec3(2);
			auto iter_mid = edge_points.find(mid);
			if (iter_mid != edge_points.end()) {
				mid_index[i] = iter_mid->second;
				edge_points.erase(iter_mid);
			}
			else
			{
				mid_index[i] = new_vert_ind;
				vertex.push_back(projToSphere(mid));
				edge_points.emplace(mid, new_vert_ind);
				new_vert_ind++;
			}

		}


		for (int i = 0; i < 3; i++)
		{
			index[ind_triangle + i] = mid_index[i];

			index.push_back(old_indexes[i]);
			index.push_back(mid_index[i]);
			index.push_back(mid_index[(i + 2) % 3]);
		}

	}
	void reinit_vertex() {
		vertex = std::vector<glm::vec3>();



		constexpr double angle_step = glm::radians(72.0f);
		double angle_offset;
		glm::vec3 curr_coord;


		angle_offset = 0;
		curr_coord.z = 0.5;
		for (uint32_t i = 0; i < 5; i++)
		{
			curr_coord.x = glm::cos(angle_offset);
			curr_coord.y = glm::sin(angle_offset);
			angle_offset += angle_step;

			vertex.push_back(curr_coord);
		}

		angle_offset = angle_step / 2;
		curr_coord.z = -0.5;
		for (uint32_t i = 0; i < 5; i++)
		{
			curr_coord.x = glm::cos(angle_offset);
			curr_coord.y = glm::sin(angle_offset);
			angle_offset += angle_step;

			vertex.push_back(curr_coord);
		}
		vertex.push_back(glm::vec3(0, 0, sqrt(5) / 2));

		vertex.push_back(glm::vec3(0, 0, -sqrt(5) / 2));
	}
	void reinit_index() {

		index = std::vector<uint32_t>();

		const uint32_t start_first_part = 0;
		const uint32_t start_second_part = 5;
		const uint32_t top_vertex = 10;
		const uint32_t bottom_vertex = 11;
		for (int i = 1; i <= 5; i++)
		{
			index.push_back(start_first_part + i % 5);
			index.push_back(start_first_part + i - 1);
			index.push_back(start_second_part + (i - 1));

			index.push_back(top_vertex);
			index.push_back(start_first_part + i - 1);
			index.push_back(start_first_part + i % 5);
			

			index.push_back(start_first_part + i % 5);
			index.push_back(start_second_part + i - 1);
			index.push_back(start_second_part + i % 5);

			index.push_back(start_second_part + i % 5);
			index.push_back(start_second_part + i - 1);
			index.push_back(bottom_vertex);
		}

	}
};

class MeshExporter {

public:

	static void toStl(const Mesh& msh, std::string filename)
	{

		const auto& vertices = msh.getVertices();
		const auto& indexes = msh.getIndexes();
		const auto& normalsFace = msh.getFaceNormals();

		auto vertex_to_string = [](const glm::vec3& vertex) {
			return std::to_string(vertex.x) + " " + std::to_string(vertex.y) + " " + std::to_string(vertex.z);
		};

		std::ofstream fout(filename + ".stl");
		fout << "solid name\n\t";
		for (uint32_t i = 0; i < indexes.size(); i += 3)
		{
			fout << "facet normal "
				+ std::to_string(normalsFace[i / 3].x) + " "
				+ std::to_string(normalsFace[i / 3].y) + " "
				+ std::to_string(normalsFace[i / 3].z) + "\n\t\t";

			fout << "outer loop\n\t\t\t";
			for (uint32_t j = 0; j < 3; j++)
				fout << "vertex " + vertex_to_string(vertices[indexes[i + j]]) + "\n\t\t\t";
			fout << "endloop loop\n\t";
			fout << "endfacet\n";
		}
		fout << "endsolid name";
		fout.close();
	}
};