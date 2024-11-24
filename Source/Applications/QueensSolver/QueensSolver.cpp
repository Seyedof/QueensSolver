#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "QueenBoard.h"
#include "MCVSolver.h"

#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <vector>
#include <set>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

constexpr float BOARD_CELL_SIZE         = 50.0f;
GLFWwindow* window = nullptr;

ImVec4 palette[20] = {
    {0.5f, 0, 0, 1.0f},
    {0, 0.5f, 0, 1.0f},
    {0, 0, 0.5f, 1.0f},
    {0.5f, 0.5f, 0, 1.0f},
    {0.5f, 0, 0.5f, 1.0f},
    {0, 0.5f, 0.5f, 1.0f},
    {1.0f, 0, 0, 1.0f},
    {0, 1.0f, 0, 1.0f},
    {0, 0, 1.0f, 1.0f},
    {1.0f, 1.0f, 0, 1.0f},
    {1.0f, 0, 1.0f, 1.0f},
    {0, 1.0f, 1.0f, 1.0f},
};

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

bool InitAppContext()
{
    if (!glfwInit()) {
        return false;
    }

#if defined(__APPLE__)
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    window = glfwCreateWindow(1280, 720, "Linkedin Queens Problem Solver", nullptr, nullptr);
    if (window == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    return true;
}

void VisualizeBoard(QueenBoard& board, MCVSolver& mcvSolver)
{
    ImGuiWindowFlags flags = 0
        | ImGuiWindowFlags_HorizontalScrollbar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::Begin("Queens Algo Visualize", nullptr, flags);
    static int s_boardSize = 8;
    ImGui::SetNextItemWidth(264);
    ImGui::SliderInt("Board Size", &s_boardSize, 4, 20);

    if (ImGui::Button("Create Board")) {
        board.Create(s_boardSize);
    }

    ImGui::SameLine();
    if (ImGui::Button("Load Board")) {
        board.LoadBoard("board.txt");

    }

    ImGui::SameLine();
    ImGui::BeginDisabled(board.Size() == 0);
    if (ImGui::Button("Save Board")) {
        board.SaveBoard("board.txt");
    }
    ImGui::EndDisabled();

    static bool s_running = false;

    static bool s_paintMode = true;
    ImGui::BeginDisabled(s_running);
    ImGui::Checkbox("Paint The Board", &s_paintMode);
    static ImVec4 s_color{ 0.0f, 0.8f, 1.0f, 1.0f };
    if (s_paintMode) {
        ImGui::ColorButton("Color1", s_color, 0, ImVec2{ 60, 60 });
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleColor(ImGuiCol_Button, 0x00000000);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff0000ff);
        for (int i = 0; i < 20; ++i) {
            ImGui::PushID(i);
            if (ImGui::ImageButton("", 0, { 30, 30 }, { 0, 0 }, { 0, 0 }, ImColor(palette[i]), ImColor(0x00ffffff))) {
                s_color = palette[i];
            }
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::NewLine();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }
    ImGui::EndDisabled();

    ImGui::NewLine();
    ImGui::BeginDisabled(board.Size() == 0);
    if (ImGui::Button("Start")) {
        s_running = !s_running;
        if (s_running) {
            s_paintMode = false;
        }

        board.InitialQueensSetup();
        board.UpdateConflictValues();
        mcvSolver.Reset();
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(!s_running);
    bool nextStep = ImGui::Button("Next Step");
    ImGui::EndDisabled();

    static bool s_autoStep = false;
    ImGui::SameLine();
    ImGui::Checkbox("Auto Step", &s_autoStep);
    nextStep |= s_autoStep;
    ImGui::EndDisabled();
    ImGui::NewLine();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleColor(ImGuiCol_Button, 0xffffffff);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff0000ff);
    ImGui::PushStyleColor(ImGuiCol_Text, 0xff0000ff);
    ImVec2 boardStartPoint = ImGui::GetCursorPos() + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY());
    for (size_t row = 0; row < board.Size(); ++row) {
        for (size_t column = 0; column < board.Size(); ++column) {
            ImGui::PushID(row * board.Size() + column);
            if (s_running) {
                ImVec2 prevPos = ImGui::GetCursorPos();
                ImGui::ImageButton("", 0, { BOARD_CELL_SIZE, BOARD_CELL_SIZE }, { 0, 0 }, { 0, 0 }, ImColor(board.GetBoardColor()[row][column]), ImColor(0x00ffffff));
                std::string conflictValue = std::to_string(board.GetBoardCV()[row][column]);
                ImGui::GetWindowDrawList()->AddText(prevPos + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY()) + ImVec2(5, 35), 0xff000000, conflictValue.c_str());
                if (board.IsOccupied(row, column)) {
                    ImGui::SetWindowFontScale(3.0f);
                    ImGui::GetWindowDrawList()->AddText(prevPos + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY()) + ImVec2(15, 5), 0xff000000, "*");
                    ImGui::SetWindowFontScale(1.0f);
                }

                if (mcvSolver.GetCurStep() == MCVSolver::Step::PickMaxQueenCV) {
                    auto pickedQueen = board.GetQueenInColumn(mcvSolver.GetPickedQueen());

                    if (row == pickedQueen.row && column == pickedQueen.column) {
                        ImGui::GetWindowDrawList()->AddRect(prevPos + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY()), prevPos + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY()) + ImVec2(BOARD_CELL_SIZE, BOARD_CELL_SIZE), 0xff0000ff);
                    }
                }
                if (mcvSolver.GetCurStep() == MCVSolver::Step::FindMinEmptyCV) {
                    auto pickedQueen = board.GetQueenInColumn(mcvSolver.GetPickedQueen());
                    if (row == mcvSolver.GetMoveToRow() && column == pickedQueen.column) {
                        ImGui::GetWindowDrawList()->AddRect(prevPos + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY()), prevPos + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY()) + ImVec2(BOARD_CELL_SIZE, BOARD_CELL_SIZE), 0xff00ff00);
                    }
                }

            }
            else {
                ImGui::ImageButton("", 0, { BOARD_CELL_SIZE, BOARD_CELL_SIZE }, { 0, 0 }, { 0, 0 }, ImColor(board.GetBoardColor()[row][column]), ImColor(0x00ffffff));
            }
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);

    if (s_paintMode) {
        if (ImGui::GetIO().MouseDown[0]) {
            auto mousePos = ImGui::GetIO().MousePos;
            float x = mousePos.x - boardStartPoint.x;
            float y = mousePos.y - boardStartPoint.y;
            if (x >= 0 && y >= 0 && x < board.Size() * BOARD_CELL_SIZE && y < board.Size() * BOARD_CELL_SIZE) {
                x /= BOARD_CELL_SIZE;
                y /= BOARD_CELL_SIZE;
                board.SetBoardColor(y, x, ImColor(s_color));
            }
        }
    }

    if (nextStep) {
        mcvSolver.NextStep(board);
    }

    ImGui::End();
}

int main(int, char**)
{
    InitAppContext();

    QueenBoard board;
    MCVSolver mcvSolver;

    auto& io = ImGui::GetIO();

#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        VisualizeBoard(board, mcvSolver);

        // Render board
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
