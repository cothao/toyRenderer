#include "editor.h"
#include "../api/api.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../modelDirectory/modelDirectory.h"
//#include "../textureDirectory/textureDirectory.h"
#include "../renderer/renderer.h"

namespace Editor
{
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
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", ObjectContext::roughness, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("metallic", ObjectContext::metallic, 0.0f, 1.0f);
        ImGui::ColorEdit3("albedo", (float*)ObjectContext::albedo); // Edit 3 floats representing a color
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color


        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        DemoWindowWidgetsImages();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / (*io).Framerate, (*io).Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
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
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {} // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

}

void Editor::Config()
{
    //IMGUI_DEMO_MARKER("Examples/Menu");
    ImGui::MenuItem("(demo menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
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
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                Config();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    //IMGUI_DEMO_MARKER("Examples/Menu/Options");
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Borders);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    //IMGUI_DEMO_MARKER("Examples/Menu/Colors");
    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        //IMGUI_DEMO_MARKER("Examples/Menu/Append to an existing menu");
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
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
        ImGui::TextWrapped(
            "Below we are displaying the font texture (which is the only texture we have access to in this demo). "
            "Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. "
            "Hover the texture for a zoomed view!");

        // Below we are displaying the font texture because it is the only texture we have access to inside the demo!
        // Read description about ImTextureID/ImTextureRef and FAQ for details about texture identifiers.
        // If you use one of the default imgui_impl_XXXX.cpp rendering backend, they all have comments at the top
        // of their respective source file to specify what they are using as texture identifier, for example:
        // - The imgui_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer.
        // - The imgui_impl_opengl3.cpp renderer expect a GLuint OpenGL texture identifier, etc.
        // So with the DirectX11 backend, you call ImGui::Image() with a 'ID3D11ShaderResourceView*' cast to ImTextureID.
        // - If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers
        //   to ImGui::Image(), and gather width/height through your own functions, etc.
        // - You can use ShowMetricsWindow() to inspect the draw data that are being passed to your renderer,
        //   it will help you debug issues if you are confused about it.
        // - Consider using the lower-level ImDrawList::AddImage() API, via ImGui::GetWindowDrawList()->AddImage().
        // - Read https://github.com/ocornut/imgui/blob/master/docs/FAQ.md
        // - Read https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples

        // Grab the current texture identifier used by the font atlas.
        //ImTextureRef my_tex_id = io.Fonts->TexRef;


        // Regular user code should never have to care about TexData-> fields, but since we want to display the entire texture here, we pull Width/Height from it.
        float my_tex_w = (float)io.Fonts->TexData->Width;
        float my_tex_h = (float)io.Fonts->TexData->Height;

        ImGui::TextWrapped("And now some textured buttons..");
        int textureId = 0;

        for (auto textureKey = TextureDirectory::TextureMappings.begin(); textureKey != TextureDirectory::TextureMappings.end(); textureKey++)
        {

            std::string     textureName = textureName;
			TextureMapping  textureMapping = textureKey->second;

            //if (textureKey->second)	    std::cout << "Texture Key: " << textureName << std::endl;
            //std::cout << textureKey->second << "\n";
            ImTextureID     my_tex_id = TextureDirectory::TextureMappings[textureName].id;
            // UV coordinates are often (0.0f, 0.0f) and (1.0f, 1.0f) to display an entire textures.
            // Here are trying to display only a 32x32 pixels area of the texture, hence the UV computation.
            // Read about UV coordinates here: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
            ImGui::PushID(textureId);

            if (textureId > 0)
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(textureId - 1.0f, textureId - 1.0f));
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