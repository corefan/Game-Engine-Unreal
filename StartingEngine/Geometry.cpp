#include "Geometry.h"



Geometry_Manager::Geometry_Manager(PrimitiveTypes prim_type) : type(prim_type)
{
}

Geometry_Manager::Geometry_Manager(const Geometry_Manager & con_copy) : color(con_copy.color), type(con_copy.type), mesh(con_copy.mesh), geometry_div(con_copy.geometry_div)
{

}

Geometry_Manager::~Geometry_Manager()
{
}

void Geometry_Manager::Initialize()
{
	// Buffer for vertex
	glGenBuffers(1, (GLuint*)&(mesh.id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) *mesh.num_vertices * 3, &mesh.vertices[0], GL_STATIC_DRAW);

	// Buffer for indices
	glGenBuffers(1, (GLuint*)&(mesh.id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.num_indices, &mesh.indices[0], GL_STATIC_DRAW);

}

void Geometry_Manager::Draw()
{
	if (mesh.num_indices>0 && mesh.num_vertices>0) {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_indices);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, NULL);
	}
	else {
		LOG("Impossible to draw the mesh");
	}
}

void Geometry_Manager::SetColor(const Color & oth_color)
{
	color = oth_color;
}

/*void Geometry_Manager::SetType(PrimitiveTypes oth_type)
{
type = oth_type;
}

void Geometry_Manager::SetDivisions(uint divis)
{
geometry_div = divis;
}*/

Color Geometry_Manager::GetColor() const
{
	return color;
}

PrimitiveTypes Geometry_Manager::GetType() const
{
	return type;
}


