#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

#CXX = g++
#CXX = clang++
DEBUG = 0
UNAME_S := $(shell uname -s)
BUILD_DIR = build-$(UNAME_S)
BIN_DIR = bin
$(shell mkdir -p $(BUILD_DIR))
EXE = $(BUILD_DIR)/main
IMGUI_DIR = src/imgui-docking
IMPLOT_DIR = src/implot
# IMGUIFILEBROWSER_DIR = src/FileBrowser
IMGUIFILEDIALOG_DIR = src/ImGuiFileDialog
SOURCES = src/main.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMPLOT_DIR)/implot.cpp $(IMPLOT_DIR)/implot_items.cpp $(IMPLOT_DIR)/implot_demo.cpp  $(IMGUIFILEDIALOG_DIR)/ImGuiFileDialog.cpp
# $(IMGUIFILEBROWSER_DIR)/ImGuiFileBrowser.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

ifeq ($(DEBUG), 0)
        CXXFLAGS = -std=c++11 -O3 -flto -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(IMPLOT_DIR) -I$(IMGUIFILEDIALOG_DIR)
else
        CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(IMPLOT_DIR) -I$(IMGUIFILEDIALOG_DIR)
endif
CXXFLAGS += -g -Wall -Wformat
LIBS =

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
        ECHO_MESSAGE = "Linux"
        LIBS += -lGL `pkg-config --static --libs glfw3`

        CXXFLAGS += `pkg-config --cflags glfw3`
        CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
        ECHO_MESSAGE = "Mac OS X"
        LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
        LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
        #LIBS += -lglfw3
        LIBS += -lglfw

        CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
        CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
        ECHO_MESSAGE = "MinGW"
        LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

        CXXFLAGS += `pkg-config --cflags glfw3`
        CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
# $(info objs: $(OBJS))

$(BUILD_DIR)/%.o:src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMGUI_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMGUI_DIR)/backends/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMPLOT_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMGUIFILEBROWSER_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMGUIFILEDIALOG_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@mkdir -p $(BIN_DIR)
	@cp $(EXE) $(BIN_DIR)/$(basename $(notdir $(EXE)))
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)

