#pragma once
#include <QSet>
#include <QWidget>

class QLabel;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;

class cToolboxSprites : public QWidget {
    QTreeWidget* mSprites;
    QLineEdit* mSearch;
    QLabel* mCurrentSprite;
    QTreeWidgetItem* mSelected = nullptr;
    QSet<QString> mCollapsedGroups;
    int mPreviewSize = 32;
    bool mNothingMatches = false;

    void ApplySearch();
    void UpdateStatus(const QTreeWidgetItem* item = nullptr);
    void SetPreviewSize(int size, const QPoint& position);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    explicit cToolboxSprites(QWidget* parent = nullptr);
    void RenderSprites();
    void ClearSelection();
};
