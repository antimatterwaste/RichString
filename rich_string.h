#ifndef CONTROL_RICH_STRING_
#define CONTROL_RICH_STRING_

/////////////////////////////////////////////////////////////

#include <QColor>
#include <QFont>
#include <QList>
#include <QString>

/////////////////////////////////////////////////////////////

class RichChar {
 public:
  inline RichChar() {
    ch_ = QChar();
    color_ = Qt::black;
    font_ = QFont();
    width_ = 0;
    height_ = 0;
  }
  inline explicit RichChar(const QChar& in_ch, const QColor& in_color,
                           const QFont& in_font) {
    ch_ = in_ch;
    color_ = in_color;
    font_ = in_font;
    updateSize();
  }
  inline const RichChar& operator=(const RichChar& other) {
    if (this != &other) {
      ch_ = other.ch_;
      color_ = other.color_;
      font_ = other.font_;
      updateSize();
    }
    return *this;
  }
  inline void setCh(const QChar& ch) { ch_ = ch; }
  inline QChar ch() const { return ch_; }
  inline void setColor(const QColor& color) { color_ = color; }
  inline QColor color() const { return color_; }
  inline void setFont(const QFont& font) { font_ = font; }
  inline QFont font() const { return font_; }

  /************************************************************************
   * desc: 字符宽度(px);
   *********************************************************************/
  inline int width() const { return width_; }

  /************************************************************************
   * desc: 字符高度(px);
   *********************************************************************/
  inline int height() const { return height_; }

 private:
  void updateSize();

 private:
  QChar ch_;
  QColor color_;
  QFont font_;
  int width_;
  int height_;
};

/////////////////////////////////////////////////////////////

class RichString {
 public:
  inline RichString() { str_.clear(); }
  explicit RichString(const QString& str, const QColor& color,
                      const QFont& font);
  inline explicit RichString(const RichChar& ch) { str_ << ch; }
  inline const RichString& operator=(const RichString& other) {
    if (this == &other) {
      return *this;
    }
    str_ = other.str_;
    return *this;
  }
  RichString(const RichString& other) { *this = other; }
  RichString& append(const RichChar& ch) {
    str_ << ch;
    return *this;
  }
  inline RichString& operator+=(const RichChar& ch) { return append(ch); }
  RichString& append(const RichString& rich_str);
  inline RichString& operator+=(const RichString& rich_str) {
    return append(rich_str);
  }
  inline const RichString operator+(const RichString& other) {
    RichString temp(*this);
    temp += other;
    return temp;
  }
  inline const RichChar& at(int index) const {
    Q_ASSERT(index >= 0 && index < str_.size());
    return str_[index];
  }
  inline RichChar& operator[](int index) {
    Q_ASSERT(index >= 0 && index < str_.size());
    return str_[index];
  }
  inline int size() const { return str_.size(); }

  /************************************************************************
   * desc: 获取纯文本;
   *********************************************************************/
  QString plainText();

  /************************************************************************
   * desc: 文字最大高度;
   *********************************************************************/
  int maxTextHeight() const;

  /************************************************************************
   * desc: 文字宽度;
   *********************************************************************/
  int textWidth() const;

  /************************************************************************
   * desc: 获取省略后的富文本;
   * param ellipses_color: 省略号颜色;
   * param ellipses_font: 省略号使用的字体;
   *********************************************************************/
  RichString elidedText(int width, const QColor& ellipses_color = Qt::black,
                        const QFont& ellipses_font = QFont()) const;

  /************************************************************************
   * desc: 去掉尾部若干字符;
   *********************************************************************/
  void chop(int n);

 private:
  QList<RichChar> str_;
};

/////////////////////////////////////////////////////////////

namespace RichStringTool {
/************************************************************************
 * desc: 绘制富文本;
 * param rect: 需要绘制的区域;
 * param alignment_flags: rect内的对齐方式;
 *********************************************************************/
void draw_rich_text(QPainter& painter, const QRect& rect, int alignment_flags,
                    const RichString& rich_string);

/************************************************************************
 * desc: 获取多行富文本整体的尺寸;
 * param line_spacing: 行间距;
 *********************************************************************/
QSize get_rich_text_size(const QList<RichString>& rich_string_vec,
                         int line_spacing);

/************************************************************************
 * desc: 将富文本按照指定宽度裁切为多行;
 *********************************************************************/
QList<RichString> split_rich_text_by_width(const RichString& rich_str,
                                           int width);
}  // namespace RichStringTool

/////////////////////////////////////////////////////////////

#endif  // CONTROL_RICH_STRING_
