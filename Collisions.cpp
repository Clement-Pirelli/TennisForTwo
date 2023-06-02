#include "Collisions.h"

std::optional<CollisionResult> collideWith(Line line, vec2 velocity, vec2 position)
{
    Line other = Line{ position, position + velocity };
    //adapted from http://www.jeffreythompson.org/collision-detection/line-line.php
    
    float uA = ((line.end.x() - line.start.x()) * (other.start.y() - line.start.y()) - (line.end.y() - line.start.y()) * (other.start.x() - line.start.x())) / ((line.end.y() - line.start.y()) * (other.end.x() - other.start.x()) - (line.end.x() - line.start.x()) * (other.end.y() - other.start.y()));
    float uB = ((other.end.x() - other.start.x()) * (other.start.y() - line.start.y()) - (other.end.y() - other.start.y()) * (other.start.x() - line.start.x())) / ((line.end.y() - line.start.y()) * (other.end.x() - other.start.x()) - (line.end.x() - line.start.x()) * (other.end.y() - other.start.y()));

    if (uA >= .0f && uA <= 1.0f && uB >= 0.0f && uB <= 1.0f) {

        const vec2 collisionPoint = 
        { 
            line.start.x() + uA * (line.end.x() - line.start.x()), 
            line.start.y() + uA * (line.end.y() - line.start.y())
        };

        const vec2 hypotenuse = position - line.start;
        const vec2 lineTangential = line.end - line.start;
        const float projectedLength = vec2::dot(hypotenuse, lineTangential.normalized());
        const vec2 normalOrigin = line.start + lineTangential.normalized() * projectedLength;
        const vec2 normal = (position - normalOrigin).normalized();

        return CollisionResult
        {
            .velocity = vec2::reflect(velocity, normal),
            .intersection = collisionPoint
        };
    }
    return std::nullopt;
}
