/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "fixedplugincontroller.h"
#include "utils.h"
#include "proxyplugincontroller.h"
#include "systempluginwindow.h"

FixedPluginController::FixedPluginController(QObject *parent)
    : AbstractPluginsController(parent)
{
    setObjectName("FixedPluginController");
    ProxyPluginController::instance(PluginType::FixedSystemPlugin)->addProxyInterface(this);
}

FixedPluginController::~FixedPluginController()
{
    ProxyPluginController::instance(PluginType::FixedSystemPlugin)->removeProxyInterface(this);
}

void FixedPluginController::itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    StretchPluginsItem *item = new StretchPluginsItem(itemInter, itemKey);
    m_pluginItems << item;
    Q_EMIT pluginItemInserted(item);
}

void FixedPluginController::itemUpdate(PluginsItemInterface * const itemInter, const QString &)
{
    for (StretchPluginsItem *item : m_pluginItems) {
        if (item->pluginInter() == itemInter) {
            Q_EMIT pluginItemUpdated(item);
            break;
        }
    }
}

void FixedPluginController::itemRemoved(PluginsItemInterface * const itemInter, const QString &)
{
    for (StretchPluginsItem *item : m_pluginItems) {
        if (item->pluginInter() == itemInter) {
            m_pluginItems.removeOne(item);
            Q_EMIT pluginItemRemoved(item);
            item->deleteLater();
            break;
        }
    }
}

bool FixedPluginController::needLoad(PluginsItemInterface *itemInter)
{
    ProxyPluginController *controller = ProxyPluginController::instance(itemInter);
    if (!controller)
        return false;

    QPluginLoader *pluginLoader = controller->pluginLoader(itemInter);
    if (!pluginLoader)
        return false;

    // isFixed配置表示该插件在时尚模式下，显示在最右侧的图标，例如关机图标
    QJsonObject json = pluginLoader->metaData().value("MetaData").toObject();
    if (json.contains("fixed"))
        return json.value("fixed").toBool();

    return false;
}
