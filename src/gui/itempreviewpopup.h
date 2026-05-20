// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QPersistentModelIndex>
#include <QVariantMap>
#include <QWidget>

class QLabel;
class QTextBrowser;

/**
 * Floating hover-preview popup shown when the user hovers over a clipboard item.
 * Displays text content (scrollable) or image preview, plus brief metadata.
 */
class ItemPreviewPopup final : public QWidget
{
    Q_OBJECT
public:
    explicit ItemPreviewPopup(QWidget *parent = nullptr);

    /**
     * Populate the popup with @a itemData (QVariantMap from ClipboardBrowser::copyIndex)
     * and position it near @a cursorGlobalPos, then show it.
     */
    void showPreview(const QVariantMap &itemData, const QPoint &cursorGlobalPos);

protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QTextBrowser *m_textBrowser = nullptr;
    QLabel       *m_imageLabel  = nullptr;
    QLabel       *m_metaLabel   = nullptr;
};
