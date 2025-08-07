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
	extern glm::mat4 lightProjection = glm::mat4(1.);
	extern glm::vec3 lightPos = glm::vec3(-20., 10.f, 0.f);
	extern glm::mat4 lightView = glm::lookAt(lightPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	extern glm::mat4 lightSpaceMatrix = glm::mat4(1.);
	extern glm::mat4 MVP = glm::mat4(1.);
	extern int nrRows = 7;
	extern int nrColumns = 7;
	extern float spacing = 2.5;
	extern float metallic = 1.f;
	extern float roughness = .0f;
	extern unsigned int brdfLUTTexture = 0;
	extern std::string currentHDRTexture = "sky";
	extern std::string lastHDRTexture = currentHDRTexture;
	extern int SHADOW_WIDTH = 1024;
	extern int SHADOW_HEIGHT = 1024;
	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float near = 1.0f;
	float far = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

	extern glm::vec3 lightPositions[] = {
	glm::vec3(-20.0f,  10.0f, 0.0f),
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

	extern std::vector<glm::mat4> shadowTransforms = {};

}

void Renderer::Init()
{

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);

	shadowTransforms.push_back(shadowProj *
	glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
shadowTransforms.push_back(shadowProj *
	glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
shadowTransforms.push_back(shadowProj *
	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
shadowTransforms.push_back(shadowProj *
	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
shadowTransforms.push_back(shadowProj *
	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
shadowTransforms.push_back(shadowProj *
	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	FramebufferManager::SetFBO("capture");

	TextureDirectory::LoadCubeMapDepthTexture();

	FramebufferManager::SetDepthMapFBO(TextureDirectory::GetTexture("depthMap"));

	TextureDirectory::SetTexture("rusted_metal_base_map", "../images/rusted_metal/rustediron2_basecolor.png", true);
	TextureDirectory::SetTexture("rusted_metal_metallic_map", "../images/rusted_metal/rustediron2_metallic.png", true);
	TextureDirectory::SetTexture("rusted_metal_normal_map", "../images/rusted_metal/rustediron2_normal.png", true);
	TextureDirectory::SetTexture("rusted_metal_roughness_map", "../images/rusted_metal/rustediron2_roughness.png", true);
	TextureDirectory::SetTexture("rusted_metal_ao_map", "../images/rusted_metal/rustediron2_roughness.png", true);

	TextureDirectory::SetLookupTexture("brdfLUTTexture");

	TextureDirectory::SetTexture("floor", "../images/wood.png", false);
	TextureDirectory::SetTexture("brick_base_map", "../images/brickwall/brickwall.jpg", false);
	TextureDirectory::SetTexture("brick_normal_map", "../images/brickwall/brickwall_normal.jpg", false);
	TextureDirectory::SetTexture("brick_orange_base_map", "../images/bricks/bricks2.jpg", false);
	TextureDirectory::SetTexture("brick_orange_normal_map", "../images/bricks/bricks2_normal.jpg", false);
	
	TextureDirectory::SetHDRTexture("photo_studio", "../images/hdr/photo_studio_loft_hall_4k.hdr");
	TextureDirectory::SetHDRTexture("sky", "../images/hdr/rustig_koppie_puresky_8k.hdr");
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

	ShaderDirectory::GetShader("shadow").Use();
	ShaderDirectory::GetShader("shadow").SetInt("irradianceMap", 0);
	ShaderDirectory::GetShader("shadow").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("shadow").SetInt("brdfLUT", 2);
	ShaderDirectory::GetShader("shadow").SetInt("base_map", 3);
	ShaderDirectory::GetShader("shadow").SetInt("metallic_map", 4);
	ShaderDirectory::GetShader("shadow").SetInt("normal_map", 5);
	ShaderDirectory::GetShader("shadow").SetInt("roughness_map", 6);
	ShaderDirectory::GetShader("shadow").SetInt("aoMap", 7);
	ShaderDirectory::GetShader("shadow").SetInt("depthMap", 8);

	EnvironmentMapManager::SetEnvCubeMap();

	InitEnvironmentMap();

	ShaderDirectory::GetShader("cubemap").Use();
	ShaderDirectory::GetShader("cubemap").SetMat4("projection", projection);
	ShaderDirectory::GetShader("depthQuad").Use();
	ShaderDirectory::GetShader("depthQuad").SetInt("depthMap", 0);
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

	ClearColor(glm::vec4(.1f, .1f, .1f, 1.f));
	ClearBuffers();

	ChangeHDRState();

	//lightPos = glm::vec3(-20 - cos(glfwGetTime()) * 5., 10.f, sin(glfwGetTime()));

	TextureDirectory::BindPBRTextures(EnvironmentMapManager::GetIrradianceMap(), EnvironmentMapManager::GetPrefilterMap());

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferManager::GetFBO("depthMap").FBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	ShaderDirectory::GetShader("shadowCubeMap").Use();

	for (int i = 0; i < shadowTransforms.size(); ++i)
	{
		ShaderDirectory::GetShader("shadowCubeMap").SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}

	ShaderDirectory::GetShader("shadowCubeMap").SetVec3("lightPos", lightPos);
	ShaderDirectory::GetShader("shadowCubeMap").SetFloat("far_plane", far);

	model = glm::translate(model, ModelDirectory::translation);
	model = glm::scale(model, ModelDirectory::size);
	model = glm::rotate(model, ModelDirectory::rotation.x, glm::vec3(1., 0., 0.));
	model = glm::rotate(model, ModelDirectory::rotation.y, glm::vec3(0., 1., 0.));
	model = glm::rotate(model, ModelDirectory::rotation.z, glm::vec3(0., 0., 1.));

	ShaderDirectory::GetShader("shadowCubeMap").SetMat4("model", model);

	DrawModels("shadowCubeMap");
	model = glm::mat4(1.);
	ShaderDirectory::GetShader("shadowCubeMap").SetMat4("model", model);

	DrawDepthObjects();
	model = glm::mat4(1.);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1600, 1000);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the models in the model directory

	ShaderDirectory::GetShader("shadow").Use();

	for (int i = 0; i < 4; i++)
	{
		//lightPositions[i] = glm::vec3((float)sin(glfwGetTime()) * 10.f, 0.f, (float)sin(glfwGetTime()) * 10.f);
		ShaderDirectory::GetShader("shadow").SetVec3("lightPosition[" + std::to_string(i) + ']', lightPositions[i]);
		ShaderDirectory::GetShader("shadow").SetVec3("lightColor[" + std::to_string(i) + ']', lightColors[i]);

	}

	model = glm::mat4(1.);

	ShaderDirectory::GetShader("shadow").SetMat4("model", model);
	ShaderDirectory::GetShader("shadow").SetMat4("view", view);
	ShaderDirectory::GetShader("shadow").SetVec3("camPos", cameraPos);
	ShaderDirectory::GetShader("shadow").SetVec3("lightPos", lightPos);
	ShaderDirectory::GetShader("shadow").SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	ShaderDirectory::GetShader("shadow").SetMat4("projection", projection);
	ShaderDirectory::GetShader("shadow").SetFloat("far_plane", far);

	TextureDirectory::BindFloorTextureMappings();

	Object::Plane();

	ShaderDirectory::GetShader("pbrMaterial").Use();

	TextureDirectory::BindModelTextureMappings();
	stbi_set_flip_vertically_on_load(TextureDirectory::flip_uvs);
	model = glm::translate(model, ModelDirectory::translation);
	model = glm::scale(model, ModelDirectory::size);
	model = glm::rotate(model, ModelDirectory::rotation.x, glm::vec3(1., 0., 0.));
	model = glm::rotate(model, ModelDirectory::rotation.y, glm::vec3(0., 1., 0.));
	model = glm::rotate(model, ModelDirectory::rotation.z, glm::vec3(0., 0., 1.));

	ShaderDirectory::GetShader("pbrMaterial").SetMat4("model", model);

	DrawModels("pbrMaterial");

	DrawObjects();
	
	ShaderDirectory::GetShader("lightShader").Use();

	model = glm::mat4(1.);

	model = glm::translate(model, lightPos);

	ShaderDirectory::GetShader("lightShader").SetMat4("model", model);
	ShaderDirectory::GetShader("lightShader").SetMat4("view", view);
	ShaderDirectory::GetShader("lightShader").SetMat4("projection", projection);

	Object::Sphere();


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
	ShaderDirectory::SetShader("depthShader", Shader("./shaders/depthShader.vert", "./shaders/depthShader.frag", nullptr));
	ShaderDirectory::SetShader("depthQuad", Shader("./shaders/depthQuad.vert", "./shaders/depthQuad.frag", nullptr));
	ShaderDirectory::SetShader("shadow", Shader("./shaders/shadow.vert", "./shaders/shadow.frag", nullptr));
	ShaderDirectory::SetShader("shadowCubeMap", Shader("./shaders/depthCubeMap.vert", "./shaders/depthCubeMap.frag", "./shaders/depthCubeMap.geom"));
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

void Renderer::DrawDepthObjects()
{

	glm::mat4 stageModel = glm::mat4(1.);

	stageModel = glm::translate(stageModel, glm::vec3(-8., 1., 4.));
	stageModel = glm::scale(stageModel, ModelDirectory::size);

	SetModelMatrix(stageModel);

	ShaderDirectory::GetShader("shadowCubeMap").SetMat4("model", model);
}//hola

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


	// Eventually, want this to be part of the Objects, but for now, just rendering like this
	//DrawLights();
	model = glm::mat4(1.);

	model = glm::translate(model, glm::vec3(-8., 1., 4.));

	ShaderDirectory::GetShader("pbrMaterial").SetMat4("model", model);
	ShaderDirectory::GetShader("pbrNoMap").Use();

	SetModelMatrix(model);

	//ShaderDirectory::GetShader("pbrNoMap").SetVec3("camPos", Renderer::GetCameraPosition());
	//ShaderDirectory::GetShader("pbrNoMap").SetVec3("albedo", glm::vec3(albedo.r, albedo.g, albedo.b));

	//for (int i = 0; i < 4; i++)
	//{

	//	ShaderDirectory::GetShader("pbrNoMap").SetVec3("lightPosition[" + std::to_string(i) + ']', lightPositions[i]);
	//	ShaderDirectory::GetShader("pbrNoMap").SetVec3("lightColor[" + std::to_string(i) + ']', lightColors[i]);

	//}

	//ShaderDirectory::GetShader("pbrNoMap").SetMat4("view", view);

	//ShaderDirectory::GetShader("pbrNoMap").SetMat4("model", model);

	//ShaderDirectory::GetShader("pbrNoMap").SetFloat("metallic", metallic);
	//ShaderDirectory::GetShader("pbrNoMap").SetFloat("roughness", roughness);
	//ShaderDirectory::GetShader("pbrNoMap").SetFloat("ao", 1.);
	//Object::Sphere();

}

void Renderer::DrawModel(std::string modelName, std::string shaderName)
{
	ModelDirectory::Directory[modelName].Draw(ShaderDirectory::GetShader(shaderName));
}

void Renderer::DrawModels(std::string shaderName) 
{
	for (auto modelKey = ModelDirectory::Directory.begin(); modelKey != ModelDirectory::Directory.end(); modelKey++)
	{
		std::string modelName = modelKey->first;
		TextureDirectory::BindModelTextureMappings();
		DrawModel(modelName, shaderName);
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