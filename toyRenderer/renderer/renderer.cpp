#include "renderer.h"
#include "../objects/objects.h"
#include "../stb_image.h"

namespace Renderer
{
	extern glm::mat4 model = glm::mat4(1.);
	extern glm::mat4 view = glm::mat4(1.);
	extern glm::mat4 projection = glm::mat4(1.);
	extern glm::vec3 cameraPos = glm::vec3(1.);
	extern glm::mat4 MVP = glm::mat4(1.);
	extern int nrRows = 7;
	extern int nrColumns = 7;
	extern float spacing = 2.5;
	extern float metallic = 1.f;
	extern float roughness = .0f;
	extern unsigned int captureFBO = 0;
	extern unsigned int captureRBO = 0;
	extern unsigned int envCubemap = 0;
	extern unsigned int irradianceMap = 0;
	extern unsigned int prefilterMap = 0;
	extern unsigned int brdfLUTTexture = 0;

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
     /*
      *  _  _ ___  ___     _____ _____  _______ _   _ ___ ___ 
      * | || |   \| _ \   |_   _| __\ \/ /_   _| | | | _ \ __|
      * | __ | |) |   /     | | | _| >  <  | | | |_| |   / _| 
      * |_||_|___/|_|_\     |_| |___/_/\_\ |_|  \___/|_|_\___|
      *                                                       
      */

     /*
      *  _    ___   _   ___      _____ _____  _______ _   _ ___ ___ ___ 
      * | |  / _ \ /_\ |   \    |_   _| __\ \/ /_   _| | | | _ \ __/ __|
      * | |_| (_) / _ \| |) |     | | | _| >  <  | | | |_| |   / _|\__ \
      * |____\___/_/ \_\___/      |_| |___/_/\_\ |_|  \___/|_|_\___|___/
      *                                                                 
      */

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	TextureDirectory::SetTexture("rusted_metal_base_map", "../images/rusted_metal/rustediron2_basecolor.png", true);
	TextureDirectory::SetTexture("rusted_metal_metallic_map", "../images/rusted_metal/rustediron2_metallic.png", true);
	TextureDirectory::SetTexture("rusted_metal_normal_map", "../images/rusted_metal/rustediron2_normal.png", true);
	TextureDirectory::SetTexture("rusted_metal_roughness_map", "../images/rusted_metal/rustediron2_roughness.png", true);
	
	TextureDirectory::SetHDRTexture("photo_studio", "../images/photo_studio_loft_hall_4k.hdr");

	InitShaders();
	InitModels();

	ShaderDirectory::GetShader("pbrNoMap").Use();
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("view", view);
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("projection", projection);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("irradianceMap", 0);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("brdfLUT", 2);

	ShaderDirectory::GetShader("sphereShader").Use();
	ShaderDirectory::GetShader("sphereShader").SetMat4("view", view);
	ShaderDirectory::GetShader("sphereShader").SetMat4("projection", projection);
	ShaderDirectory::GetShader("sphereShader").SetFloat("ao", 1.f);
	ShaderDirectory::GetShader("sphereShader").SetInt("base_map", 3);
	ShaderDirectory::GetShader("sphereShader").SetInt("metallic_map", 4);
	ShaderDirectory::GetShader("sphereShader").SetInt("normal_map", 5);
	ShaderDirectory::GetShader("sphereShader").SetInt("roughness_map", 6);
	ShaderDirectory::GetShader("sphereShader").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("sphereShader").SetInt("brdfLUT", 2);

	ShaderDirectory::GetShader("equirectangular").Use();
	ShaderDirectory::GetShader("equirectangular").SetInt("equirectangularMap", 0);
	ShaderDirectory::GetShader("equirectangular").SetMat4("projection", projection);
	ShaderDirectory::GetShader("equirectangular").SetMat4("view", view);

	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	// convert HDR equirectangular environment map to cubemap equivalent
	ShaderDirectory::GetShader("equirectangular").Use();
	ShaderDirectory::GetShader("equirectangular").SetInt("equirectangularMap", 0);
	ShaderDirectory::GetShader("equirectangular").SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("photo_studio"));

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ShaderDirectory::GetShader("equirectangular").SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Object::Cube(); // renders a 1x1 cube
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
			GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	ShaderDirectory::GetShader("irradianceShader").Use();
	ShaderDirectory::GetShader("irradianceShader").SetInt("environmentMap", 0);
	ShaderDirectory::GetShader("irradianceShader").SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ShaderDirectory::GetShader("irradianceShader").SetMat4("view", captureViews[i]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Object::Cube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	ShaderDirectory::GetShader("prefilterShader").Use();
	ShaderDirectory::GetShader("prefilterShader").SetInt("environmentMap", 0);
	ShaderDirectory::GetShader("prefilterShader").SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		ShaderDirectory::GetShader("prefilterShader").SetFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			ShaderDirectory::GetShader("prefilterShader").SetMat4("view", captureViews[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Object::Cube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	ShaderDirectory::GetShader("brdfShader").Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Object::Quad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

	ShaderDirectory::GetShader("sphereShader").Use();

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_base_map"));

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_metallic_map"));

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_normal_map"));

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_roughness_map"));

	ShaderDirectory::GetShader("sphereShader").SetVec3("camPos", Renderer::GetCameraPosition());

	for (int i = 0; i < 4; i++)
	{

		ShaderDirectory::GetShader("sphereShader").SetVec3("lightPosition[" + std::to_string(i) + ']', lightPositions[i]);
		ShaderDirectory::GetShader("sphereShader").SetVec3("lightColor[" + std::to_string(i) + ']', lightColors[i]);

	}

	ShaderDirectory::GetShader("sphereShader").SetMat4("view", view);

	ShaderDirectory::GetShader("sphereShader").SetMat4("model", model);

	Object::Sphere();

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

	ShaderDirectory::GetShader("pbrNoMap").Use();

	glm::mat4 stageModel = glm::mat4(1.);

	stageModel = glm::translate(stageModel, glm::vec3(4., 0., 0.));

	SetModelMatrix(stageModel);

	ShaderDirectory::GetShader("pbrNoMap").SetVec3("camPos", Renderer::GetCameraPosition());
	ShaderDirectory::GetShader("pbrNoMap").SetVec3("albedo", glm::vec3(1., 0., 0.));

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

	//ShaderDirectory::GetShader("equirectangular").Use();
	//ShaderDirectory::GetShader("equirectangular").SetMat4("view", view);

	//glActiveTexture(GL_TEXTURE0);

	//glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("photo_studio"));

	//Object::Cube();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	ShaderDirectory::GetShader("cubemap").Use();
	ShaderDirectory::GetShader("cubemap").SetMat4("view", view);

	ShaderDirectory::GetShader("cubemap").SetInt("environmentMap", 0);

	Object::Cube();

	// Draw the models in the model directory
	//for (auto modelKey = ModelDirectory::Directory.begin(); modelKey != ModelDirectory::Directory.end(); modelKey++)
	//{
	//	DrawModel(modelKey->first, "modelShader");
	//}


}

// Initializes shaders from ShaderDirectory
void Renderer::InitShaders()
{

	ShaderDirectory::SetShader("modelShader", Shader("./shaders/model.vert", "./shaders/model.frag", nullptr));
	ShaderDirectory::SetShader("sphereShader", Shader("./shaders/sphere_pbr.vert", "./shaders/sphere_pbr.frag", nullptr));
	ShaderDirectory::SetShader("lightShader", Shader("./shaders/sphere_pbr.vert", "./shaders/light.frag", nullptr));
	ShaderDirectory::SetShader("pbrNoMap", Shader("./shaders/pbr_no_map.vert", "./shaders/pbr_no_map.frag", nullptr));
	ShaderDirectory::SetShader("equirectangular", Shader("./shaders/equirectangular.vert", "./shaders/equirectangular.frag", nullptr));
	ShaderDirectory::SetShader("cubemap", Shader("./shaders/cubemap.vert", "./shaders/cubemap.frag", nullptr));
	ShaderDirectory::SetShader("irradianceShader", Shader("./shaders/cubemap.vert", "./shaders/convolutionShader.frag", nullptr));
	ShaderDirectory::SetShader("prefilterShader", Shader("./shaders/cubemap.vert", "./shaders/prefilter.frag", nullptr));
	ShaderDirectory::SetShader("brdfShader", Shader("./shaders/brdf.vert", "./shaders/brdf.frag", nullptr));

}

void Renderer::InitModels()
{

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

void Renderer::DrawObject()
{

}

void Renderer::DrawModel(std::string modelName, std::string shaderName)
{
	ModelDirectory::Directory[modelName].Draw(ShaderDirectory::GetShader(shaderName));
}