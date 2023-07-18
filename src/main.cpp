// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

// HACK: so that stdint.h does not get included twice...
#define _MSC_STDINT_H_

// NOTE: Disabling command window spawn when running the .exe
#if _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

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
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

// // Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
// #ifndef IMGUI_CDECL
// #ifdef _MSC_VER
// #define IMGUI_CDECL __cdecl
// #else
// #define IMGUI_CDECL
// #endif
// #endif

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

struct Tab
{
    int Index;
    std::vector<int> activePlots;
};

struct Curve
{
    // Data:
    std::vector<double> x;
    std::vector<double> y;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double mean;
    double deltaX = 1.0;

    // Options:
    ImVec4 color = ImPlot::GetColormapColor(0);
    float thickness = 1.0f;
    bool markerFlag = false;
    bool showMeanFlag = false;
    ImPlotMarker marker;
    float markerSize = 1.0f;
    float markerWeight = 1.0f;
    
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

    static int compare(const void * lhs, const void * rhs)
    {
        const Node* a = (const Node*)lhs;
        const Node* b = (const Node*)rhs;
        return (strcmp(a->Name.c_str(), b->Name.c_str()));
        // return (a->Index - b->Index);
        // return 1;
    }
    
    static Node listFilesInDirectory(std::string path, std::string &lastPath, int &dirCntr)
    {
        DIR* dir;
        struct dirent * ent;
        struct stat st;

        if ((dir = opendir(path.c_str())) != NULL)
        {
            int id = 0;

            // Replace windows slash in path for display
            const char winSlash = '\\';
            const char slash = '/';
            std::replace(path.begin(), path.end(), winSlash, slash);
            Node root = {path, ".", dirCntr++, -1, false, NULL};
            std::vector<Node> children;
            while ((ent = readdir(dir)) != NULL)
            {
                if(ent->d_type == DT_REG)
                {

                    std::string filePath = path + std::string("/") + ent->d_name;
                    lastPath = path + std::string("/");
    // #if _WIN32
                    std::string filePathName = tokenize(path) + std::string("/") + ent->d_name;
    // #elif __linux__
    //                 std::string filePathName = tokenize(path, "/") + std::string("/") + ent->d_name;
    // #endif
                    stat(filePath.c_str(), &st);
                    int size = st.st_size;

                    // Node child = {ent->d_name, filePathName, id++, size, false, &root};

                    Node child = {ent->d_name, filePath, id++, size, false, &root};
                    // child.curve = readCurve(filePath);

                    children.push_back(child);
                // } else if(dp->d_type == DT_DIR) {
                }
            }
            // TODO: sort children before adding them
            // qsort(&children, children.size(), sizeof(children[0]), Node::compare);
            // TODO: std::move?
            root.children = children;

            // printTree(&root);
            closedir(dir);
            return root;
        } else {
            Node root = {"Invalid Directory", ".",  dirCntr++, -1, false, NULL};
            std::vector<Node> children;
            Node childDummy = {"", ".", 0, 0, false, NULL};
            children.push_back(childDummy);
            root.children = children;
            return root;
        }

    }

    
    static void DisplayNodes(Node &root, ImGuiTextFilter filter, std::vector<Node> &Tree, std::string &lastPath, int &dirCntr)
    {
        const bool is_folder = (root.children.size() > 0);
        if (is_folder)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            bool open = ImGui::TreeNodeEx(root.Name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth |
                                                             ImGuiTreeNodeFlags_DefaultOpen);

            if (ImGui::BeginPopupContextItem(root.Name.c_str())) // <-- use last item id as popup id
            {
                // ImGui::Text("This a popup for \"%s\"!", root.Name.c_str());
                ImGui::Text("Action:");
                if (ImGui::Button("Reload"))
                {
                    int tmpIndex = root.Index;
                    Node dirRoot = Node::listFilesInDirectory(root.Name, lastPath, tmpIndex);
                    Tree.erase(Tree.begin() + root.Index);
                    dirCntr = 0;
                    for(long unsigned int i = 0; i < Tree.size(); i++)
                    {
                        Tree[i].Index = dirCntr++;
                    }
                    Tree.insert(Tree.begin() + (--tmpIndex), dirRoot);
                    dirCntr++;

                    ImGui::CloseCurrentPopup();
                }
                 
                if (ImGui::Button("Remove"))
                {
                    Tree.erase(Tree.begin() + root.Index);
                    dirCntr = 0;
                    for(long unsigned int i = 0; i < Tree.size(); i++)
                    {
                        Tree[i].Index = dirCntr++;
                    }

                    ImGui::CloseCurrentPopup();
                }

                // if (ImGui::Button("Close"))
                //     ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Right-click for options");
            // ImGui::TextDisabled("--");
            // ImGui::TableNextColumn();
            // ImGui::Button("Options");
            ImGui::TableNextColumn();

            // // POPUPTEST
            // static int selected_fish = -1;
            // const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
            // static bool toggles[] = { true, false, false, false, false };

            // // Simple selection popup (if you want to show the current selection inside the Button itself,
            // // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
            // if (ImGui::Button("Select.."))
            //     ImGui::OpenPopup("my_select_popup");
            // ImGui::SameLine();
            // ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
            // if (ImGui::BeginPopup("my_select_popup"))
            // {
            //     ImGui::Text("Aquarium");
            //     ImGui::Separator();
            //     for (int i = 0; i < IM_ARRAYSIZE(names); i++)
            //         if (ImGui::Selectable(names[i]))
            //             selected_fish = i;
            //     ImGui::EndPopup();
            // }
            if (open)
            {

                for (long unsigned int j = 0; j < root.children.size(); j++)
                {
                    Node::DisplayNodes(root.children[j], filter, Tree, lastPath, dirCntr);
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

double calculateMean(std::vector<double> y, double xmin, double xmax, double deltax)
{
    double mean = 0.0;
    std::vector<double>(y.begin()+xmin/deltax, y.begin()+xmax/deltax).swap(y);
    int newSize = y.size();
    for(double elem : y)
    {
        mean += elem;
    }
    mean = mean * deltax / (xmax - xmin);
    return mean;
}
              
Curve readCurve(std::string filePath)
{
    Curve curve;

    std::ifstream file;
    std::string line;
    double x,y;
    file.open(filePath);
    
    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            if (line[0] != '#')
            {
                std::istringstream iss(line);
                iss >> x >> y;
                curve.x.push_back(x);
                curve.y.push_back(y);
            }
        }
        file.close();
    }
    else std::cout << "Unable to open file"; 

    curve.xmax = *std::max_element(curve.x.begin(), curve.x.end());
    curve.xmin = *std::min_element(curve.x.begin(), curve.x.end());

    curve.ymax = *std::max_element(curve.y.begin(), curve.y.end());
    curve.ymin = *std::min_element(curve.y.begin(), curve.y.end());

    curve.deltaX = curve.x[1] - curve.x[0];
    
    // std::cout << "SIZE: " << curve.x.size() << std::endl;
    // std::cout.precision(8);
    // for(int i = 0; i < curve.x.size(); i++)
    //     std::cout << "X: " << std::fixed << curve.x[i] << ", Y: " << std::fixed << curve.y[i] << std::endl;
    
    return curve;
}

// void printNode(const Node *node) {
//     if (node) {
//         std::cout << "Name: " << node->Name << std::endl;
//         std::cout << "Index: " << node->Index << std::endl;
//         if (node->parent) {
//             std::cout << "Parent: " << node->parent->Name << std::endl;
//         }
//         // if (node->children) {
//             std::cout << "Children: " << std::endl;
//             for (long unsigned int i = 0; i < node->children.size(); i++) {
//                 std::cout << node->children[i].Name << std::endl;
//             }
//         // }
//     }
// }

void openFileDialog(bool *p_open, std::vector<Node> &Tree, std::string &lastPath, int &dirCntr)
{
    ImVec2 maxSize = ImVec2(800.0, 400.0);  // The full display area
    ImVec2 minSize = ImVec2(400.0, 200.0); // Half the display area

    ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose a Directory", nullptr, lastPath, 0);
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

            // Check whether curve directory already exists in the tree and add it
            bool addNode = true;
            for(long unsigned int i = 0; i < Tree.size(); i++)
            {
                if(Tree[i].Name == filePathName)
                    addNode = false;
            }
            if(addNode)
            {
                Node dirRoot = Node::listFilesInDirectory(filePathName, lastPath, dirCntr);
                if(dirRoot.children.size() != 0)
                    Tree.push_back(dirRoot);
            }
        }

        // close
        ImGuiFileDialog::Instance()->Close();
        *p_open=false;
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
    static ImVector<Tab> activeTabs;
    static int nextTabId = 0;
    static std::string lastPath = ".";
    static char xAxisLabelText[128] = "";
    static char yAxisLabelText[128] = "";
    // static char xAxisMinText[128] = "";
    // static char xAxisMaxText[128] = "";
    static double xAxisMin = 0.0;
    static double xAxisMax = 1.0;
    bool show_demo_window = false;
    bool show_filedialog = false;
    bool show_scandialog = false;
    bool show_styleEditor = false;
    bool show_helpWindow = false;
    bool quit_application = false;
    bool plotFlag = false;
    bool redock_all = false;
    bool reset_plots = false;
    bool addDirFlag = false;
    bool globalShowMeanFlag = false;
    int dirCntr = 0;
    
    std::vector<Node> Tree;

    // Initialize one plot tab
    Tab defaultTab = {nextTabId++};
    activeTabs.push_back(defaultTab);

    // HACK(Tamir): adding some dirs hardcoded for testing
    
    // Tree.push_back(Node::listFilesInDirectory(std::string("v:/home/tamir/projects/Mesys/Jobs/axialHousing/first-crv"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("v:/home/tamir/projects/Mesys/Jobs/radialHousing/first-crv"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("v:/home/tamir/projects/Mesys/Jobs/axialPlusRadialHousing/first-crv"), lastPath, dirCntr));


    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-fine"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-stat200-fine"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-stat200-fine-damp"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-stat200-fine-freq"), lastPath, dirCntr));

    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Jobs/setup1-100Nm-stat200-fine/first-crv"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Jobs/setup1-100Nm-stat200-fine-damp/first-crv"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Jobs/setup1-100Nm-stat200-fine-freq/first-crv"), lastPath, dirCntr));

    Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-Testbench-stat2-setup1"), lastPath, dirCntr));
    Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-stat200-fine"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-Testbench-stat1-setup6"), lastPath, dirCntr));
    Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-Testbench-stat1-setup1"), lastPath, dirCntr));
    Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/setup1-100Nm-fine"), lastPath, dirCntr));
    // Tree.push_back(Node::listFilesInDirectory(std::string("z:/ZIM-EleSim/Results/"), lastPath, dirCntr));
    
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
                ImGui::MenuItem("Open Directory...", "CTRL+O", &show_filedialog);
                ImGui::MenuItem("Scan for *-crv directories...", "CTRL+F", &show_scandialog);
                // TODO: implement recent directories list
                if(ImGui::BeginMenu("Open Recent...", "CTRL+O"))
                {
                    ImGui::MenuItem("test1-crv");
                    ImGui::MenuItem("test2-crv");
                    ImGui::MenuItem("test3-crv");

                    ImGui::EndMenu();
                }
                // TODO: implement quit
                ImGui::MenuItem("Quit...", "Alt+F4", &quit_application);
                
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Options"))
            {
                ImGui::MenuItem("Edit Style...", "CTRL+L", &show_styleEditor);
                ImGui::MenuItem("Debug...", "CTRL+D", &show_demo_window);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("About"))
            {
                ImGui::MenuItem("Help...", "CTRL+H", &show_helpWindow);
                ImGui::MenuItem("Version:", "0.1");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Make docspace in main window
        ImGui::DockSpaceOverViewport();

        if(show_filedialog) openFileDialog(&show_filedialog, Tree, lastPath, dirCntr);
        if (show_styleEditor)
        {
            ImGui::Begin("Style Editor", &show_styleEditor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }
        if (show_helpWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_Always);
            ImGui::Begin("Help", &show_helpWindow);
            ImGui::Text("For bug reports send a detailed description to:\n\ntamir.dombrovskij@ist-aachen.com");
            ImGui::End();
        }
                
        // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).        
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
            ImPlot::ShowDemoWindow(&show_demo_window);

        }
        // NAVIGATION WINDOW SECTION
        {
            ImGui::SetNextWindowSize(ImVec2(400, 700), ImGuiCond_FirstUseEver);
            ImGui::Begin("Navigation");                          
            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            static char pathInputText[128] = "";
            ImGui::InputTextWithHint("##", "Absolute path to directory...", pathInputText, IM_ARRAYSIZE(pathInputText));
            ImGui::SameLine();
            
            if(ImGui::Button("Add"))
                addDirFlag = true;
            ImGui::SameLine();
            if(ImGui::Button("Browse..."))
                show_filedialog = true;

            if(addDirFlag)
            {
                if (!(std::string(pathInputText).empty()))
                {
                    Node dirRoot = Node::listFilesInDirectory(std::string(pathInputText), lastPath, dirCntr);
                    if(dirRoot.children.size() != 0)
                        Tree.push_back(dirRoot);
                }
                addDirFlag = false;
            }
            static ImGuiTextFilter filter;
            filter.Draw("Search");
            ImGui::SameLine();
            HelpMarker(
                "Input search string here."
            );

            plotFlag = ImGui::Button("Add Plot Tab");
            ImGui::SameLine();
            redock_all = ImGui::Button("Redock all");
            ImGui::SameLine();
            reset_plots = ImGui::Button("Reset all Plots");

            // HACK(Tamir): Just use simple text fields for axis labels for now... Maybe modal/popup in plot window later
            ImGui::InputTextWithHint("##xaxis", "X-Axis Label...", xAxisLabelText, IM_ARRAYSIZE(xAxisLabelText));
            ImGui::InputTextWithHint("##yaxis", "Y-Axis Label...", yAxisLabelText, IM_ARRAYSIZE(yAxisLabelText));

            // Range selection for mean calculation
            // static char buf2[64] = ""; ImGui::InputText("decimal",     buf2, 64, ImGuiInputTextFlags_CharsDecimal);

            ImGui::InputDouble("x min", &xAxisMin);
            ImGui::InputDouble("x max", &xAxisMax);
            // ImGui::InputTextWithHint("##xaxismin", "x min...", xAxisMinText, IM_ARRAYSIZE(xAxisMinText), ImGuiInputTextFlags_CharsDecimal);
            // ImGui::InputTextWithHint("##xaxismax", "x max...", xAxisMaxText, IM_ARRAYSIZE(xAxisMaxText), ImGuiInputTextFlags_CharsDecimal);
            // ImGui::Checkbox("Markers", &Tree[i].children[j].curve.markerFlag);
            ImGui::Checkbox("Show mean", &globalShowMeanFlag);
            
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
                                               // ImGuiTableFlags_ScrollY;

                // static ImGuiTableFlags flags =
             // ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
            // | ImGuiTableFlags_ScrollY;

                ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5);
                if (ImGui::BeginTable("table_scrolly", 2, flags, outer_size))
                {
                    // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                    // ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible (only works with ScrollY flag enabled?)
                    ImGui::TableHeadersRow();

                    for (long unsigned int i = 0; i < Tree.size(); i++)
                    {
                        Node::DisplayNodes(Tree[i], filter, Tree, lastPath, dirCntr);
                    }

                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

            ImGui::End();
        }

        // PLOT WINDOW SECTION
        {
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
            ImGui::Begin("Plot");

            static bool show_trailing_button = true;
            // static bool show_leading_button = true;

            // TabItemButton() and Leading/Trailing flags are distinct features which we will demo together.
            // (It is possible to submit regular tabs with Leading/Trailing flags, or TabItemButton tabs without Leading/Trailing flags...
            // but they tend to make more sense together)


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

            // if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            // {
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
                    // if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
                //         active_tabs.push_back(next_tab_id++); // Add new tab



                // Create a DockSpace node where any window can be docked
                ImGuiID dockspace_id = ImGui::GetID("PlotDockSpace");
                ImGui::DockSpace(dockspace_id);

                // Keep track of the last focussed tab
                static int lastFocussedTab;
                                        
                // if (show_trailing_button)
                //     if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
                //         active_tabs.push_back(next_tab_id++); // Add new tab
                
                // Submit tab item on add tab button press
                if (plotFlag)
                {
                    Tab nextTab = {nextTabId++};
                    activeTabs.push_back(nextTab);
                    plotFlag = false;
                }

                // Submit our regular tabs
                for (int n = 0; n < activeTabs.Size;)
                {
                    bool open = true;
                    char name[16];
                    snprintf(name, IM_ARRAYSIZE(name), "%04d", activeTabs[n].Index);

                    // Set dockID to make tab dockable
                    ImGui::SetNextWindowDockID(dockspace_id, redock_all ? ImGuiCond_Always : ImGuiCond_FirstUseEver);

                    // if (lastFocussedTab == activeTabs[n].Index)
                    // {
                    //     for (long unsigned int i = 0; i < Tree.size(); i++)
                    //     {
                    //         for (long unsigned int j = 0; j < Tree[i].children.size(); j++)
                    //         {
                    //             for(long unsigned int k = 0; k < activeTabs[n].activePlots.size(); k++)
                    //             {
                    //                 if(Tree[i].children[j].Index == activeTabs[n].activePlots[k])
                    //                 {
                    //                     Tree[i].children[j].checkboxState = true;
                    //                 }
                    //             }
                    //         }
                    //     }
                    // }
                    activeTabs[n].activePlots.clear();
                    
                    // if (ImGui::BeginTabItem(name, &open, ImGuiTabItemFlags_None))
                    if (ImGui::Begin(name, &open))
                    {

                        const ImVec2 plotSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

                        static char   titleInput[128];
                        static bool   titleFlag = false;
                        static bool   rangesChanged = false;
                        static int    numCurves = 0;
                        static double xmin = 0;
                        static double xmax = 0;
                        static double ymin = 0;
                        static double ymax = 0;

                        const ImPlotMarker marker[] = {ImPlotMarker_Circle, ImPlotMarker_Square, ImPlotMarker_Asterisk,
                                                       ImPlotMarker_Diamond, ImPlotMarker_Cross, ImPlotMarker_Plus};
                        const char* items[] = {"Circle", "Square", "Asterisk",
                                               "Diamond", "Cross", "Plus"};
                        static int item_current = 0;
                        
                        // TODO: Reactivate once the state of the checkboxes is saved correctly
                        if(lastFocussedTab == activeTabs[n].Index && reset_plots)
                        {
                            numCurves = 0;
                            for (long unsigned int i = 0; i < Tree.size(); i++)
                            {
                                for (long unsigned int j = 0; j < Tree[i].children.size(); j++)
                                {
                                    Tree[i].children[j].checkboxState = false;
                                    Tree[i].children[j].curve.x.clear();
                                    Tree[i].children[j].curve.y.clear();
                                }
                            }
                        }

                        // Setup state, colors and axis limits in first pass
                        for (long unsigned int i = 0; i < Tree.size(); i++)
                        {
                            for (long unsigned int j = 0; j < Tree[i].children.size(); j++)
                            {
                                
                                if(Tree[i].children[j].checkboxState == true)
                                {
                                    bool indexNotSavedYet = true;
                                    for(long unsigned int k = 0; k < activeTabs[n].activePlots.size(); k++)
                                    {
                                        if(Tree[i].children[j].Index == activeTabs[n].activePlots[k])
                                        {
                                            indexNotSavedYet = false;
                                        }
                                    }
                                    if(lastFocussedTab == activeTabs[n].Index  && indexNotSavedYet)
                                    {
                                        activeTabs[n].activePlots.push_back(Tree[i].children[j].Index);
                                    }
                                    if(Tree[i].children[j].curve.x.size() == 0)
                                    {
                                        Tree[i].children[j].curve = readCurve(Tree[i].children[j].pathName);
                                        Tree[i].children[j].curve.color.w = 1.0f;
                                        Tree[i].children[j].curve.color = ImPlot::GetColormapColor(numCurves++);
                                    }

                                    if (Tree[i].children[j].curve.xmin < xmin)
                                    {
                                        xmin = Tree[i].children[j].curve.xmin;
                                        rangesChanged = true;
                                    }
                                    if (Tree[i].children[j].curve.xmax > xmax)
                                    {
                                        xmax = Tree[i].children[j].curve.xmax;
                                        rangesChanged = true;
                                    }
                                    if (Tree[i].children[j].curve.ymin < ymin)
                                    {
                                        ymin = Tree[i].children[j].curve.ymin;
                                        rangesChanged = true;
                                    }
                                    if (Tree[i].children[j].curve.ymax > ymax)
                                    {
                                        ymax = Tree[i].children[j].curve.ymax;
                                        rangesChanged = true;
                                    }
                                } else {
                                    
                                    // Tree[i].children[j].curve.color = ImPlot::GetColormapColor(numCurves--);
                                }
                            }
                        }
                                
                        // Plotting in second pass (because we need to calculate axis limits beforehand)
                        if(ImPlot::BeginPlot("##", plotSize))
                        {
                            // ImGui::BulletText(
                            //     "Tab Index = %d\n"
                            //     "Active Plots (size) = %d\n"
                            //     "IsItemFocused() = %d\n"
                            //     "Last focussed Tab = %d\n",
                            //     activeTabs[n].Index,
                            //     activeTabs[n].activePlots.size(),
                            //     ImGui::IsItemFocused(),
                            //     lastFocussedTab
                            // );

                            ImPlot::SetupAxes(xAxisLabelText, yAxisLabelText);
                            // ImPlot::SetupAxis(ImAxis_X1, xAxisLabelText);
                            // ImPlot::SetupAxis(ImAxis_Y1, yAxisLabelText);
                            if(rangesChanged)
                            {
                                ImPlot::SetupAxesLimits(xmin, xmax, ymin, ymax, ImPlotCond_Always);
                                rangesChanged = false;
                            }
                            
                            for(long unsigned int i = 0; i < Tree.size(); i++)
                            {
                                for(long unsigned int j = 0; j < Tree[i].children.size(); j++)
                                {
                                    bool inTab = false;
                                    if(lastFocussedTab == activeTabs[n].Index)
                                    {
                                        for(long unsigned int k = 0; k < activeTabs[n].activePlots.size(); k++)
                                        {
                                            if(Tree[i].children[j].Index == activeTabs[n].activePlots[k])
                                            {
                                                inTab = true;
                                            }
                                        }
                                    }                                    
                                    if(Tree[i].children[j].checkboxState == true && inTab) 
                                    {

                                        ImPlot::SetNextLineStyle(Tree[i].children[j].curve.color, Tree[i].children[j].curve.thickness);

                                        if (Tree[i].children[j].curve.markerFlag)
                                        {
                                            ImPlot::SetNextMarkerStyle(Tree[i].children[j].curve.marker, Tree[i].children[j].curve.markerSize,
                                                                       IMPLOT_AUTO_COL, Tree[i].children[j].curve.markerWeight);
                                        }

                                        if (ImPlot::BeginLegendPopup(Tree[i].children[j].pathName.c_str())) {
        
                                            ImGui::InputTextWithHint("##", "Title (Legend)", titleInput, IM_ARRAYSIZE(titleInput));
                                            ImGui::SameLine();
                                            if(ImGui::Button("Set Title"))
                                            {
                                                titleFlag = true;
                                            }
                                            if(titleFlag)
                                            {
                                                Tree[i].children[j].pathName = titleInput;
                                                titleFlag = false;
                                            }

                                            ImGui::Separator();

                                            ImGui::ColorEdit4("##Color", &Tree[i].children[j].curve.color.x);
                                            ImPlot::SetNextLineStyle(Tree[i].children[j].curve.color, Tree[i].children[j].curve.thickness);

                                            ImGui::SliderFloat("Thickness", &Tree[i].children[j].curve.thickness, 0.1, 10);
                                            ImGui::Checkbox("Markers", &Tree[i].children[j].curve.markerFlag);
                                            if(Tree[i].children[j].curve.markerFlag)
                                            {
                                                ImGui::SliderFloat("Marker Size", &Tree[i].children[j].curve.markerSize, 0, 10, "%.2f");
                                                ImGui::SliderFloat("Marker Weight", &Tree[i].children[j].curve.markerWeight, 1, 10, "%.2f");
                                                ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
                                                Tree[i].children[j].curve.marker = marker[item_current];
                                            }

                                            ImPlot::EndLegendPopup();
                                        }

                                        double* x = &Tree[i].children[j].curve.x[0];
                                        double* y = &Tree[i].children[j].curve.y[0];

                                        // TODO: does PlotLine convert to float?
                                        ImPlot::PlotLine(Tree[i].children[j].pathName.c_str(),
                                                         x,
                                                         y,
                                                         Tree[i].children[j].curve.x.size());
                                        if(globalShowMeanFlag)
                                        {
                                            Tree[i].children[j].curve.showMeanFlag = globalShowMeanFlag;
                                            // uint32_t xMinVal = *static_cast<uint32_t*>(xAxisMinText);
                                            // uint32_t xMaxVal = *static_cast<uint32_t*>(xAxisMaxText);

                                            // uint32_t xMinVal = (uint32_t)(xAxisMinText);
                                            // uint32_t xMaxVal = (uint32_t)(xAxisMaxText);

                                            
                                            // memcpy(&xMinVal, xAxisMinText, sizeof xMinVal);
                                            // memcpy(&xMaxVal, xAxisMaxText, sizeof xMaxVal);
                                            Tree[i].children[j].curve.mean = calculateMean(Tree[i].children[j].curve.y, xAxisMin, xAxisMax, Tree[i].children[j].curve.deltaX);
                                            // std::cout << Tree[i].children[j].curve.mean << std::endl;
                                            // Plot mean if checkbox is checked
                                            // ImPlot::PlotLine()
                                            ImPlot::SetNextLineStyle(Tree[i].children[j].curve.color, Tree[i].children[j].curve.thickness);
                                            std::string meanText = "Mean: " + Tree[i].children[j].pathName;
                                            ImPlot::PlotInfLines(meanText.c_str(), &Tree[i].children[j].curve.mean, 1, ImPlotInfLinesFlags_Horizontal);
                                        }
                                    }
                                }
                            }
                            
                            ImPlot::EndPlot();
                        }

                        // Track the last focussed tab to add plots to correct tab
                        if(ImGui::IsItemFocused())
                        {
                            lastFocussedTab = activeTabs[n].Index;
                        }
                    }
                
                    if(!open)
                    {
                        activeTabs.erase(activeTabs.Data + n);
                    }
                    else
                    {
                        n++;
                    }
                    
                    ImGui::End();
                }

            //     ImGui::EndTabBar();
            // }
 
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
