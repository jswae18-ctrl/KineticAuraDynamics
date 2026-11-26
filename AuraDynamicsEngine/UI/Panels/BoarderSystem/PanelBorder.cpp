#include "PanelBorder.h"

PanelBorder::PanelBorder(float width, float height) noexcept
    : fullBorderWidth(width), fullBorderHeight(height)
{
    CreateBorder();
    BuildSegments();
}

void PanelBorder::CreateBorder()
{
    // CLEAR OLD VALUES BEFORE CREATING NEW ONES
    point.clear();
    segment.clear();
    fullSegment.clear();
    
    // DEFINES THE POSITION AND SIZE
    borderRECT.x = 0.0f;
    borderRECT.y = 0.0f;
    borderRECT.width = fullBorderWidth;
    borderRECT.height = fullBorderHeight;
    
    if (!std::isfinite(borderRECT.x)||
        !std::isfinite(borderRECT.y) ||
        !std::isfinite(borderRECT.width) ||
        !std::isfinite(borderRECT.height) ||
        borderRECT.width <= 0.0f ||
        borderRECT.height <= 0.0f)
    {
        return;
    }
    
    // BORDER POINT POSITION
    BorderPoint topLeft;
    topLeft.x = borderRECT.x;
    topLeft.y = borderRECT.y;
    point.push_back(topLeft);
    
    BorderPoint topRight;
    topRight.x = borderRECT.x + borderRECT.width;
    topRight.y = borderRECT.y;
    point.push_back(topRight);
    
    BorderPoint bottomLeft;
    bottomLeft.x = borderRECT.x;
    bottomLeft.y = borderRECT.y + borderRECT.height;
    point.push_back(bottomLeft);
    
    BorderPoint bottomRight;
    bottomRight.x = borderRECT.x + borderRECT.width;
    bottomRight.y = borderRECT.y + borderRECT.height;
    point.push_back(bottomRight);
    
    
    // POINT SEGMENTS
    PointSegment top;
    top.startPoint = 0;
    top.endPoint = 1;
    segment.push_back(top);
    
    PointSegment rightSide;
    rightSide.startPoint = 1;
    rightSide.endPoint = 3;
    segment.push_back(rightSide);
    
    PointSegment bottom;
    bottom.startPoint = 3;
    bottom.endPoint = 2;
    segment.push_back(bottom);
    
    PointSegment leftSide;
    leftSide.startPoint = 2;
    leftSide.endPoint = 0;
    segment.push_back(leftSide);
}

void PanelBorder::BuildSegments()
{
    fullSegment.clear();
    
    for (const PointSegment& currentSegment : segment)
    {
        if (currentSegment.startPoint >= point.size() ||
            currentSegment.endPoint >= point.size())
        {
            continue;
        }
        const BorderPoint& start = point[currentSegment.startPoint];
        const BorderPoint& end = point[currentSegment.endPoint];
        
        BorderSegment builtSegment;
        builtSegment.start = start;
        builtSegment.end = end;
        fullSegment.push_back(builtSegment);
    }
}
