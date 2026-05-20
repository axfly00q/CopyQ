// SPDX-License-Identifier: GPL-3.0-or-later

#include "itempreviewpopup.h"
#include "winblur.h"

#include "common/mimetypes.h"

#include <QApplication>
#include <QBuffer>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QRegularExpression>
#include <QScreen>
#include <QShowEvent>
#include <QTextBrowser>
#include <QVBoxLayout>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <dwmapi.h>
#endif

static constexpr int kMaxTextLen   = 3000;
static constexpr int kPopupWidth   = 420;
static constexpr int kPopupMinH    = 80;
static constexpr int kPopupMaxH    = 340;
static constexpr int kCursorOffset = 18;  // px from cursor
static constexpr int kRadius       = 12;
static constexpr int kPadding      = 10;

ItemPreviewPopup::ItemPreviewPopup(QWidget *parent)
    : QWidget(parent,
              Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
                  | Qt::WindowDoesNotAcceptFocus)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFixedWidth(kPopupWidth);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(kPadding, kPadding, kPadding, kPadding);
    root->setSpacing(6);

    // --- Content area ---
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setReadOnly(true);
    m_textBrowser->setOpenLinks(false);
    m_textBrowser->setFrameShape(QFrame::NoFrame);
    m_textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // Transparent so our painted background shows through
    m_textBrowser->setStyleSheet(
        QStringLiteral("QTextBrowser { background: transparent; color: rgba(240,240,245,255); "
                       "border: none; font-size: 12px; }"));
    m_textBrowser->setMinimumHeight(50);
    m_textBrowser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet(QStringLiteral("QLabel { background: transparent; }"));
    m_imageLabel->setMinimumHeight(50);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_imageLabel->hide();

    root->addWidget(m_textBrowser);
    root->addWidget(m_imageLabel);

    // --- Metadata strip ---
    m_metaLabel = new QLabel(this);
    m_metaLabel->setWordWrap(false);
    m_metaLabel->setStyleSheet(
        QStringLiteral("QLabel { background: transparent; color: rgba(180,185,200,200); "
                       "font-size: 11px; }"));
    root->addWidget(m_metaLabel);
}

void ItemPreviewPopup::showPreview(const QVariantMap &data, const QPoint &cursorGlobalPos)
{
    bool isImage = false;
    QStringList metaParts;

    // --- Determine content type and populate widget ---
    // Try PNG image first
    const QByteArray pngData = data.value(QStringLiteral("image/png")).toByteArray();
    const QByteArray bmpData = data.value(QStringLiteral("image/bmp")).toByteArray();

    QImage img;
    if (!pngData.isEmpty())
        img.loadFromData(pngData, "PNG");
    else if (!bmpData.isEmpty())
        img.loadFromData(bmpData, "BMP");

    if (!img.isNull()) {
        isImage = true;
        // Scale to fit popup width while keeping aspect ratio
        const int maxW = kPopupWidth - kPadding * 2;
        const int maxH = 260;
        QPixmap pix = QPixmap::fromImage(img);
        if (pix.width() > maxW || pix.height() > maxH)
            pix = pix.scaled(maxW, maxH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_imageLabel->setPixmap(pix);
        m_imageLabel->show();
        m_textBrowser->hide();
        metaParts << tr("%1 × %2 px").arg(img.width()).arg(img.height());
    } else {
        // Text / binary
        isImage = false;
        m_imageLabel->hide();
        m_textBrowser->show();

        const QString text = QString::fromUtf8(
            data.value(QLatin1String(mimeText)).toByteArray());
        const QString html = QString::fromUtf8(
            data.value(QLatin1String(mimeHtml)).toByteArray());

        if (!text.isEmpty()) {
            const bool trimmed = text.length() > kMaxTextLen;
            const QString display = trimmed ? text.left(kMaxTextLen) + QStringLiteral("…")
                                            : text;
            m_textBrowser->setPlainText(display);
            const int charCount = text.length();
            const int wordCount = text.split(QRegularExpression(QStringLiteral("\\s+")),
                                             Qt::SkipEmptyParts).size();
            metaParts << tr("%L1 字符 / %L2 词").arg(charCount).arg(wordCount);
        } else if (!html.isEmpty()) {
            m_textBrowser->setHtml(html.left(kMaxTextLen));
        } else {
            // Unknown binary
            qsizetype totalBytes = 0;
            for (auto it = data.cbegin(); it != data.cend(); ++it)
                totalBytes += it.value().toByteArray().size();
            m_textBrowser->setPlainText(tr("（二进制数据）"));
            metaParts << tr("%L1 字节").arg(totalBytes);
        }
    }

    // Source window title
    const QString winTitle = QString::fromUtf8(
        data.value(QLatin1String(mimeWindowTitle)).toByteArray());
    if (!winTitle.isEmpty())
        metaParts.prepend(tr("来源: %1").arg(winTitle));

    m_metaLabel->setText(metaParts.join(QStringLiteral("   │   ")));
    m_metaLabel->setVisible(!metaParts.isEmpty());

    // --- Adjust height ---
    const int contentH = isImage ? m_imageLabel->sizeHint().height()
                                 : qMin(m_textBrowser->document()->size().height() + 10,
                                        static_cast<double>(kPopupMaxH - 40));
    const int metaH = m_metaLabel->isVisible() ? m_metaLabel->sizeHint().height() + 6 : 0;
    const int totalH = qBound(kPopupMinH,
                               kPadding * 2 + static_cast<int>(contentH) + 6 + metaH,
                               kPopupMaxH);
    setFixedHeight(totalH);

    // --- Position near cursor with screen boundary check ---
    const QScreen *screen = QGuiApplication::screenAt(cursorGlobalPos);
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    const QRect screenRect = screen->availableGeometry();

    int x = cursorGlobalPos.x() + kCursorOffset;
    int y = cursorGlobalPos.y() + kCursorOffset;

    if (x + kPopupWidth > screenRect.right())
        x = cursorGlobalPos.x() - kPopupWidth - kCursorOffset;
    if (y + totalH > screenRect.bottom())
        y = cursorGlobalPos.y() - totalH - kCursorOffset;

    x = qMax(x, screenRect.left());
    y = qMax(y, screenRect.top());

    move(x, y);
    show();
    raise();
}

void ItemPreviewPopup::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());

    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    // Round corners
    const DWORD round = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &round, sizeof(round));

    CopyQWin11::applyBlur(hwnd);
#endif
}

void ItemPreviewPopup::paintEvent(QPaintEvent *)
{
    // Draw a frosted-glass background so the popup looks distinct even without DWM blur.
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Semi-transparent dark fill
    p.setBrush(QColor(18, 18, 22, 200));
    // Subtle border
    p.setPen(QPen(QColor(255, 255, 255, 28), 1));
    p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), kRadius, kRadius);
}
