#include "editor.h"
#include "../api/api.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../modelDirectory/modelDirectory.h"
//#include "../textureDirectory/textureDirectory.h"
#include "../renderer/renderer.h"

namespace Editor
{
    // Move to Model/Objects?? EVENTUALLY
	namespace ObjectContext
	{

	    float* roughness = &Renderer::roughness;
		float* metallic = &Renderer::metallic;
		extern glm::vec4 * albedo = &Renderer::albedo;

	};

    bool show_demo_window = true;
    bool show_another_window = false;
    bool show_image_display = false;
    ImVec4 clear_color = {};
    extern std::string current_displayed_image = "";

    ImGuiIO* io = nullptr;

    namespace
    {
        float getMainScale()
        {
            return ImGui_ImplGlfw_GetContentScaleForMonitor(API::getPrimaryMonitorPointer());
        }
    }
}

void Editor::Init()
{


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    Editor::clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    Editor::io = &ImGui::GetIO();
    (void)io;    // Setup Dear ImGui context
    (*io).ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    (*io).ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(getMainScale());        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    //style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(API::window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init("#version 330");

};

void Editor::StartFrame()
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);
    bool v = true;
    bool b = true;
    {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
        tab_bar_flags |= ImGuiTabBarFlags_DrawSelectedOverline;
        if (ImGui::BeginTabBar("#tabs", tab_bar_flags))
        {

            // [DEBUG] Stress tests
            //if ((ImGui::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (ImGui::GetIO().KeyCtrl) ImGui::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (int i = 0; i < 1; ++i)
            {


                if (ImGui::BeginTabItem("Object Manager", &v, 0))
                {
                    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
                    {

                        ImGui::Checkbox("Flip UVs", &TextureDirectory::flip_uvs);
                        ImGui::Checkbox("PBR Material", &Renderer::usePBRMaterial);
                        ImGui::Checkbox("ViewSphere", &Renderer::viewSphere);
                        
                        ImGui::SliderFloat("roughness", ObjectContext::roughness, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                        ImGui::SliderFloat("metallic", ObjectContext::metallic, 0.0f, 1.0f);
                        ImGui::InputFloat3("size", (float*)&ModelDirectory::size);
						ImGui::SliderFloat3("Rotation", (float*)&ModelDirectory::rotation, -180.0f, 180.0f); // Edit 3 floats using a slider from -180.0f to 180.0f
						ImGui::InputFloat3("Translation", (float*)&ModelDirectory::translation); // Edit 3 floats using a slider from -180.0f to 180.0f
                        ImGui::ColorEdit3("albedo", (float*)ObjectContext::albedo); // Edit 3 floats representing a color
                        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
                        ImGui::SameLine();
                        DemoWindowWidgetsImages();
                        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / (*io).Framerate, (*io).Framerate);
                    }
                ImGui::EndTabItem();
                };


                if (ImGui::BeginTabItem("Floor Manager", &b, 0))
                {
                    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
                    {

                        ImGui::SliderFloat("float", ObjectContext::roughness, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                        ImGui::SliderFloat("metallic", ObjectContext::metallic, 0.0f, 1.0f);
                        ImGui::ColorEdit3("albedo", (float*)ObjectContext::albedo); // Edit 3 floats representing a color
                        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color


                        ImGui::SameLine();
                        FloorTextureDisplayer();
                        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / (*io).Framerate, (*io).Framerate);
                    }
                    ImGui::EndTabItem();
                };

            }

            ImGui::EndTabBar();
        }
    }

    RenderMainMenuBar();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(API::window, &display_w, &display_h);
    API::SetViewPortSize(display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::RenderMainMenuBar()
{

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            Config();
            ImGui::EndMenu();
        }
        //if (ImGui::BeginMenu("Edit"))
        //{
        //    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        //    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {} // Disabled item
        //    ImGui::Separator();
        //    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        //    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        //    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        //    ImGui::EndMenu();
        //}
        ImGui::EndMainMenuBar();
    }

}

void Editor::Config()
{
    if (ImGui::MenuItem("Open texture", "Ctrl+O")) 
    {
        //ModelDirectory::SetModelFromFile();
        TextureDirectory::SetHDRTextureFromFile();
    }
    if (ImGui::MenuItem("Open model", "Ctrl+O"))
    {
        ModelDirectory::SetModelFromFile();
        //TextureDirectory::SetHDRTextureFromFile();
    }
    // commenting out as this might be useful for later projects
    //if (ImGui::BeginMenu("Open Recent"))
    //{
    //    ImGui::MenuItem("fish_hat.c");
    //    ImGui::MenuItem("fish_hat.inl");
    //    ImGui::MenuItem("fish_hat.h");
    //    if (ImGui::BeginMenu("More.."))
    //    {
    //        ImGui::MenuItem("Hello");
    //        ImGui::MenuItem("Sailor");
    //        if (ImGui::BeginMenu("Recurse.."))
    //        {
    //            Config();
    //            ImGui::EndMenu();
    //        }
    //        ImGui::EndMenu();
    //    }
    //    ImGui::EndMenu();
    //}
    //if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    //if (ImGui::MenuItem("Save As..")) {}

    //ImGui::Separator();
    ////IMGUI_DEMO_MARKER("Examples/Menu/Options");
    //if (ImGui::BeginMenu("Options"))
    //{
    //    static bool enabled = true;
    //    ImGui::MenuItem("Enabled", "", &enabled);
    //    ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Borders);
    //    for (int i = 0; i < 10; i++)
    //        ImGui::Text("Scrolling Text %d", i);
    //    ImGui::EndChild();
    //    static float f = 0.5f;
    //    static int n = 0;
    //    ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
    //    ImGui::InputFloat("Input", &f, 0.1f);
    //    ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
    //    ImGui::EndMenu();
    //}

}

void Editor::Terminate()
{

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

static void Editor::DemoWindowWidgetsImages()
{

        ImGuiIO& io = ImGui::GetIO();


        // Regular user code should never have to care about TexData-> fields, but since we want to display the entire texture here, we pull Width/Height from it.
        float my_tex_w = (float)io.Fonts->TexData->Width;
        float my_tex_h = (float)io.Fonts->TexData->Height;

        ImGui::TextWrapped("");

        int textureId = 0;

        for (auto textureKey = TextureDirectory::TextureMappings.begin(); textureKey != TextureDirectory::TextureMappings.end(); textureKey++)
        {

            std::string     textureName = textureKey->first;
			TextureMapping  textureMapping = textureKey->second;

            //if (textureName != "")	    std::cout << "Texture Key: " << textureName << std::endl;
            //std::cout << textureKey->second << "\n";
            ImTextureID     my_tex_id = TextureDirectory::TextureMappings[textureName].id;
            // UV coordinates are often (0.0f, 0.0f) and (1.0f, 1.0f) to display an entire textures.
            // Here are trying to display only a 32x32 pixels area of the texture, hence the UV computation.
            // Read about UV coordinates here: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
            ImGui::PushID(textureId);

            if (textureId > 0)
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
            ImVec2 size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
            ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32.0f / my_tex_h);    // UV coordinates for (32,32) in our texture
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

            if (ImGui::ImageButton("", my_tex_id, size, uv0, uv1, bg_col, tint_col))
            {
				current_displayed_image = textureName;
                show_image_display = !show_image_display;
            }

            ImGui::Text(textureMapping.name.c_str());

            ImageDisplay(TextureDirectory::TextureMappings[textureName]);

            if (ImGui::Button("Insert"))
            {
				TextureDirectory::TextureMappings[textureName].SetTextureMapping(textureName);
            }

            if (textureId > 0)
                ImGui::PopStyleVar();
            ImGui::PopID();
            //ImGui::SameLine();
            textureId++;
        }
        //ImGui::NewLine();
}

static void Editor::FloorTextureDisplayer()
{

    ImGuiIO& io = ImGui::GetIO();

    // Regular user code should never have to care about TexData-> fields, but since we want to display the entire texture here, we pull Width/Height from it.
    float my_tex_w = (float)io.Fonts->TexData->Width;
    float my_tex_h = (float)io.Fonts->TexData->Height;

    ImGui::TextWrapped("");
    int textureId = 0;

    for (auto textureKey = TextureDirectory::FloorTextureMappings.begin(); textureKey != TextureDirectory::FloorTextureMappings.end(); textureKey++)
    {

        std::string     textureName = textureKey->first;
        TextureMapping  textureMapping = textureKey->second;

        ImTextureID     my_tex_id = TextureDirectory::FloorTextureMappings[textureName].id;
        ImGui::PushID(textureId);

        if (textureId > 0)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
        ImVec2 size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
        ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
        ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32.0f / my_tex_h);    // UV coordinates for (32,32) in our texture
        ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

        if (ImGui::ImageButton("", my_tex_id, size, uv0, uv1, bg_col, tint_col))
        {
            current_displayed_image = textureName;
            show_image_display = !show_image_display;
        }

        ImGui::Text(textureMapping.name.c_str());

        ImageDisplay(TextureDirectory::FloorTextureMappings[textureName]);

        if (ImGui::Button("Insert"))
        {
            TextureDirectory::FloorTextureMappings[textureName].SetTextureMapping(textureName);
        }

        if (textureId > 0)
            ImGui::PopStyleVar();
        ImGui::PopID();
        //ImGui::SameLine();
        textureId++;
    }
    ImGui::NewLine();
}


void Editor::ImageDisplay(TextureMapping texture)
{

    if (show_image_display && texture.name == current_displayed_image)
    {

        const char* textureName = texture.name.c_str();
        
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        
        ImGui::Text(textureName);

        ImGui::Image(texture.id, ImVec2(512.f, 512.f), ImVec2(0., 0.), ImVec2(1., 1.));

        if (ImGui::Button("Close Me"))
            show_image_display = !show_image_display;
        ImGui::End();
    }
}