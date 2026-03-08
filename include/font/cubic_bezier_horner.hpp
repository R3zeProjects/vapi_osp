/** @file cubic_bezier_horner.hpp
 *  @brief Cubic Bézier segment with Horner evaluation and cached coefficients.
 *  Used for font outlines: one segment = 4 control points, eval(t) in 6 multiplies per component. */

#ifndef VAPI_FONT_CUBIC_BEZIER_HORNER_HPP
#define VAPI_FONT_CUBIC_BEZIER_HORNER_HPP

#include "core/types.hpp"
#include <array>

namespace vapi::font {

/// Coefficients for x(t)=ax*t^3+bx*t^2+cx*t+dx, y(t)=ay*t^3+by*t^2+cy*t+dy. Horner: ((a*t+b)*t+c)*t+d.
struct CubicCoeffs {
    f32 ax{0.f}, bx{0.f}, cx{0.f}, dx{0.f};
    f32 ay{0.f}, by{0.f}, cy{0.f}, dy{0.f};

    vec2 eval(f32 t) const {
        f32 x = ((ax * t + bx) * t + cx) * t + dx;
        f32 y = ((ay * t + by) * t + cy) * t + dy;
        return {x, y};
    }
};

/// One cubic Bézier segment (4 control points). Coefficients cached after first use.
struct CubicSegment {
    std::array<vec2, 4> points;

    mutable CubicCoeffs coeffs;
    mutable bool coeffsValid{false};

    void updateCoeffs() const {
        const vec2& P0 = points[0];
        const vec2& P1 = points[1];
        const vec2& P2 = points[2];
        const vec2& P3 = points[3];
        coeffs.dx = P0.x;
        coeffs.cx = 3.f * (P1.x - P0.x);
        coeffs.bx = 3.f * (P0.x - 2.f * P1.x + P2.x);
        coeffs.ax = -P0.x + 3.f * P1.x - 3.f * P2.x + P3.x;
        coeffs.dy = P0.y;
        coeffs.cy = 3.f * (P1.y - P0.y);
        coeffs.by = 3.f * (P0.y - 2.f * P1.y + P2.y);
        coeffs.ay = -P0.y + 3.f * P1.y - 3.f * P2.y + P3.y;
        coeffsValid = true;
    }

    const CubicCoeffs& getCoeffs() const {
        if (!coeffsValid) updateCoeffs();
        return coeffs;
    }

    vec2 eval(f32 t) const {
        return getCoeffs().eval(t);
    }

    /// Linear segment (straight line): P0=P1=from, P2=P3=to.
    static CubicSegment linear(vec2 from, vec2 to) {
        CubicSegment s;
        s.points[0] = from;
        s.points[1] = from;
        s.points[2] = to;
        s.points[3] = to;
        s.coeffsValid = false;
        return s;
    }
};

} // namespace vapi::font

#endif // VAPI_FONT_CUBIC_BEZIER_HORNER_HPP
