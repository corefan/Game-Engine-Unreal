#include "ModuleAssimp.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleGui.h"
#include "Module.h"
#include"Imgui/imgui_impl_sdl.h"
#include"Imgui\imgui_impl_sdl_gl3.h"
#include"Imgui/imgui.h"
#include"ModuleSceneIntro.h"



ModuleAssimp::ModuleAssimp(bool start_enabled) : Module(start_enabled)
{

}

// Destructor
ModuleAssimp::~ModuleAssimp()
{
}

// Called before render is available
bool ModuleAssimp::Start()
{
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	ilClearColour(255, 255, 255, 000);

	//Check for error
	ILenum ilError = ilGetError();
	if (ilError != IL_NO_ERROR)
	{
		LOG("IlInit error!!");
	}
	


	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, NULL);
	aiAttachLogStream(&stream);

	//ImportGeometry();

	for (int p = 0; p < meshes_vec.size(); p++) {
		meshes_vec[p]->Initialize();
	}

	return true;
}

update_status ModuleAssimp::PreUpdate(float dt)
{



	return update_status::UPDATE_CONTINUE;
}

update_status ModuleAssimp::Update(float dt)
{

	for (int p = 0; p < meshes_vec.size(); p++) {
		meshes_vec[p]->Draw();
	}


	return UPDATE_CONTINUE;
}


update_status ModuleAssimp::PostUpdate(float dt)
{

	if (App->input->flie_dropped) {
		ImportGeometry(App->input->dropped_filedir);
		for (int p = 0; p < meshes_vec.size(); p++) {
			meshes_vec[p]->Initialize();
		}
		App->input->flie_dropped = false;
	}


	return UPDATE_CONTINUE;
}

bool ModuleAssimp::Gui_Engine_Modules(float dt)
{
	
	return false;
}


// Called before quitting
bool ModuleAssimp::CleanUp()
{
	

	return true;
}

void ModuleAssimp::ImportGeometry(char* fbx)
{

	//----------------ASSIMP
	

	std::string full_path;
	full_path = fbx;
	const aiScene* scene = aiImportFile(full_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++) {
			m = new Geometry_Manager(PrimitiveTypes::Primitive_Mesh);
			m->mesh.num_vertices = scene->mMeshes[i]->mNumVertices;
			m->mesh.vertices = new float[m->mesh.num_vertices * 3];
			memcpy(m->mesh.vertices, scene->mMeshes[i]->mVertices, sizeof(float) * m->mesh.num_vertices * 3);
			LOG("New mesh with %d vertices", m->mesh.num_vertices);
			// copy faces
			if (scene->mMeshes[i]->HasFaces())
			{
				m->mesh.num_indices = scene->mMeshes[i]->mNumFaces * 3;
				m->mesh.indices = new uint[m->mesh.num_indices]; // assume each face is a triangle
				
				for (uint k = 0; k < scene->mMeshes[i]->mNumFaces; ++k)
				{
					if (scene->mMeshes[i]->mFaces[k].mNumIndices != 3) {
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else {
						memcpy(&m->mesh.indices[k * 3], scene->mMeshes[i]->mFaces[k].mIndices, 3 * sizeof(uint));
					}
				}
			}

			if (scene->mMeshes[i]->HasNormals()) {
				m->mesh.normals = new float[m->mesh.num_vertices * 3];
				memcpy(m->mesh.normals, scene->mMeshes[i]->mNormals, sizeof(float) * m->mesh.num_vertices * 3);
			}

			// colors
			/*if (scene->mMeshes[i]->HasVertexColors(0))
			{
				m->mesh.colors = new float[m->mesh.num_vertices * 3];
				memcpy(m->mesh.colors, scene->mMeshes[i]->mColors, sizeof(float) * m->mesh.num_vertices * 3);
			}*/

			// texture coords (only one texture for now)
			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				m->mesh.textures_coord = new float[m->mesh.num_vertices * 2];

				for (int z = 0; z < scene->mMeshes[i]->mNumVertices; ++z) {

					m->mesh.textures_coord[z*2] = scene->mMeshes[i]->mTextureCoords[0][z].x;
					m->mesh.textures_coord[z * 2+1] = scene->mMeshes[i]->mTextureCoords[0][z].y;
					

				}
				m->mesh.texture_str = "Baker_house.png";

				//memcpy(m->mesh.textures_coord, scene->mMeshes[i]->mTextureCoords[0], sizeof(float) * m->mesh.num_vertices * 2);
			}
			
			meshes_vec.push_back(m);
			
	
		}
		aiReleaseImport(scene);
	}
	else {
		LOG("Error loading scene %s", full_path);
	}
	//aiMesh



}

GLuint ModuleAssimp::LoadImage_devil(const char * theFileName, GLuint *buff)
{

	//Texture loading success
	bool textureLoaded = false;

	//Generate and set current image ID
	uint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);

	//Load image
	ILboolean success = ilLoadImage(theFileName);
	
	//Image loaded successfully
	if (success == IL_TRUE)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		//Convert image to RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (success == IL_TRUE)
		{
			textureLoaded = loadTextureFromPixels32((GLuint*)ilGetData(), (GLuint)ilGetInteger(IL_IMAGE_WIDTH), (GLuint)ilGetInteger(IL_IMAGE_HEIGHT), buff);
			//Create texture from file pixels
			textureLoaded = true;
		}

		//Delete file from memory
		ilDeleteImages(1, &imgID);
	}

	//Report error
	if (!textureLoaded)
	{

	}
	return textureLoaded;

}

bool ModuleAssimp::loadTextureFromPixels32(GLuint * id_pixels, GLuint width_img, GLuint height_img, GLuint *buff)
{


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, buff);
	glBindTexture(GL_TEXTURE_2D, *buff);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_img, height_img,
		0, GL_RGBA, GL_UNSIGNED_BYTE, id_pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);
	//Check for error
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error loading texture from %p pixels! %s\n", id_pixels, gluErrorString(error));
		return false;
	}

	return true;
}