#pragma once

// UI 2D POSITONS
struct UIVector2 final
{
    float x = 0.0f;
    float y = 0.0f;
};

// UI RECTANGLE SIZE
struct UISize final
{
    float width = 0.0f;
    float height = 0.0f;
};

// THE COMPLETED UI RECTANGLE
struct UIRect final
{
    UIVector2 position;
    UISize size;
};

// UI MARGINS
struct UIMargin final
{
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
};

// UI ANCHOR POSITIONS
enum class UIAnchor
{
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

// UI ALIGNMENT
enum class UIAlignment
{
    Start,
    Center,
    End,
    Stretch
};
