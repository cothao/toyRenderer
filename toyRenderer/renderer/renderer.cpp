#include "renderer.h"
#include "../objects/objects.h"
#include "../stb_image.h"

namespace Renderer
{
	extern glm::vec3 cameraPos = glm::vec3(1.);
	extern glm::vec4 albedo = glm::vec4(1., 0., 0., 1.);
	extern glm::mat4 model = glm::mat4(1.);
	extern glm::mat4 view = glm::mat4(1.);
	extern glm::mat4 projection = glm::mat4(1.);
	extern glm::mat4 MVP = glm::mat4(1.);
	extern int nrRows = 7;
	extern int nrColumns = 7;
	extern float spacing = 2.5;
	extern float metallic = 1.f;
	extern float roughness = .0f;
	extern unsigned int brdfLUTTexture = 0;
	extern std::string currentHDRTexture = "loft";
	extern std::string lastHDRTexture = currentHDRTexture;

	extern glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
	};

	extern glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
}

void Renderer::Init()
{

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);

	FramebufferManager::SetFBO("capture");
	FramebufferManager::SetFBO("depthMap");

	TextureDirectory::SetTexture("rusted_metal_base_map", "../images/rusted_metal/rustediron2_basecolor.png", true);
	TextureDirectory::SetTexture("rusted_metal_metallic_map", "../images/rusted_metal/rustediron2_metallic.png", true);
	TextureDirectory::SetTexture("rusted_metal_normal_map", "../images/rusted_metal/rustediron2_normal.png", true);
	TextureDirectory::SetTexture("rusted_metal_roughness_map", "../images/rusted_metal/rustediron2_roughness.png", true);
	TextureDirectory::SetTexture("rusted_metal_ao_map", "../images/rusted_metal/rustediron2_roughness.png", true);

	TextureDirectory::SetTexture("gun_base_map", "../assets/Cerberus_by_Andrew_Maximov/Cerberus_by_Andrew_Maximov/Textures/Cerberus_A.png", true);
	TextureDirectory::SetTexture("gun_metallic_map", "../assets/Cerberus_by_Andrew_Maximov/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.png", true);
	TextureDirectory::SetTexture("gun_normal_map", "../assets/Cerberus_by_Andrew_Maximov/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.png", true);
	TextureDirectory::SetTexture("gun_roughness_map", "../assets/Cerberus_by_Andrew_Maximov/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.png", true);

	TextureDirectory::SetTexture("mask_base_map", "../assets/pbr-kabuto-samurai-helmet/textures/BaseColor.png", true);
	TextureDirectory::SetTexture("mask_metallic_map", "../assets/pbr-kabuto-samurai-helmet/textures/Metalness.png", true);
	TextureDirectory::SetTexture("mask_normal_map", "../assets/pbr-kabuto-samurai-helmet/textures/NormalMap.png", true);
	TextureDirectory::SetTexture("mask_roughness_map", "../assets/pbr-kabuto-samurai-helmet/textures/Roughness.png", true);
	TextureDirectory::SetTexture("mask_ao_map", "../assets/pbr-kabuto-samurai-helmet/textures/AOMap.png", true);

	TextureDirectory::SetTexture("backpack_base_map", "../assets/backpack/diffuse.jpg", true);
	TextureDirectory::SetTexture("backpack_metallic_map", "../assets/backpack/specular.jpg", true);
	TextureDirectory::SetTexture("backpack_normal_map", "../assets/backpack/normal.png", true);
	TextureDirectory::SetTexture("backpack_roughness_map", "../assets/backpack/roughness.jpg", true);
	TextureDirectory::SetTexture("backpack_ao_map", "../assets/backpack/ao.jpg", true);

	TextureDirectory::SetLookupTexture("brdfLUTTexture");
	
	TextureDirectory::SetHDRTexture("photo_studio", "../images/hdr/photo_studio_loft_hall_4k.hdr");
	TextureDirectory::SetHDRTexture("field", "../images/hdr/horn-koppe_spring_4k.hdr");
	TextureDirectory::SetHDRTexture("loft", "../images/hdr/newport_loft.hdr");

	InitShaders();
	InitModels();

	ShaderDirectory::GetShader("modelShader").Use();
	ShaderDirectory::GetShader("modelShader").SetInt("texture_diffuse1", 0);
	ShaderDirectory::GetShader("modelShader").SetMat4("view", view);
	ShaderDirectory::GetShader("modelShader").SetMat4("projection", projection);

	ShaderDirectory::GetShader("pbrNoMap").Use();
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("view", view);
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("projection", projection);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("irradianceMap", 0);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("brdfLUT", 2);

	ShaderDirectory::GetShader("pbrMaterial").Use();
	ShaderDirectory::GetShader("pbrMaterial").SetMat4("view", view);
	ShaderDirectory::GetShader("pbrMaterial").SetMat4("projection", projection);
	//ShaderDirectory::GetShader("pbrMaterial").SetFloat("ao", 0.f);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("base_map", 3);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("metallic_map", 4);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("normal_map", 5);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("roughness_map", 6);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("aoMap", 7);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("brdfLUT", 2);

	ShaderDirectory::GetShader("equirectangular").Use();
	ShaderDirectory::GetShader("equirectangular").SetInt("equirectangularMap", 0);
	ShaderDirectory::GetShader("equirectangular").SetMat4("projection", projection);
	ShaderDirectory::GetShader("equirectangular").SetMat4("view", view);

	EnvironmentMapManager::SetEnvCubeMap();

	InitEnvironmentMap();

	ShaderDirectory::GetShader("cubemap").Use();
	ShaderDirectory::GetShader("cubemap").SetMat4("projection", projection);

};

void Renderer::SetModelMatrix(glm::mat4 model)
{
	Renderer::model = model;
}

void Renderer::SetViewMatrix(glm::mat4 view)
{
	Renderer::view = view;
}

void Renderer::SetProjectionMatrix(glm::mat4 projection)
{
	Renderer::projection = projection;
}

void Renderer::SetCameraPosition(glm::vec3 cameraPos)
{
	Renderer::cameraPos = cameraPos;
}


void Renderer::ClearBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::ClearColor(glm::vec4 color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

// Renders the entire scene. Includes shader updates, transformations, etc.
void Renderer::RenderScene()
{

	ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
	ClearBuffers();

	ChangeHDRState();

	TextureDirectory::BindMapTextures(
		TextureDirectory::GetTexture("rusted_metal_base_map"),
		TextureDirectory::GetTexture("rusted_metal_metallic_map"),
		TextureDirectory::GetTexture("rusted_metal_normal_map"),
		TextureDirectory::GetTexture("rusted_metal_roughness_map"),
		TextureDirectory::GetTexture("rusted_metal_ao_map")
		);

	TextureDirectory::BindPBRTextures(EnvironmentMapManager::GetIrradianceMap(), EnvironmentMapManager::GetPrefilterMap());

	DrawObjects();

	// Draw the models in the model directory

	//stbi_set_flip_vertically_on_load(false);

	ShaderDirectory::GetShader("pbrMaterial").Use();

	//TextureDirectory::BindModelTextureMappings("gun");
	
	//TextureDirectory::BindModelTextureMappings("mask");

	//TextureDirectory::BindModelTextureMappings("backpack");

	model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(3.f, 0.f, 5.f));
	model = glm::scale(model, glm::vec3(10.1f, 10.1f, 10.1f));
	model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
	ShaderDirectory::GetShader("pbrMaterial").SetMat4("model", model);
	ShaderDirectory::GetShader("pbrMaterial").SetMat4("view", view);
	ShaderDirectory::GetShader("pbrMaterial").SetMat4("projection", projection);

	DrawModels();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMapManager::GetEnvCubeMap());

	ShaderDirectory::GetShader("cubemap").Use();
	ShaderDirectory::GetShader("cubemap").SetMat4("view", view);
	ShaderDirectory::GetShader("cubemap").SetInt("environmentMap", 0);

	Object::Cube();

}

// Initializes shaders from ShaderDirectory
void Renderer::InitShaders()
{

	ShaderDirectory::SetShader("modelShader", Shader("./shaders/model.vert", "./shaders/model.frag", nullptr));
	ShaderDirectory::SetShader("pbrMaterial", Shader("./shaders/sphere_pbr.vert", "./shaders/sphere_pbr.frag", nullptr));
	ShaderDirectory::SetShader("lightShader", Shader("./shaders/sphere_pbr.vert", "./shaders/light.frag", nullptr));
	ShaderDirectory::SetShader("pbrNoMap", Shader("./shaders/pbr_no_map.vert", "./shaders/pbr_no_map.frag", nullptr));
	ShaderDirectory::SetShader("equirectangular", Shader("./shaders/equirectangular.vert", "./shaders/equirectangular.frag", nullptr));
	ShaderDirectory::SetShader("cubemap", Shader("./shaders/cubemap.vert", "./shaders/cubemap.frag", nullptr));
	ShaderDirectory::SetShader("background", Shader("./shaders/background.vert", "./shaders/background.frag", nullptr));
	ShaderDirectory::SetShader("irradianceShader", Shader("./shaders/equirectangular.vert", "./shaders/convolutionShader.frag", nullptr));
	ShaderDirectory::SetShader("prefilterShader", Shader("./shaders/equirectangular.vert", "./shaders/prefilter.frag", nullptr));
	ShaderDirectory::SetShader("brdfShader", Shader("./shaders/brdf.vert", "./shaders/brdf.frag", nullptr));

}

void Renderer::InitModels()
{
	//ModelDirectory::SetModel("mask", Model("../assets/pbr-kabuto-samurai-helmet/source/HelmetPresentationLightMap.fbx.fbx"));
	//ModelDirectory::SetModel("gun", Model("../assets/Cerberus_by_Andrew_Maximov/Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx"));
	//ModelDirectory::SetModel("backpack", Model("../assets/backpack/backpack.obj"));
}

glm::mat4 Renderer::GetModelMatrix()
{
	return Renderer::model;
}

glm::mat4 Renderer::GetViewMatrix()
{
	return Renderer::view;
}

glm::mat4 Renderer::GetProjectionMatrix()
{
	return Renderer::projection;
}

glm::vec3 Renderer::GetCameraPosition()
{
	return Renderer::cameraPos;
}

void Renderer::DrawObjects()
{

	ShaderDirectory::GetShader("pbrMaterial").Use();

	ShaderDirectory::GetShader("pbrMaterial").SetVec3("camPos", Renderer::GetCameraPosition());

	for (int i = 0; i < 4; i++)
	{

		ShaderDirectory::GetShader("pbrMaterial").SetVec3("lightPosition[" + std::to_string(i) + ']', lightPositions[i]);
		ShaderDirectory::GetShader("pbrMaterial").SetVec3("lightColor[" + std::to_string(i) + ']', lightColors[i]);

	}

	ShaderDirectory::GetShader("pbrMaterial").SetMat4("view", view);

	ShaderDirectory::GetShader("pbrMaterial").SetMat4("model", model);

	Object::Sphere();

	// Eventually, want this to be part of the Objects, but for now, just rendering like this
	DrawLights();

	ShaderDirectory::GetShader("pbrNoMap").Use();

	glm::mat4 stageModel = glm::mat4(1.);

	stageModel = glm::translate(stageModel, glm::vec3(4., 0., 0.));

	SetModelMatrix(stageModel);

	ShaderDirectory::GetShader("pbrNoMap").SetVec3("camPos", Renderer::GetCameraPosition());
	ShaderDirectory::GetShader("pbrNoMap").SetVec3("albedo", glm::vec3(albedo.r, albedo.g, albedo.b));

	for (int i = 0; i < 4; i++)
	{

		ShaderDirectory::GetShader("pbrNoMap").SetVec3("lightPosition[" + std::to_string(i) + ']', lightPositions[i]);
		ShaderDirectory::GetShader("pbrNoMap").SetVec3("lightColor[" + std::to_string(i) + ']', lightColors[i]);

	}

	ShaderDirectory::GetShader("pbrNoMap").SetMat4("view", view);

	ShaderDirectory::GetShader("pbrNoMap").SetMat4("model", model);

	ShaderDirectory::GetShader("pbrNoMap").SetFloat("metallic", metallic);
	ShaderDirectory::GetShader("pbrNoMap").SetFloat("roughness", roughness);
	ShaderDirectory::GetShader("pbrNoMap").SetFloat("ao", 1.);
	Object::Sphere();

}

void Renderer::DrawModel(std::string modelName, std::string shaderName)
{
	ModelDirectory::Directory[modelName].Draw(ShaderDirectory::GetShader(shaderName));
}

void Renderer::DrawModels() 
{
	for (auto modelKey = ModelDirectory::Directory.begin(); modelKey != ModelDirectory::Directory.end(); modelKey++)
	{
		std::string modelName = modelKey->first;
		TextureDirectory::BindModelTextureMappings();
		DrawModel(modelName, "pbrMaterial");
	}
}



void Renderer::DrawLights()
{
	for (int i = 0; i < 4; i++)
	{

		glm::mat4 model = glm::mat4(1.);
		model = glm::translate(model, lightPositions[i]);

		SetModelMatrix(model);
		ShaderDirectory::GetShader("lightShader").Use();
		ShaderDirectory::GetShader("lightShader").SetMat4("model", model);
		ShaderDirectory::GetShader("lightShader").SetMat4("view", view);
		ShaderDirectory::GetShader("lightShader").SetMat4("projection", projection);
		Object::Sphere();

	}
}

void Renderer::InitEnvironmentMap()
{

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};


	FramebufferManager::GenerateCubemapFromEnvironmentMap(ShaderDirectory::GetShader("equirectangular"), TextureDirectory::GetTexture(currentHDRTexture), EnvironmentMapManager::GetEnvCubeMap(), captureProjection, captureViews);

	EnvironmentMapManager::SetIrradianceMap();

	FramebufferManager::GenerateCubemapFromIrradianceMap(ShaderDirectory::GetShader("irradianceShader"), EnvironmentMapManager::GetIrradianceMap(), EnvironmentMapManager::GetEnvCubeMap(), captureProjection, captureViews);

	EnvironmentMapManager::SetPrefilterMap();

	FramebufferManager::GenerateCubemapFromPrefilterMap(ShaderDirectory::GetShader("prefilterShader"), EnvironmentMapManager::GetPrefilterMap(), EnvironmentMapManager::GetEnvCubeMap(), captureProjection, captureViews);

	FramebufferManager::GenerateTextureFromFramebuffer(ShaderDirectory::GetShader("brdfShader"), TextureDirectory::GetTexture("brdfLUTTexture"));

}

void Renderer::ChangeHDRState()
{
	// Move this to separate function
	if (lastHDRTexture != currentHDRTexture)
	{

		InitEnvironmentMap();
		lastHDRTexture = currentHDRTexture;

	}
}