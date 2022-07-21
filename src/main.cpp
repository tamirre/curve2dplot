// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

// HACK: so that stdint.h does not get included twice...
#define _MSC_STDINT_H_

#include "math.h"
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
#include <cstdlib>
#include<time.h>
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

void listFilesInDirectory(const char* path)
{
    ImVector<char*> fileList;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
        {
            fileList.push_back(ent->d_name);
            // printf("%s\n", ent->d_name);
        }
        
        closedir(dir);
    } else {
        /* could not open directory */
        // perror ("");
        // return EXIT_FAILURE;
    }
    for (int i = 0; i < fileList.Size; i++)
    {
        printf("%s\n", fileList[i]);
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
            listFilesInDirectory(filePathName.c_str());
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

struct Curve
{
    int size = 101;
    float x[101];
    float y[101];
};

// void appendPlotList()
// {

// }

int main(int, char**)
{

  
    // for(int i = 0; i < 4; i++)
        // cout <<  << " ";
 
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
    bool plotFlag = false;
    static ImVector<Curve> plotList;
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
        // NAVIGATION WINDOW SECTION
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
                plotFlag = true;
            }

            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                // NOTE(Tamir:) Create child such that the scroll bar is only scrolling the file browser, not the entire window
                ImGui::BeginChild("ChildBelow", ImVec2(ImGui::GetContentRegionAvail().x,ImGui::GetContentRegionAvail().y), false, window_flags);
                
                const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
                const float TEXT_BASE_HEIGHT = ImGui::CalcTextSize("A").y;
                static ImGuiTableFlags flags = ImGuiTableFlags_BordersV |
                                               ImGuiTableFlags_BordersOuterH |
                                               ImGuiTableFlags_Resizable |
                                               // TODO: Reordering not working with row spacing yet
                                               // ImGuiTableFlags_Reorderable | 
                                               ImGuiTableFlags_RowBg |
                                               ImGuiTableFlags_NoBordersInBody;

                ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5);
                if (ImGui::BeginTable("table_scrolly", 2, flags, outer_size))
                {
                    // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                    ImGui::TableHeadersRow();

                    // Simple storage to output a dummy file-system.
                    struct MyTreeNode
                    {
                        const char*     Name;
                        int             Size;
                        int             ChildIdx;
                        int             ChildCount;
                        bool            checkboxState;

                        // Helper class to easy setup a text filter.
                        // You may want to implement a more feature-full filtering scheme in your own application.

                        static void DisplayNode(MyTreeNode* node, MyTreeNode* all_nodes)
                        {
                            const bool is_folder = (node->ChildCount > 0);
                            if (is_folder)
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                bool open = ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_SpanFullWidth |
                                                                          ImGuiTreeNodeFlags_DefaultOpen);
                                ImGui::TableNextColumn();
                                ImGui::TextDisabled("--");
                                // ImGui::TableNextColumn();
                                // ImGui::TextUnformatted(node->Type);
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
                                    ImGui::TableNextRow();
                                    ImGui::TableNextColumn();

                                    int nodeIdx = (int)(node - all_nodes);
                                    std::string strNodeIdx = std::string("##") + std::to_string(nodeIdx);

                                    ImGui::Checkbox(strNodeIdx.c_str(), &node->checkboxState);
                                    ImGui::SameLine(1.0f, ImGui::GetCursorPosX());

                                    ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf |
                                                                  ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                                                  ImGuiTreeNodeFlags_SpanFullWidth);

                                    ImGui::TableNextColumn();
                                    ImGui::Text("%d", node->Size);
                                    // ImGui::TableNextColumn();
                                    // ImGui::TextUnformatted(node->Type);
                                } else {
                                    node->checkboxState = false;
                                }
                            }
                        }
                    };

                    static MyTreeNode nodes[] =
                    {
                        { "Jobs",                           -1,       1, 3 , false   }, // 0
                        { "Music",                          -1,       5, 2 , false   }, // 1
                        { "Stuff",                          -1,       7, 3 , false   }, // 1
                        { "Textures",                       -1,      10, 20, false   }, // 2
                        { "desktop.ini",                    1024,    -1,-1 , false   }, // 3
                        { "File1_a.wav",                    123000,  -1,-1 , false   }, // 4
                        { "File1_b.wav",                    456000,  -1,-1 , false   }, // 5
                        { "Something.txt",                  420,     -1,-1 , false   }, // 5                        
                        { "Something.txt",                  420,     -1,-1 , false   }, // 5                        
                        { "Something.txt",                  420,     -1,-1 , false   }, // 5                        
                        { "Image001.png",                   203128,  -1,-1 , false   }, // 6
                        { "Copy of Image001.png",           203256,  -1,-1 , false   }, // 7
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }, // 8
                        { "Copy of Image001 (Final2).png",  203512,  -1,-1 , false   }  // 8
                    };

                    MyTreeNode::DisplayNode(&nodes[0], nodes);

                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

            ImGui::End();
        }

        // PLOT WINDOW SECTION
        {
            ImGui::Begin("Plot");
            
            static ImVector<int> active_tabs;
            static int next_tab_id = 0;
            // if (next_tab_id == 0) // Initialize with some default tabs
            //     for (int i = 0; i < 3; i++)
            //         active_tabs.push_back(next_tab_id++);

            // TabItemButton() and Leading/Trailing flags are distinct features which we will demo together.
            // (It is possible to submit regular tabs with Leading/Trailing flags, or TabItemButton tabs without Leading/Trailing flags...
            // but they tend to make more sense together)
            // static bool show_leading_button = true;
            static bool show_trailing_button = true;
            // ImGui::Checkbox("Show Leading TabItemButton()", &show_leading_button);
            // ImGui::Checkbox("Show Trailing TabItemButton()", &show_trailing_button);

            // Expose some other flags which are useful to showcase how they interact with Leading/Trailing tabs
            static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs |
                                                    ImGuiTabBarFlags_Reorderable |
                                                    ImGuiTabBarFlags_TabListPopupButton |
                                                    ImGuiTabBarFlags_FittingPolicyResizeDown;
            // ImGui::CheckboxFlags("ImGuiTabBarFlags_TabListPopupButton", &tab_bar_flags, ImGuiTabBarFlags_TabListPopupButton);
            // if (ImGui::CheckboxFlags("ImGuiTabBarFlags_FittingPolicyResizeDown", &tab_bar_flags, ImGuiTabBarFlags_FittingPolicyResizeDown))
            //     tab_bar_flags &= ~(ImGuiTabBarFlags_FittingPolicyMask_ ^ ImGuiTabBarFlags_FittingPolicyResizeDown);
            // if (ImGui::CheckboxFlags("ImGuiTabBarFlags_FittingPolicyScroll", &tab_bar_flags, ImGuiTabBarFlags_FittingPolicyScroll))
                // tab_bar_flags &= ~(ImGuiTabBarFlags_FittingPolicyMask_ ^ ImGuiTabBarFlags_FittingPolicyScroll);

            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                // Demo a Leading TabItemButton(): click the "?" button to open a menu
                // if (show_leading_button)
                //     if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                //         ImGui::OpenPopup("MyHelpMenu");
                // if (ImGui::BeginPopup("MyHelpMenu"))
                // {
                //     ImGui::Selectable("Hello!");
                //     ImGui::EndPopup();
                // }

                // Demo Trailing Tabs: click the "+" button to add a new tab (in your app you may want to use a font icon instead of the "+")
                // Note that we submit it before the regular tabs, but because of the ImGuiTabItemFlags_Trailing flag it will always appear at the end.
                // if (show_trailing_button)
                //     if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
                //         active_tabs.push_back(next_tab_id++); // Add new tab
                //         appendPlotList(plotList);

                // Submit tab item on plot button press
                if (plotFlag)
                {
                    Curve tmp;
                    // srand(time(0));
                    // static float xs1[tmp_size], ys1[tmp_size];
                    for (int i = 0; i < tmp.size; ++i) {
                        tmp.x[i] = i * 1.0f/(tmp.size-1);
                        tmp.y[i] = next_tab_id;
                    }
                    plotList.push_back(tmp);
                    active_tabs.push_back(next_tab_id++);
                    plotFlag = false;
                }
                            
                // Submit our regular tabs
                for (int n = 0; n < active_tabs.Size; )
                {
                    bool open = true;
                    char name[16];
                    snprintf(name, IM_ARRAYSIZE(name), "%04d", active_tabs[n]);
                    
                    if (ImGui::BeginTabItem(name, &open, ImGuiTabItemFlags_None))
                    {

                        const ImVec2 plotSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
                        if(ImPlot::BeginPlot("", plotSize))
                        {
                            ImPlot::SetupAxes("x","y");
                            ImPlot::SetupFinish();

                            std::string functionName = std::string("f") + std::to_string(active_tabs[n]) + std::string("(x)");
                            ImPlot::PlotLine(functionName.c_str(), plotList[n].x, plotList[n].y, plotList[n].size);
                            // ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                            // ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);
                            ImPlot::EndPlot();
                        }
                         
                        ImGui::EndTabItem();
                    }

                    if (!open)
                    {
                        plotList.erase(plotList.Data + n);
                        active_tabs.erase(active_tabs.Data + n);
                    }
                    else
                    {
                        n++;
                    }
                }

                ImGui::EndTabBar();
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
        // Sleep(100);
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
