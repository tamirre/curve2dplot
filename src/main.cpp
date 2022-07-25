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

// For testing:
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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

// helper function to split a string
std::string tokenize(std::string s)
{

    std::string del = "\\";
    int start = 0;
    int end;
    // NOTE(Tamir): To avoid checking the OS
    // Try windows first
    if (s.find(del) != std::string::npos)
    {
        end = s.find(del);
    }
    // Then try linux...
    else
    {
        end = s.find(std::string("/"));
        del = "/";
    }
    while (end != -1) {
        // cout << s.substr(start, end - start) << endl;
        start = end + del.size();
        end = s.find(del, start);
    }

    return s.substr(start, start-end);
}

struct Curve
{
    std::vector<double> x;
    std::vector<double> y;
};

struct Node {
    std::string Name;
    std::string pathName;
    int Index;
    int Size;
    bool checkboxState;
    Node *parent; 
    std::vector<Node> children;
    Curve curve;

    static void DisplayNodes(Node &root, ImGuiTextFilter filter)
    {
        const bool is_folder = (root.children.size() > 0);
        if (is_folder)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            bool open = ImGui::TreeNodeEx(root.Name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth |
                                                              ImGuiTreeNodeFlags_DefaultOpen);
            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");
            if (open)
            {

                for (long unsigned int j = 0; j < root.children.size(); j++)
                {
                    DisplayNodes(root.children[j], filter);
                }

                ImGui::TreePop();
            }
        }
        else
        {                        
            if(filter.PassFilter(root.Name.c_str()))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int nodeIdx = (int)(root.Index);
                std::string strNodeIdx = std::string("##") + std::to_string(nodeIdx);

                ImGui::Checkbox(strNodeIdx.c_str(), &root.checkboxState);
                // ImGui::Checkbox("##", &node->checkboxState);
                float rowSpacing = ImGui::GetCursorPosX() + 5.0f;
                ImGui::SameLine();
                ImGui::SetCursorPosX(rowSpacing);
                // ImGui::SameLine(1.0f, ImGui::GetCursorPosX());

                ImGui::TreeNodeEx(root.Name.c_str(), ImGuiTreeNodeFlags_Leaf |
                                                      ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                                      ImGuiTreeNodeFlags_SpanFullWidth);

                ImGui::TableNextColumn();
                float sizeDisplay;
                if (root.Size < 1024)
                {
                    ImGui::Text("%d B", root.Size);
                } else if (root.Size < 1024*1024)
                {
                    sizeDisplay = (float)(root.Size)/1024.0f; 
                    ImGui::Text("%.2f KB", sizeDisplay);
                } else 
                {
                    sizeDisplay = (float)(root.Size)/(1024.0f*1024.0f); 
                    ImGui::Text("%.2f MB", sizeDisplay);
                }
            } else {
                root.checkboxState = false;
            }
        }
    }
};

Curve readCurve(std::string filePath)
{
    Curve curve;

    std::ifstream file;
    std::string line;
    double x,y;
    file.open(filePath);
    if (file.is_open())
    {
        while(getline(file,line))
        {
            // Skip comments marked by # character
            if (line[0] == '#') continue;
            file >> x >> y;
            curve.x.push_back(x);
            curve.y.push_back(y);
        }    
    }
    else std::cout << "Unable to open file"; 
    file.close();

    // std::cout.precision(8);
    // for(int i = 0; i<curve.x.size(); i++)
    //     std::cout << "X: " << std::fixed << curve.x[i] << ", Y: " << std::fixed << curve.y[i] << std::endl;
    
    return curve;
}

Node listFilesInDirectory(std::string path)
{
    int id = 0;
    Node root = {path, ".", id, -1, false, NULL};
    std::vector<Node> children;
    DIR* dir;
    struct dirent * ent;
    struct stat st;

    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if(ent->d_type == DT_REG)
            {

                std::string filePath = path + std::string("/") + ent->d_name;
// #if _WIN32
                std::string filePathName = tokenize(path) + std::string("/") + ent->d_name;
// #elif __linux__
//                 std::string filePathName = tokenize(path, "/") + std::string("/") + ent->d_name;
// #endif
                stat(filePath.c_str(), &st);
                int size = st.st_size;

                Node child = {ent->d_name, filePathName, id++, size, false, &root};
                child.curve = readCurve(filePath);
                
                children.push_back(child);
            // } else if(dp->d_type == DT_DIR) {
            }
        }
    } else {

    }
    closedir(dir);
    root.children = children;

    // printTree(&root);
    return root;
}

void printNode(const Node *node) {
    if (node) {
        std::cout << "Name: " << node->Name << std::endl;
        std::cout << "Index: " << node->Index << std::endl;
        if (node->parent) {
            std::cout << "Parent: " << node->parent->Name << std::endl;
        }
        // if (node->children) {
            std::cout << "Children: " << std::endl;
            for (long unsigned int i = 0; i < node->children.size(); i++) {
                std::cout << node->children[i].Name << std::endl;
            }
        // }
    }
}

void openFileDialog(bool *p_open, std::vector<Node> &Tree)
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

            // NOTE: Attempts at multiselection:
            // std::string userDatas;
            // if (ImGuiFileDialog::Instance()->GetUserDatas())
            // {
            //     userDatas = std::string((const char*)ImGuiFileDialog::Instance()->GetUserDatas());
            // }
            // auto selection = ImGuiFileDialog::Instance()->GetSelection(); // multiselection
            // std::cout << typeid(selection).name() << std::endl;
            
            Tree.push_back(listFilesInDirectory(filePathName));
        }

        // close
        ImGuiFileDialog::Instance()->Close();
        *p_open=false;
    }

    // return nodes;
}

void showMainMenu()
{

}

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
    /////////////////////////////////////////////////////////
    // ImFontConfig config;
    // config.OversampleH = 3;
    // config.OversampleV = 3;

    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("imgui-docking/misc/fonts/Roboto-Medium.ttf", 16.0f, &config);
    // io.Fonts->AddFontFromFileTTF("imgui-docking/misc/fonts/Cousine-Regular.ttf", 15.0f, &config);
    // io.Fonts->AddFontFromFileTTF("imgui-docking/misc/fonts/DroidSans.ttf", 16.0f, &config);
    // io.Fonts->AddFontFromFileTTF("imgui-docking/misc/fonts/ProggyTiny.ttf", 10.0f, &config);
    // io.Fonts->AddFontFromFileTTF("imgui-docking/misc/fonts/SourceCodePro-Regular.ttf", 16.0f, &config);

    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != NULL);
 
    // Our state
    bool show_demo_window = true;
    bool show_filedialog = false;
    bool plotFlag = false;
    // static ImVector<Curve> plotList;
    std::vector<Node> Tree;
    
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
        if(ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open Directory...", "CTRL+O", &show_filedialog))
                {
                    // openFileDialog(&show_filedialog, nodes, parentCntr);
                }
                if (ImGui::MenuItem("Add Directory...", "CTRL+D", &show_filedialog))
                {

                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Make docspace in main window
        ImGui::DockSpaceOverViewport();
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if(show_filedialog) openFileDialog(&show_filedialog, nodes, parentCntr);
        if(show_filedialog) openFileDialog(&show_filedialog, Tree);
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
                                               ImGuiTableFlags_Reorderable | 
                                               ImGuiTableFlags_RowBg |
                                               ImGuiTableFlags_NoBordersInBody;

                ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5);
                if (ImGui::BeginTable("table_scrolly", 2, flags, outer_size))
                {
                    // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                    ImGui::TableHeadersRow();

                    for (long unsigned int i = 0; i < Tree.size(); i++)
                    {
                        Node::DisplayNodes(Tree[i], filter);
                    }

                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

            ImGui::End();
        }

        // PLOT WINDOW SECTION
        {
            ImGui::Begin("Plot");

            static bool show_trailing_button = true;
            static ImVector<int> active_tabs;
            static int next_tab_id = 0;

            // TabItemButton() and Leading/Trailing flags are distinct features which we will demo together.
            // (It is possible to submit regular tabs with Leading/Trailing flags, or TabItemButton tabs without Leading/Trailing flags...
            // but they tend to make more sense together)
            // static bool show_leading_button = true;

            // ImGui::Checkbox("Show Leading TabItemButton()", &show_leading_button);
            // ImGui::Checkbox("Show Trailing TabItemButton()", &show_trailing_button);

            // Expose some other flags which are useful to showcase how they interact with Leading/Trailing tabs
            static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs |
                                                    // TODO: Reordering not working with implot, 
                                                    //       dragging the plot reorders the tabs...
                                                    // ImGuiTabBarFlags_Reorderable |
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
                if (show_trailing_button)
                    if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
                        active_tabs.push_back(next_tab_id++); // Add new tab
                        // appendPlotList(plotList);

                // Submit tab item on plot button press
                if (plotFlag)
                {
                //     // Curve tmp;
                //     // // srand(time(0));
                //     // // static float xs1[tmp_size], ys1[tmp_size];
                //     // for (int i = 0; i < tmp.x.size(); ++i) {
                //     //     tmp.x[i] = i * 1.0f/(tmp.x.size()-1);
                //     //     tmp.y[i] = next_tab_id;
                //     // }
                //     plotList.push_back(tmp);
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

                            for (long unsigned int i = 0; i < Tree.size(); i++)
                            {
                                for (long unsigned int j = 0; j < Tree[i].children.size(); j++)
                                {
                                    // std::string functionName = std::to_string(Tree.children[i].Name);
                                    if(Tree[i].children[j].checkboxState == true)
                                    {
                                        double* x = &Tree[i].children[j].curve.x[0];
                                        double* y = &Tree[i].children[j].curve.y[0];
                                        ImPlot::PlotLine(Tree[i].children[j].pathName.c_str(),
                                                         x,
                                                         y,
                                                         Tree[i].children[j].curve.x.size()-1); // length-1 for correct size
                                    }

                                }
                            }
                            
                            ImPlot::EndPlot();
                        }
                         
                        ImGui::EndTabItem();
                    }

                    if (!open)
                    {
                        // plotList.erase(plotList.Data + n);
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
