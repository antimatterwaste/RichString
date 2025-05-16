#include "rich_string.h"
#include <QPainter>

RichString::RichString(const QString& str, const QColor& color,
                       const QFont& font) {
  str_.clear();
  for (int i = 0; i < str.size(); ++i) {
    str_ << RichChar(str[i], color, font);
  }
}

QString RichString::plainText() {
  QString text;
  text.reserve(str_.size());
  for (int i = 0; i < str_.size(); ++i) {
    text.append(str_.at(i).ch());
  }
  return text;
}

int RichString::maxTextHeight() const {
  int max = 0;
  for (int i = 0; i < str_.size(); ++i) {
    QFontMetrics metric(str_[i].font());
    if (max < metric.height()) {
      max = metric.height();
    }
  }
  return max;
}

RichString& RichString::append(const RichString& rich_str) {
  for (int i = 0; i < rich_str.size(); ++i) {
    append(rich_str.at(i));
  }
  return *this;
}

int RichString::textWidth() const {
  int width = 0;
  for (int i = 0; i < str_.size(); ++i) {
    width += str_.at(i).width();
  }
  return width;
}

RichString RichString::elidedText(int width, const QColor& ellipses_color,
                                  const QFont& ellipses_font) const {
  if (width <= 0) {
    return RichString();
  }
  if (textWidth() <= width) {
    return *this;
  }
  QString ellipses = QString::fromUtf8("\u2026" /*…*/);
  QFontMetrics ellipses_metric(ellipses_font);
  int ellipses_width = ellipses_metric.width(ellipses);

  RichString temp = *this;
  for (; temp.size() > 0;) {
    if (temp.textWidth() + ellipses_width <= width) {
      temp += RichString(ellipses, ellipses_color, ellipses_font);
      break;
    }
    temp.chop(1);
  }
  return temp;
}

void RichString::chop(int n) {
  if (n <= 0) {
    return;
  }
  if (n > str_.size()) {
    str_.clear();
    return;
  }
  str_.erase(str_.end() - n, str_.end());
}

void RichString::replaceRearWithEllipses(const QColor& ellipses_color,
                                         const QFont& ellipses_font) {
  if (isEmpty()) {
    return;
  }
  QFontMetrics m(ellipses_font);
  QString ellipses = QString::fromUtf8("\u2026" /*…*/);
  RichString temp = *this;
  int width = temp.textWidth();
  for (; !temp.isEmpty();) {
    temp.chop(1);
    if (temp.textWidth() + m.width(ellipses) <= width) {
      append(RichString(ellipses, ellipses_color, ellipses_font));
      break;
    }
  }
  if (!temp.isEmpty()) {
    *this = temp;
  }
}

void RichStringTool::draw_rich_text(QPainter& painter, const QRect& rect,
                                    int alignment_flags,
                                    const RichString& rich_string,
                                    int line_spacing) {
  painter.save();
  if (!painter.device() || !rect.isValid()) {
    return;
  }
  int display_width = rect.width();
  QList<RichString> lines =
      split_rich_text_by_width(rich_string, display_width);
  if (lines.isEmpty()) {
    return;
  }
  int display_height = 0;
  for (int i = 0; i < lines.size(); ++i) {
    display_height += lines.at(i).maxTextHeight();
    if (i < lines.size() - 1) {
      display_height += line_spacing;
    }
  }
  if (display_height <= 0) {
    return;
  }
  QPixmap pix(display_width, display_height);
  pix.fill(Qt::transparent);
  QPainter pix_painter(&pix);
  int draw_x = 0;
  int draw_y = 0;
  for (int line_index = 0; line_index < lines.size(); ++line_index) {
    const RichString& line_str = lines.at(line_index);
    draw_x = 0;
    for (int ch_index = 0; ch_index < line_str.size(); ++ch_index) {
      const RichChar& ch = line_str.at(ch_index);
      pix_painter.setPen(ch.color());
      pix_painter.setFont(ch.font());
      pix_painter.drawText(
          draw_x, (line_str.maxTextHeight() - ch.height()) / 2 + draw_y,
          ch.width(), ch.height(), Qt::AlignCenter, ch.ch());
      draw_x += ch.width();
    }
    draw_y += line_str.maxTextHeight();
    if (line_index < lines.size() - 1) {
      draw_y += line_spacing;
    }
  }

  int diff_y = 0;
  if (alignment_flags & Qt::AlignVCenter) {
    diff_y = (rect.height() - pix.height()) / 2;
  } else if (alignment_flags & Qt::AlignBottom) {
    diff_y = rect.height() - pix.height();
  }

  painter.drawPixmap(rect.x(), rect.y() + diff_y, pix);
  painter.restore();
}

QList<RichString> RichStringTool::split_rich_text_by_width(
    const RichString& rich_str, int width) {
  QList<RichString> line_list;
  if (rich_str.isEmpty()) {
    return line_list;
  }
  RichString line;
  int line_width = 0;
  for (int i = 0; i < rich_str.size(); ++i) {
    const RichChar& ch = rich_str.at(i);
    if (line_width + ch.width() <= width) {
      line_width += ch.width();
      line += rich_str.at(i);
    } else {
      line_list << line;
      line = RichString(ch);
      line_width = ch.width();
    }
  }
  if (line.size() > 0) {
    line_list << line;
  }
  return line_list;
}

QSize RichStringTool::get_rich_text_size(
    const QList<RichString>& rich_string_vec, int line_spacing) {
  int width = 0;
  int height = 0;
  for (int i = 0; i < rich_string_vec.size(); ++i) {
    if (width < rich_string_vec.size()) {
      width = rich_string_vec[i].textWidth();
    }
    height += rich_string_vec[i].maxTextHeight();
    if (i < rich_string_vec.size() - 1) {
      height += line_spacing;
    }
  }
  return QSize(width, height);
}

void RichChar::updateSize() {
  QFontMetrics m(font_);
  width_ = m.width(ch_);
  height_ = m.height();
}
