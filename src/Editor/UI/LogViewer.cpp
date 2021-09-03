#include "Editor/UI/LogViewer.h"

void LogViewer::Draw()
{
    if (ImGui::BeginDock("Log viewer", &Active))
        graphLog.Draw();
    ImGui::EndDock();
}

void LogViewer::StartCapture()
{
    oldLog = logger;
    logger = &graphLog;
    graphLog.Clear();
}

void LogViewer::EndCapture()
{
    logger = oldLog;
}

LogViewer::GraphLog::GraphLog()
{
    Clear();
}

void LogViewer::GraphLog::Write(const std::string &msg)
{
    lock.lock();
    int old_size = buffer.size();
    buffer.append(msg.c_str());
    for (int new_size = buffer.size(); old_size < new_size; ++old_size)
        if (buffer[old_size] == '\n')
            lineOffsets.push_back(old_size + 1);
    lock.unlock();
}

void LogViewer::GraphLog::Clear()
{
    lock.lock();
    buffer.clear();
    lineOffsets.clear();
    lineOffsets.push_back(0);
    lock.unlock();
}

void LogViewer::GraphLog::Draw()
{
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        Clear();

    lock.lock();
    if (copy)
        ImGui::LogToClipboard();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const char* buffer_start = buffer.begin();
    const char* buffer_end = buffer.end();
    if (filter.IsActive())
    {
        for (int line_no = 0; line_no < lineOffsets.Size; ++line_no)
        {
            const char* line_start = buffer_start + lineOffsets[line_no];
            const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buffer_start + lineOffsets[line_no + 1] - 1) : buffer_end;
            if (filter.PassFilter(line_start, line_end))
                ImGui::TextUnformatted(line_start, line_end);
        }
    }
    else
    {
        ImGuiListClipper clipper;
        clipper.Begin(lineOffsets.Size);
        while (clipper.Step())
        {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; ++line_no)
            {
                const char* line_start = buffer_start + lineOffsets[line_no];
                const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buffer_start + lineOffsets[line_no + 1] - 1) : buffer_end;
                ImGui::TextUnformatted(line_start, line_end);
            }
        }
        clipper.End();
    }
    ImGui::PopStyleVar();
    lock.unlock();

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
}
