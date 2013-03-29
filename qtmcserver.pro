#-------------------------------------------------
# Qt Minecraft Server
# Copyleft 2013
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtmcserver
TEMPLATE = app

# ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}

# ensure one "debug" or "release" in CONFIG so they can be used as
# conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}

release {
    OUTPUT_DIR = "release"
}

debug {
    OUTPUT_DIR = "debug"
}

win32 {
RC_FILE = qtmcserver.rc
}

SOURCES += main.cpp\
        mainwindow.cpp \
    licensedialog.cpp \
    aboutdialog.cpp \
    settingsdialog.cpp \
    downloaddialog.cpp

HEADERS  += mainwindow.h \
    licensedialog.h \
    aboutdialog.h \
    settingsdialog.h \
    downloaddialog.h

FORMS    += mainwindow.ui \
    licensedialog.ui \
    aboutdialog.ui \
    settingsdialog.ui \
    downloaddialog.ui

RESOURCES += \
    qtmcserver.qrc

OTHER_FILES += \
    gpl-3.0.txt \
    images/stop.png \
    images/start.png \
    images/settings.png \
    images/qtmcserver-256x256.png \
    images/qtmcserver-16x16.png \
    images/qtmcserver.ico \
    images/exit.png \
    images/about.png \
    images/download.png \
    images/exportlog.png \
    images/clear.png \
    qtmcserver.rc
