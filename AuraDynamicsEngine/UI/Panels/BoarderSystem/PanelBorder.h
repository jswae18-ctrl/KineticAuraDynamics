#pragma once
#include "../../Window/PanelWindow.h"
#include <cstddef>
#include <cmath>
#include <vector>


// SETS THE INDIVIDUAL POINT POSITIONS
struct BorderPoint
{
    float x;
    float y;
};

// SETS THE START AND END POSITIONS OF 1 FULL BORDER POSITION TO ANOTHER
struct PointSegment
{
    std::size_t startPoint;
    std::size_t endPoint;
};

// GETS THE FULL LINE SEGMENT LOCATION
struct BorderSegment
{
    BorderPoint start;
    BorderPoint end;
};

// CLASS CREATION
class PanelBorder final
{
public:
    explicit PanelBorder(float width, float height) noexcept;
    ~PanelBorder() = default;
    
    PanelBorder(const PanelBorder&) = delete;
    PanelBorder& operator=(const PanelBorder&) = delete;
    PanelBorder(PanelBorder&&) = delete;
    PanelBorder& operator=(PanelBorder&&) = delete;
    
private:
    
    // BORDER CREATION
    void CreateBorder();
    
public:
    
    // GETS THE POINT VALUES
    const std::vector<BorderPoint>& GetPoints() const noexcept
    {
        return point;
    }
    
    // GETS THE SEGMENT VAlUES 
    const std::vector<PointSegment>& GetSegments() const noexcept
    {
        return segment;
    }
    
    // BUILDS THE SEGMENT
    void BuildSegments();
    
    // GETS THE FULL SEGMENT
    const std::vector<BorderSegment>& GetFullSegments() const noexcept
    {
        return fullSegment;
    }
    
    
    // GETS THE BORDER RECTANGLE
    const PanelRectangle& GetBorderRECT() const noexcept
    {
        return borderRECT;
    }
    
private:
    std::vector<BorderPoint> point;
    std::vector<PointSegment> segment;
    std::vector<BorderSegment> fullSegment;
    float fullBorderWidth;
    float fullBorderHeight;
    PanelRectangle borderRECT;
};
// ============================================================================
// GEOMETRY RELATIONSHIP
// ============================================================================
//
// PanelBorder owns and creates its own geometry.
//
// PanelBorder does NOT:
// - Own a PanelWindow.
// - Require a PanelWindow to exist.
// - Depend on PanelWindow in order to create a border.
// - Control the size, position, anchoring, or state of a PanelWindow.
//
// PanelBorder:
// - Owns its design width and height.
// - Owns its border rectangle.
// - Creates border geometry independently.
// - Will eventually own border points and segments.
// - Can later adapt its existing geometry to a target rectangle.
//
// DATA FLOW:
//
// Construction
//     |
//     | width / height
//     v
// PanelBorder
//     |
//     | stores design dimensions
//     v
// CreateBorder()
//     |
//     | creates independent border geometry
//     v
// borderRECT
//     |
//     | future reference geometry
//     v
// Border Points / Segments
//
//
// Later:
//
// Target Rectangle
//     |
//     v
// UpdateBorder(...)
//     |
//     | resolves/adapts existing border geometry
//     v
// Updated Border Geometry
//
// IMPORTANT:
// Creating a border and fitting a border to another UI element are
// separate operations.
//
// PanelBorder geometry exists independently from PanelWindow geometry.