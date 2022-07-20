// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

// HACK: so that stdint.h does not get included twice...
#define _MSC_STDINT_H_

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include "dirent.h"
#include "implot.h"
// #include "ImGuiFileBrowser.h"
#include "ImGuiFileDialog.h"

#include <stdio.h>
// For testing
#include <iostream>
using namespace std;
static bool show_filedialog = false;

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void listFilesInDirectory(char* path)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir(dir)) != NULL) {
        printf("%s\n", ent->d_name);
    }
    closedir(dir);
    } else {
        /* could not open directory */
        // perror ("");
        // return EXIT_FAILURE;
    }
}

void openFileDialog(bool *p_open)
{
    ImVec2 maxSize = ImVec2(800.0, 400.0);  // The full display area
    ImVec2 minSize = ImVec2(400.0, 200.0); // Half the display area

    ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose a Directory", nullptr, ".", 0);
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) 
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
        }

        // close
        ImGuiFileDialog::Instance()->Close();
        *p_open=false;
    }
}

void showMainMenu()
{
    if(ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Directory...", "CTRL+O", &show_filedialog))
            {
                
            }
            if (ImGui::MenuItem("Add Directory...", "CTRL+D", &show_filedialog))
            {
                
            }
            
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "XPlot", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
 
    // Our state
    bool show_demo_window = true;

    // bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.086f, 0.086f, 0.086f, 1.00f);
    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
    
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show menu bar for opening/adding a directory
        showMainMenu();

        // Make docspace in main window
        ImGui::DockSpaceOverViewport();
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_filedialog) openFileDialog(&show_filedialog);
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
            ImPlot::ShowDemoWindow(&show_demo_window);    
        }
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Navigation");                          // Create a window called "Hello, world!" and append into it.
            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


            static ImGuiTextFilter filter;
            filter.Draw("Search");
            ImGui::SameLine();
            HelpMarker(
                "Input regular expression search string here."
            );
            ImGui::SameLine();
            if(ImGui::Button("Plot"))
            {
                // TBI
            }
            
            const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
            const float TEXT_BASE_HEIGHT = ImGui::CalcTextSize("A").y;
            static ImGuiTableFlags flags = ImGuiTableFlags_BordersV |
                                           ImGuiTableFlags_BordersOuterH |
                                           ImGuiTableFlags_Resizable |
                                           ImGuiTableFlags_RowBg |
                                           ImGuiTableFlags_NoBordersInBody;

            ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5);
            if (ImGui::BeginTable("table_scrolly", 3, flags, outer_size))
            // if (ImGui::BeginTable("table_scrolly", 3, flags))
            {
                // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
                ImGui::TableHeadersRow();

                // Simple storage to output a dummy file-system.
                struct MyTreeNode
                {
                    const char*     Name;
                    const char*     Type;
                    int             Size;
                    int             ChildIdx;
                    int             ChildCount;

                    // Helper class to easy setup a text filter.
                    // You may want to implement a more feature-full filtering scheme in your own application.

                    static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        const bool is_folder = (node->ChildCount > 0);
                        if (is_folder)
                        {
                            bool open = ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_SpanFullWidth |
                                                                      ImGuiTreeNodeFlags_DefaultOpen);
                            ImGui::TableNextColumn();
                            ImGui::TextDisabled("--");
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(node->Type);
                            if (open)
                            {
                                for (int child_n = 0; child_n < node->ChildCount; child_n++)
                                {
                                    DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
                                }
                                ImGui::TreePop();
                            }
                        }
                        else
                        {
                            if(filter.PassFilter(node->Name))
                            {
                                ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf |
                                                              ImGuiTreeNodeFlags_Bullet |
                                                              ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                                              ImGuiTreeNodeFlags_SpanFullWidth);
                                ImGui::TableNextColumn();
                                ImGui::Text("%d", node->Size);
                                ImGui::TableNextColumn();
                                ImGui::TextUnformatted(node->Type);
                            }
                        }
                    }
                };
                int n = 3;
                static const MyTreeNode nodes[] =
                {
                    { "Jobs",                         "Folder",       -1,       1, n    }, // 0
                    { "Music",                        "Folder",       -1,       5, 2    }, // 1
                    { "Stuff",                        "Folder",       -1,       7, 3    }, // 1
                    { "Textures",                     "Folder",       -1,      10, 20    }, // 2
                    { "desktop.ini",                  "System file",  1024,    -1,-1    }, // 3
                    { "File1_a.wav",                  "Audio file",   123000,  -1,-1    }, // 4
                    { "File1_b.wav",                  "Audio file",   456000,  -1,-1    }, // 5
                    { "Something.txt",                "Text file",    420,     -1,-1    }, // 5                        
                    { "Something.txt",                "Text file",    420,     -1,-1    }, // 5                        
                    { "Something.txt",                "Text file",    420,     -1,-1    }, // 5                        
                    { "Image001.png",                 "Image file",   203128,  -1,-1    }, // 6
                    { "Copy of Image001.png",         "Image file",   203256,  -1,-1    }, // 7
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                    { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
                };

                MyTreeNode::DisplayNode(&nodes[0], nodes);

                ImGui::EndTable();
            }

            ImGui::End();
        }
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        //glUseProgram(last_program);

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
