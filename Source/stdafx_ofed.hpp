#ifndef _STDAFX_OFED
#define _STDAFX_OFED

#include <qimage.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qdialog.h>
#include <QtWidgets/QMainWindow>
#include <QAbstractTableModel>

#include "stdafx.hpp"
#include "WindowQT.hpp"

#include "Models/CampaignModel.hpp"
#include "Models/MissionModel.hpp"

#include "Dialogs/CampaignDialog.hpp"
#include "Dialogs/NewMapDialog.hpp"
#include "Dialogs/ToolboxTiles.hpp"
#include "Dialogs/ToolboxSprites.hpp"

class cOFED;

extern cOFED* g_OFED;
#endif
