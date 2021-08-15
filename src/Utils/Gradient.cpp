#include "Utils/Gradient.h"

void Gradient::computeColorAt(f32 position, glm::vec4 *color) const noexcept
{
    position = position < 0.0f? 0.0f : position > 1.0f? 1.0f : position;

    if (marks.empty())
        *color = glm::vec4(0.0f,0.0f,0.0f,0.0f);

    Mark lower = marks.front();
    Mark upper = marks.front();

    for(const auto &mark : marks)
    {
        if(mark.position < position && lower.position < mark.position)
            lower = mark;

        if(mark.position >= position && upper.position > mark.position)
            upper = mark;
    }

    if (float distance = upper.position - lower.position; distance != 0)
    {
        float delta = (position - lower.position) / distance;
        *color = ((1.0f - delta) * lower.color) + ((delta) * upper.color);
    }
    else
        *color = lower.color;
}

Gradient::Gradient()
{
    AddMark(0.0f, glm::vec4(0.0f,0.0f,0.0f,0.0f));
    AddMark(1.0f, glm::vec4(1.0f,1.0f,1.0f,1.0f));

    RefreshCache();
}

glm::vec4 Gradient::GetColorAt(f32 position) const noexcept
{
    position = position < 0.0f? 0.0f : position > 1.0f? 1.0f : position;

    glm::vec4 color = cachedValues[int(position * (CacheSize - 1))];

    for(i32 i = 0; i < 4; ++i)
        if(color[i] < 0)
            color[i] = 0;

    return color;
}

void Gradient::AddMark(f32 position, const glm::vec4 color)
{
    position = position < 0.0f? 0.0f : position > 1.0f? 1.0f : position;

    Mark mark{};
    mark.position = position;
    mark.color = color;

    marks.push_back(mark);

    RefreshCache();
}

void Gradient::RemoveMark(MarkIterator element) noexcept
{
    marks.erase(element);
    RefreshCache();
}

void Gradient::RefreshCache()
{
    marks.sort([](const Mark &a, const Mark &b) {
        return a.position < b.position;
    });

    for(u32 i = 0; i < CacheSize; ++i)
        computeColorAt(float(i) / (CacheSize - 1.0f), &cachedValues[i]);
}
