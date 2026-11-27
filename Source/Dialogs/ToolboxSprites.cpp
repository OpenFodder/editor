#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_ToolboxSprites.h"

#include <QLineEdit>
#include <QMouseEvent>
#include <QPixmap>
#include <QScrollBar>
#include <QSet>
#include <QSignalBlocker>
#include <QTreeWidget>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

namespace {
constexpr int SpriteTypeRole = Qt::UserRole;
constexpr int SpriteImageRole = Qt::UserRole + 1;

bool IsSprite(const QTreeWidgetItem* item) {
    return item && item->data(0, SpriteTypeRole).isValid();
}

void UpdatePreview(QTreeWidgetItem* item, int size) {
    const auto image = item->data(0, SpriteImageRole).value<QImage>();
    item->setIcon(0, QPixmap::fromImage(image.scaled(QSize(size, size), Qt::KeepAspectRatio)));
}
}

cToolboxSprites::cToolboxSprites(QWidget* parent) : QWidget(parent) {
    Ui_ToolboxSprites ui;
    ui.setupUi(this);
    mSprites = ui.mSpriteList;
    mSearch = ui.mSpriteSearch;
    mCurrentSprite = ui.mCurrentSprite;
    mCurrentSprite->setMinimumHeight(mCurrentSprite->fontMetrics().lineSpacing() * 2);
    mCurrentSprite->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    mSprites->setIconSize(QSize(mPreviewSize, mPreviewSize));
    mSprites->viewport()->setMouseTracking(true);
    mSprites->viewport()->installEventFilter(this);
    connect(mSprites, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem* item) {
        if (IsSprite(item)) {
            mSelected = item;
            g_OFED->SetCursorSprite(item->data(0, SpriteTypeRole).toUInt());
        }
        UpdateStatus();
    });
    connect(mSearch, &QLineEdit::textChanged, this, &cToolboxSprites::ApplySearch);
    connect(mSearch, &QLineEdit::returnPressed, this, [this]() {
        for (int i = 0; i < mSprites->topLevelItemCount(); ++i) {
            const auto* group = mSprites->topLevelItem(i);
            if (group->isHidden() || !group->isExpanded()) continue;
            for (int row = 0; row < group->childCount(); ++row) {
                auto* sprite = group->child(row);
                if (sprite->isHidden()) continue;
                mSprites->setCurrentItem(sprite);
                mSprites->setFocus();
                return;
            }
        }
    });
    connect(mSprites, &QTreeWidget::itemCollapsed, this, [this](const QTreeWidgetItem* group) {
        if (mSearch->text().simplified().isEmpty()) mCollapsedGroups.insert(group->text(0));
    });
    connect(mSprites, &QTreeWidget::itemExpanded, this, [this](const QTreeWidgetItem* group) {
        if (mSearch->text().simplified().isEmpty()) mCollapsedGroups.remove(group->text(0));
    });
}

void cToolboxSprites::RenderSprites() {
    if (g_OFED && g_OFED->GetCursorSpriteID() >= 0 && !IsPlaceableSprite(g_OFED->GetCursorSpriteID()))
        g_OFED->CursorReset();

    const QSignalBlocker block(mSprites);
    const auto window = std::dynamic_pointer_cast<cWindowQT>(g_Window);
    const int selected = g_OFED ? g_OFED->GetCursorSpriteID() : -1;
    const int scroll = mSprites->verticalScrollBar()->value();

    g_Fodder->mGraphics->SetActiveSpriteSheet(eGFX_IN_GAME);
    mSelected = nullptr;
    mSprites->clear();
    QSet<int> listed;
    const auto addSprite = [&](QTreeWidgetItem*& group, const QString& title, int type) {
        listed.insert(type);
        if (!IsPlaceableSprite(type)) return;
        const auto image = window->SpritePaletteImage(type);
        if (image.isNull()) return;

        if (!group) {
            group = new QTreeWidgetItem(mSprites, {title});
            group->setFlags(Qt::ItemIsEnabled);
            auto font = group->font(0);
            font.setBold(true);
            group->setFont(0, font);
            group->setBackground(0, mSprites->palette().brush(QPalette::AlternateBase));
        }
        const QString name = QString::fromStdString(g_SpriteName[type]);
        auto* item = new QTreeWidgetItem(group, {name});
        item->setData(0, SpriteTypeRole, type);
        item->setData(0, SpriteImageRole, image);
        item->setToolTip(0, name);
        UpdatePreview(item, mPreviewSize);
        if (type == selected) mSelected = item;
    };
    const auto addGroup = [&](const QString& title, std::initializer_list<int> types) {
        QTreeWidgetItem* group = nullptr;
        for (int type : types) addSprite(group, title, type);
    };

    addGroup(tr("Soldiers"), {
        eSprite_Player, eSprite_Enemy, eSprite_Enemy_Rocket, eSprite_Enemy_Leader,
        eSprite_Floating_Dead_Soldier
    });
    addGroup(tr("Civilians and hostages"), {
        eSprite_Civilian, eSprite_Civilian_Fast, eSprite_Civilian_Spear,
        eSprite_Civilian_Invisible, eSprite_Hostage, eSprite_Hostage_Rescue_Tent
    });
    addGroup(tr("Ground vehicles"), {
        eSprite_VehicleNoGun_Human, eSprite_VehicleGun_Human,
        eSprite_VehicleNoGun_Enemy, eSprite_VehicleGun_Enemy, eSprite_Vehicle_Unk_Enemy,
        eSprite_Tank_Human, eSprite_Tank_Enemy,
        eSprite_Looping_Vehicle_Left, eSprite_Looping_Vehicle_Right,
        eSprite_Looping_Vehicle_Up, eSprite_Looping_Vehicle_Down
    });
    addGroup(tr("Helicopters and call pads"), {
        eSprite_Helicopter_Grenade_Human, eSprite_Helicopter_Unarmed_Human,
        eSprite_Helicopter_Missile_Human, eSprite_Helicopter_Homing_Human,
        eSprite_Helicopter_Grenade_Human_Called, eSprite_Helicopter_Unarmed_Human_Called,
        eSprite_Helicopter_Missile_Human_Called, eSprite_Helicopter_Homing_Human_Called,
        eSprite_Helicopter_Grenade_Enemy, eSprite_Helicopter_Unarmed_Enemy,
        eSprite_Helicopter_Missile_Enemy, eSprite_Helicopter_Homing_Enemy,
        eSprite_Helicopter_Homing_Enemy_Called, eSprite_Helicopter_CallPad, eSprite_UFO_Callpad
    });
    addGroup(tr("Turrets and cannons"), {
        eSprite_Cannon, eSprite_Turret_Cannon_Human, eSprite_Turret_Missile_Human,
        eSprite_Turret_Cannon_Enemy, eSprite_Turret_Missile_Enemy, eSprite_Turret_HomingMissile_Enemy,
        eSprite_Turret_Cannon_Invulnerable, eSprite_Turret_Missile_Invulnerable
    });
    addGroup(tr("Buildings and doors"), {
        eSprite_BuildingRoof, eSprite_BuildingDoor, eSprite_BuildingDoor_Hut,
        eSprite_Door_Civilian, eSprite_Door_Civilian_Fast, eSprite_Door_Civilian_Spear,
        eSprite_Door_Civilian_Rescue, eSprite_BunkerDoor_HeavyExplosionOnly,
        eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly
    });
    addGroup(tr("Pickups and bonuses"), {
        eSprite_GrenadeBox, eSprite_RocketBox, eSprite_Bonus_Rockets, eSprite_Bonus_Armour,
        eSprite_Bonus_RankToGeneral, eSprite_Bonus_RankHomingInvin_SquadLeader,
        eSprite_Bonus_RankHomingInvin_Squad
    });
    addGroup(tr("Hazards"), {
        eSprite_Mine, eSprite_Mine_SlideTrigger, eSprite_Spike, eSprite_BoilingPot,
        eSprite_Seal_Mine, eSprite_Spider_Mine
    });
    addGroup(tr("Scenery and wildlife"), {
        eSprite_Shrub, eSprite_Shrub2, eSprite_Tree, eSprite_Snowman, eSprite_Waterfall,
        eSprite_Bird_Left_Wraparound, eSprite_Bird_Left_CameraRespawn, eSprite_Bird_Right_CameraRespawn,
        eSprite_Seal, eSprite_Flashing_Light
    });
    addGroup(tr("Computers"), {eSprite_Computer_1, eSprite_Computer_2, eSprite_Computer_3});

    QTreeWidgetItem* other = nullptr;
    for (int type = 0; type < 118; ++type)
        if (!listed.contains(type)) addSprite(other, tr("Other"), type);

    ApplySearch();
    if (mSelected && !mSelected->isHidden() && mSelected->parent()->isExpanded())
        mSprites->setCurrentItem(mSelected);
    mSprites->doItemsLayout();
    mSprites->verticalScrollBar()->setValue(scroll);
    UpdateStatus();
}

void cToolboxSprites::ApplySearch() {
    const auto terms = mSearch->text().simplified().split(' ', Qt::SkipEmptyParts);
    const QSignalBlocker block(mSprites);
    bool hasMatches = false;
    for (int i = 0; i < mSprites->topLevelItemCount(); ++i) {
        auto* group = mSprites->topLevelItem(i);
        bool groupMatches = false;
        for (int row = 0; row < group->childCount(); ++row) {
            auto* sprite = group->child(row);
            const QString text = group->text(0) + ' ' + sprite->text(0);
            const bool matches = std::all_of(terms.begin(), terms.end(), [&](const QString& term) {
                return text.contains(term, Qt::CaseInsensitive);
            });
            sprite->setHidden(!matches);
            groupMatches |= matches;
        }
        group->setHidden(!groupMatches);
        group->setExpanded(!terms.isEmpty() || !mCollapsedGroups.contains(group->text(0)));
        hasMatches |= groupMatches;
    }
    mNothingMatches = !terms.isEmpty() && !hasMatches;
    UpdateStatus();
}

void cToolboxSprites::UpdateStatus(const QTreeWidgetItem* item) {
    if (!IsSprite(item)) item = mSelected;
    mCurrentSprite->setText(mNothingMatches ? tr("No matching sprites")
        : item ? item->text(0) : tr("Select a sprite to place · Ctrl+wheel to zoom"));
}

void cToolboxSprites::SetPreviewSize(int size, const QPoint& position) {
    if (size == mPreviewSize) return;
    const auto* anchor = mSprites->itemAt(position);
    const int oldTop = anchor ? mSprites->visualItemRect(anchor).top() : 0;
    mPreviewSize = size;
    mSprites->setIconSize(QSize(size, size));
    for (int i = 0; i < mSprites->topLevelItemCount(); ++i) {
        const auto* group = mSprites->topLevelItem(i);
        for (int row = 0; row < group->childCount(); ++row)
            UpdatePreview(group->child(row), size);
    }
    mSprites->doItemsLayout();
    if (anchor) {
        auto* bar = mSprites->verticalScrollBar();
        bar->setValue(bar->value() + mSprites->visualItemRect(anchor).top() - oldTop);
    }
    UpdateStatus(mSprites->itemAt(position));
}

bool cToolboxSprites::eventFilter(QObject* watched, QEvent* event) {
    if (watched == mSprites->viewport()) {
        if (event->type() == QEvent::MouseMove) {
            const auto* mouse = static_cast<QMouseEvent*>(event);
            UpdateStatus(mSprites->itemAt(mouse->position().toPoint()));
        } else if (event->type() == QEvent::Leave) {
            UpdateStatus();
        } else if (event->type() == QEvent::Wheel) {
            auto* wheel = static_cast<QWheelEvent*>(event);
            if (wheel->modifiers().testFlag(Qt::ControlModifier)) {
                const qreal steps = wheel->angleDelta().y() ? wheel->angleDelta().y() / 120.0
                                                            : wheel->pixelDelta().y() / 40.0;
                const auto size = std::clamp(mPreviewSize * std::pow(1.25, steps), 16.0, 72.0);
                SetPreviewSize(qRound(size), wheel->position().toPoint());
                wheel->accept();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void cToolboxSprites::ClearSelection() {
    mSelected = nullptr;
    mSprites->clearSelection();
    mSprites->setCurrentItem(nullptr);
    UpdateStatus();
}
