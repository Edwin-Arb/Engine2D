#pragma once
#include <vector>

struct FMatrix3x3;
struct FVector2D;

namespace sf
{
	class RenderWindow;
	class Color;
	struct Vertex;
}  // namespace sf

/**
 * Lightweight immediate-mode debug drawing. Queue line / arrow / circle shapes during a
 * frame, then flush them all in a single draw call. Every shape is built out of line
 * segments (pairs of vertices), so one SFML "Lines" draw renders the whole batch.
 *
 * Usage: Clear() at the start of the frame, queue shapes with the Add* calls, then Draw()
 * from the render pass with the camera's view matrix.
 */
class FDebugDraw
{
public:
	/** Drops all queued shapes. Call once at the start of each frame before queuing. */
	void Clear();

	/** Queues a single line segment from InA to InB. */
	void AddLine(const FVector2D& InA, const FVector2D& InB, const sf::Color& Color);

	/** Queues an arrow: a shaft from InStart along the displacement InVec, plus a two-barb head. */
	void AddArrow(const FVector2D& InStart, const FVector2D& InVec, const sf::Color& Color);

	/** Queues a circle outline, approximated by a ring of short line segments. */
	void AddCircle(const FVector2D& InCenter, float InRadius, const sf::Color& Color);

	/** Flushes every queued segment to the window in one draw call (world space, via ViewMatrix). */
	void Draw(sf::RenderWindow& Window, const FMatrix3x3& ViewMatrix);

private:
	/** Length of each arrowhead barb, in pixels. */
	float HeadLength = 40.0f;

	/** Angle between the arrow shaft and each head barb, in degrees. */
	float HeadAngle = 30.0f;

	/** Flat vertex buffer; consecutive pairs of vertices form line segments. */
	std::vector<sf::Vertex> Lines;
};
