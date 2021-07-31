#ifndef _STDAFX_OFED
#define _STDAFX_OFED


#include <qimage.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qdialog.h>
#include <QtWidgets/QMainWindow>
#include <QAbstractTableModel>
#include <qlabel.h>

#include "stdafx.hpp"
#include "WindowQT.hpp"

#include "Models/CampaignModel.hpp"
#include "Models/MissionModel.hpp"
#include "Models/SpriteModel.hpp"

#include "Dialogs/CampaignDialog.hpp"
#include "Dialogs/NewMapDialog.hpp"
#include "Dialogs/ToolboxTiles.hpp"
#include "Dialogs/ToolboxSprites.hpp"
#include "Dialogs/MapView.hpp"

class cOFED;

extern cOFED* g_OFED;
#endif
