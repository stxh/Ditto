# DittoWTL — MFC → WTL 迁移现状

本文是唯一有效的迁移状态文档。仓库里其余 11 份 `WTL_*` / `MIGRATION_*` / `MigrationReport` 等文档描述的是**已放弃的路线 A（MFC 仿真 shim）**，只作历史保留，各文件顶部已加说明。

## 1. 当前产品标识

| 项 | 值 |
|---|---|
| 显示名称 | DittoWTL |
| 版本 | 4.0.0.1（`CP_Main.rc` 的 `VS_VERSION_INFO`） |
| 可执行文件 | 仍为 `Ditto.exe`（`TargetName` 未改） |
| 数据目录 | 仍为 `%APPDATA%\Ditto`，INI/注册表键名未改 |

版本只存在于 `CP_Main.rc`：About 框通过 `CInternetUpdate::GetRunningVersion()` 读文件版本，再用 `GetVersionString()` 以 `%02i.%02i.%02i.%02i` 格式化，所以 About 里显示 `04.00.00.01`。改版本号只需改 rc。

改名范围刻意限定在**显示层**：主框架标题、托盘 tooltip、气泡标题、错误框 caption、About 列表、选项页分类名、窗口 caption。以下刻意保持不变，否则会破坏运行数据：`%APPDATA%\Ditto` 路径、便携模式 INI 段名（`Options.cpp`）、主题键比较（`Theme.cpp`）、窗口类名、互斥体名、防火墙规则依赖的 `TargetName=Ditto`。选项页内部若干标签文本（如 "Start Ditto on System Startup"）也未改，它们受 `DittoLang.xml` 语言包覆盖。

## 2. 迁移策略（路线 B）

不使用任何 MFC 仿真类型。每个窗口类都是真实 WTL 实现：`CWindowImpl` / `CDialogImpl` / `CFrameWindowImpl` / `CPropertyPageImpl` / `CPropertySheetImpl`；容器为 `std::vector` / `std::map`；`CString` 为 `ATL::CString`。

- WTL 10 原样内嵌在 `wtl\include`，`CP_Main.vcxproj` 通过 `$(ProjectDir)wtl\include` 引用（`additionalIncludeDirectories`），不依赖外部 WTL 安装。
- `wtl_compatibility.h` 现在只是一个 include 汇总头（ATL → WTL 顺序 + `WTL::CWindow`/`CString` 别名），不含仿真类型。
- MFC 的 `CFile` / `CFileException` / `CFileFind` 由 `FileCompat.h` 的 Win32 实现替代；`COleDataObject` / `COleDataSource` 由 `OleDataCompat.h` 替代。
- `stdafx.h` 保留 `ASSERT`/`VERIFY`/`TRACE`/`DEBUG_NEW` 四个宏映射到 ATL 等价物，这是编译期宏，不是类型仿真。

## 3. 构建

工具链：VS 18 Community（MSVC 14.51.36231），平台工具集 `v145`，Windows SDK 10.0。

```
MSBuild.exe CP_Main.vcxproj -p:Configuration=Release -p:Platform=x64 -m -nologo -v:m
```

| 配置 | 状态 |
|---|---|
| Release / Debug x64 | 可构建、可运行，Debug 无断言 |
| Release / Debug Win32 | 可构建（本次修复，见下） |
| ARM64 | 配置存在于 vcxproj，未验证 |

Win32 之前链接失败（`LNK1181` 找不到 `Release\EncryptDecrypt.lib`，补齐后 `LNK1120` 七个未解析符号）。两个原因：一是 EncryptDecrypt 的输出目录用了 `$(SolutionDir)`，单独构建 `CP_Main.vcxproj` 时该宏不成立，依赖必须先由解决方案或显式构建 `EncryptDecrypt.vcxproj` 产出；二是 `Release|Win32` 是唯一还用 `/MT` 的配置，而 `zlibstatic.lib`/wininet 按导入 CRT 引用 `__imp__*`，且 `AdditionalDependencies` 里漏了 `wininet.lib`。现已改为 `MultiThreadedDLL` 并补上 `wininet.lib`，与 `Release|x64`、`Debug|Win32` 一致。

用整解决方案构建 Win32 仍会在 `Addins\DittoUtil` 上失败：`MSB8041 此项目需要 MFC 库`（见剩余工作第 3 项），这与主程序无关。

已知非阻断告警：链接期 `LNK4098`（LIBCMT/LIBCMTD 冲突，历史遗留）、若干 `C4244`/`C4018`/`C4101`/`C4996`/`C4715`。

## 4. 已完成

- 全部编译进 `CP_Main.vcxproj` 的源文件已无 MFC 头依赖；`UseOfMfc=false`。
- 路线 A 的仿真类型（`CWinThread`、`CWinApp` 消息泵、MFC 属性网格、`CStringArray` 等）已删除，改为真实 WTL/ATL/STL 实现。
- 复制线程改为带真实消息泵的线程，线程退出时窗口析构顺序修正（`CopyThread.cpp`）。
- 插件卸载前释放字符串，避免退出时 `CString` 越界（`72a299d`）。
- Debug|x64 与 Debug|Win32 的 CRT 断言清零（`8ef7dd5`）：
  - 21 处 `CFont::DeleteObject()` 加 `IsNull()` 守卫——ATL 断言句柄非空，MFC 返回 FALSE。
  - `CHyperLink` 从 `Attach()` 改为 `SubclassWindow()`——它是 `CWindowImpl` 派生类，`Attach()` 留下无 thunk 的活句柄，`~CWindowImplRoot()` 会断言；这同时恢复了 About 页链接的实际可点。
  - `CTrayNotifyIcon::m_wndInvisible`（静态 `CTrayWnd`）新增 `ReleaseInvisibleWindow()`，在 `ExitInstance()` 调用；`MinimiseToTray()` 会把主框架重新挂到该窗口上，不销毁则进程退出时断言。
  - `OptionsSheet.cpp` 补 `#include "OptionsUtilities.h"`，修掉 `delete m_pUtilites` 跳过析构的 C4150。
- 网络接收改为默认关闭、逐项 opt-in（`9fe3277`）。
- 迁移过程中被静默丢掉的行为已按 `git show 2ee24aa` 逐一比对恢复。

## 5. 剩余工作

按风险从低到高：

1. **死代码清除**（已转为 WTL/STL 写法但无调用者）：`CRichEditCtrlEx::GetSystemFonts()` + `m_saFontList`、`SimpleBrowser::ParsePostData()`。
2. **未参与编译的遗留 MFC 文件**：`SystemTray.h/.cpp`、`MyDropTarget.h/.cpp`、`ProcessCopy.h/.cpp`、`DittoDropTarget.h/.cpp`、`EditWnd.h`（都不在 vcxproj 中，编译期分别被 `TrayWnd`、WTL 拖放实现、`DittoWindow` 取代）。
3. **旁路项目仍是 MFC**：`AccessToSqlite`、`U3Stop`（都不在 `CP_Main_10.sln` 里，不影响主程序）；`Addins\DittoUtil` 仅 `Release|Win32` 一个配置还是 `UseOfMfc=Dynamic`，导致整解决方案的 Win32 构建在未装 MFC 组件的机器上报 `MSB8041`。
4. **未注册的 OLE 拖放**：`CQPasteWnd::OnDragEnter/OnDragOver/OnDrop/OnDragLeave` 是空实现，上游 MFC 版本同样如此且 `CMyDropTarget`/`CDittoDropTarget` 从未注册，因此不是迁移丢失，但可顺手清理或真正实现拖放。
5. **工作区垃圾**：`*.backup`、`test_*.cpp`、`atledit.h`、`DittoWindow_mfc.h`、`DittoPopupWindow.h.bak`、`ComboBoxSearch.h/.cpp`、`wtl\include_fake_removed`、`wtl\include.backup`、根目录日志/obj，建议清理或加入 `.gitignore`；构建产物（`Release64/`、`Debug64/`、`x64/`、`*.map`、`*.log`、`*.tlh`、`CP_Main_i.c/h`）也被纳入版本控制，源码提交需要显式挑选文件。
6. **告警治理**：`LNK4098` 与 C4xxx 系列。

## 6. 手动验证清单

迁移的正确性只能靠运行验证，不靠编译通过。以下每一项都需人工执行：

| # | 操作 | 预期 |
|---|---|---|
| 1 | 启动 `Ditto.exe`，复制一段文本 | 主框架标题与托盘 tooltip 显示 `DittoWTL`；Quick Paste 列表出现该条 |
| 2 | `Ctrl+~` 打开列表，双击/回车 | 内容粘贴回上一个窗口，格式与复制时一致 |
| 3 | 打开选项，逐个切换全部 8 个属性页 | 每页控件正常绘制，Debug 版不弹断言 |
| 4 | Options → General | "Disable Receiving Clips" 默认勾选 |
| 5 | About 页 | 版本行显示 `04.00.00.01`；两个链接可点击（mailto / 主页） |
| 6 | 托盘菜单 Exit | 进程 2 秒内退出，Debug 版无断言对话框 |
| 7 | 关闭选项后再次粘贴 | 功能不降级（属性页销毁不影响主窗口句柄） |
