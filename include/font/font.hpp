/** @file font.hpp
 *  @brief Модуль отрисовки шрифтов: типы, источники, атлас, layout/paint, DrawList.
 *
 *  Слои:
 *  1) font_types     — идентификаторы, метрики, TextLayout, TextGeometry (результат paint).
 *  2) i_font_source  — интерфейс источника глифов (растр + метрики).
 *  3) Источники: stb_font_source (TTF/OTF), ascii_bezier_font_source (8x8 → Безье → растр).
 *  4) Безье: cubic_bezier_horner, glyph_outline, outline_rasterizer.
 *  5) glyph_atlas    — упаковка глифов в одну текстуру.
 *  6) text_painter   — layout + paint в TextGeometry.
 *  7) font_service   — загрузка шрифтов по пути.
 *  8) font_draw_list — UI-список отрисовки (AddText / AddImage), один атлас, один буфер вершин.
 */

#ifndef VAPI_FONT_FONT_HPP
#define VAPI_FONT_FONT_HPP

// Типы и идентификаторы
#include "font/font_types.hpp"
#include "font/i_font_source.hpp"

// Системные пути и загрузка
#include "font/system_font_paths.hpp"
#include "font/font_service.hpp"

// Реализации источников глифов
#include "font/stb_font_source.hpp"
#include "font/cubic_bezier_horner.hpp"
#include "font/glyph_outline.hpp"
#include "font/outline_rasterizer.hpp"
#include "font/ascii_bezier_font_source.hpp"

// Атлас и отрисовка
#include "font/glyph_atlas.hpp"
#include "font/text_painter.hpp"
#include "font/font_draw_list.hpp"

#endif // VAPI_FONT_FONT_HPP
