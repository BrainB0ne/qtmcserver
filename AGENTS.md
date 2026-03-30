# AGENTS.md - Coding Guidelines for Qt Minecraft Server

## Build Commands

### CMake Build (Primary)
```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Clean
cmake --build build --target clean
```

### QMake Build (Legacy)
```bash
# Configure
qmake qtmcserver.pro

# Build
make  # or nmake on Windows
```

## Project Structure
- C++17 Qt5 GUI application for managing Minecraft servers
- Uses Qt Widgets, Network modules
- UI files (.ui) use Qt Designer format
- Resource file: qtmcserver.qrc
- Windows executable with RC file

## Code Style Guidelines

### File Headers
Every source file must include the GPL v3 license header:
```cpp
/*
 * Qt Minecraft Server
 * Copyleft 2013
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
```

### Naming Conventions
- **Classes**: PascalCase (e.g., `MainWindow`, `SettingsDialog`)
- **Member variables**: `m_` prefix + camelCase (e.g., `m_pSettings`, `m_mcServerPath`)
- **Pointers**: `m_p` prefix (e.g., `m_pServerProcess`, `m_pFileSystemWatcher`)
- **Methods**: camelCase (e.g., `loadSettings()`, `on_actionStart_triggered()`)
- **UI pointers**: `ui` (e.g., `ui->setupUi(this)`)
- **Qt slots**: `on_` prefix for auto-connect slots

### Include Order
```cpp
// 1. Own header
#include "classname.h"

// 2. UI generated header
#include "ui_classname.h"

// 3. Other project headers
#include "otherdialog.h"

// 4. Qt headers (alphabetical)
#include <QApplication>
#include <QFileDialog>
```

### Class Structure
```cpp
class ClassName : public QMainWindow  // or QDialog
{
    Q_OBJECT
    
public:
    explicit ClassName(QWidget *parent = 0);
    ~ClassName();
    void initialize();
    
public slots:
    // Slot implementations
    
protected:
    // Override protected methods
    
private slots:
    // UI-connected slots
    
private:
    void helperMethods();
    
    // Member variables
    Ui::ClassName *ui;
    QLabel *statusLabel;
    QString m_memberVar;
    QProcess *m_pProcess;
};
```

### Memory Management
- Initialize pointers to 0 in constructor: `m_pPointer = 0;`
- Check before delete: `if(m_pPointer) { delete m_pPointer; m_pPointer = 0; }`
- Parent-child relationships: Pass `this` to Qt objects for auto-cleanup
- Dialogs: Create with `new`, call `exec()`, then `delete`

### Qt Conventions
- Use `tr()` for all user-visible strings
- Use Qt containers: `QString`, `QStringList`, `QList`, `QMap`
- Prefer `QString::arg()` for string formatting
- Use `QDir::toNativeSeparators()` for paths in UI
- File paths: Check `isEmpty()` and `QFile::exists()` before operations

### Signal/Slot Connections
- Old syntax: `connect(sender, SIGNAL(signal()), this, SLOT(slot()))`
- Check object validity: `if(trayIcon) { trayIcon->show(); }`

### Error Handling
- Check QFile operations with `if(!file.open()) { return; }`
- Validate paths before file operations
- Use `QMessageBox` for user notifications
- Check process state before operations

### UI Files
- Form files: `.ui` extension
- Generated headers: `ui_filename.h`
- All forms include standard buttons (OK/Cancel)
- Resource prefix: `://images/`

### Git Workflow
- Target branch: `master`
- CI runs on Windows (MSVC 2019) and Ubuntu (GCC)
- No tests currently configured in CMake

## License
All code must be GPL v3 licensed with Copyleft 2013 attribution.
