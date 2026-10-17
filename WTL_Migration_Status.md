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

`Addins\DittoUtil` 之前是 Win32 整解决方案构建的阻塞点（`MSB8041`），现已修复（`5a89dc2`）：`Release|Win32` 的 `UseOfMfc` 由 `Dynamic` 改为 `false`（与其他配置一致），并为全部 6 个配置补上内嵌 WTL 的包含目录 `$(SolutionDir)wtl\include`——因为它的 `stdafx.h` 经根目录 `StdAfx.h` 间接引用 WTL 头。至此整解决方案四个配置均可构建。

`LNK4098`（LIBCMT/LIBCMTD 与动态 CRT 冲突）已消除——根因是 `EncryptDecrypt.vcxproj` 的 Win32/x64 配置一直没设 `RuntimeLibrary`，MSBuild 默认走静态 CRT（Debug=`/MTd`→LIBCMTD，Release=`/MT`→LIBCMT），而 `CP_Main` 是 `/MDd`/`/MD` 动态 CRT；静态 CRT 库链进动态 CRT 程序会有两套堆，`EncryptDecrypt` 里 ATL/CString 分配的内存在进程调试堆上释放会破坏调试堆链表，表现为启动断言 `debug_heap.cpp: __acrt_last_block == header`。已把 EncryptDecrypt 四个配置改为与 CP_Main 一致（Debug→`/MDd`，Release→`/MD`；ARM64 不构建，保持原样）。剩余告警：若干 `C4244`/`C4018`/`C4101`/`C4996`/`C4715`；Win32 `focus.vcxproj` 的 `LNK4075`（`/EDITANDCONTINUE` 因 `/SAFESEH` 被忽略）。

## 4. 已完成

- 全部编译进 `CP_Main.vcxproj` 的源文件已无 MFC 头依赖；`UseOfMfc=false`。
- 路线 A 的仿真类型（`CWinThread`、`CWinApp` 消息泵、MFC 属性网格、`CStringArray` 等）已删除，改为真实 WTL/ATL/STL 实现。
- 复制线程改为带真实消息泵的线程，线程退出时窗口析构顺序修正（`CopyThread.cpp`）。
- 插件卸载前释放字符串，避免退出时 `CString` 越界（`72a299d`）。
- Debug|x64 与 Debug|Win32 的 CRT 断言清零（`8ef7dd5`）：
  - 21 处 `CFont::DeleteObject()` 加 `IsNull()` 守卫——ATL 断言句柄非空，MFC 返回 FALSE。
  - `CHyperLink` 从 `Attach()` 改为 `SubclassWindow()`——它是 `CWindowImpl` 派生类，`Attach()` 留下无 thunk 的活句柄，`~CWindowImplRoot()` 会断言；这同时恢复了 About 页链接的实际可点。
  - `CTrayNotifyIcon::m_wndInvisible`（静态 `CTrayWnd`）新增 `ReleaseInvisibleWindow()`，在 `ExitInstance()` 调用；`MinimiseToTray()` 会把主框架重新挂到该窗口上，不销毁则进程退出时断言。
  - 死代码 `COptionsUtilities` 整体删除，`OptionsSheet` 中的 `m_pUtilites` 成员与配套 include、new/delete 一并移除。
- 网络接收改为默认关闭、逐项 opt-in（`9fe3277`）。
- 迁移过程中被静默丢掉的行为已按 `git show 2ee24aa` 逐一比对恢复。
- 项目清理（本轮）：
  - 删除 56 个死代码文件（`e6d0476`），含上游从未编译的 MFC 遗留 `SystemTray`、`MyDropTarget`、`ProcessCopy`、`DittoDropTarget`、`EditWnd`，以及从未被使用的 `OptionsUtilities`。
  - 删除旁路项目目录 `Help\`、`WebSite\`、`U3Stop\`、`AccessToSqlite\`（94 个文件，均不在 `CP_Main_10.sln` 内）。
  - 语言包与主题从输出目录 `Debug\` 移到仓库根 `Language\`、`Themes\`，安装脚本与便携打包脚本同步更新（`b191d2c`）。
  - 约 85 个构建产物（`*.obj/*.lib/*.exe/*.dll/*.map/*.log/*.res/*.recipe/*.ilk/*.idb/*.tlh/*.tli/*.user/CP_Main_i.c/h` 等）从版本控制移除，`.gitignore` 扩展覆盖；`DittoSetup` 的 rcedit/ProjectZip 工具和 `zlib\zlibstat.lib` 属构建输入，保留跟踪。磁盘上的 `Debug/Debug64/Release/Release64/x64/Win32/` 及各项目中间目录已清空（约 2 GB），全部可由构建再生。
  - 清理后从零重建 `CP_Main_10.sln` 全部四个配置（Release/Debug × x64/Win32）均 `0 error`，`Ditto.exe` 与 `DittoUtil.dll` 八个产物齐备（`5a89dc2`）。
- 清理后运行验证发现并修复的缺陷：
  - **搜索框无法输入**（`952f6e5` 之前的尝试，最终修复见下）：`CSymbolEdit` 是 `"Edit"` 的 WTL superclass，但迁移时新增了 `MSG_WM_CREATE(OnCreate)` 且 `bHandled` 默认为 TRUE，导致 WTL 不再把 `WM_CREATE` 转给底层真 EDIT 窗口过程，控件文本缓冲未初始化 → 能显示提示文字却打不进字符。修复是从消息表移除 `WM_CREATE` 处理，让 WTL 自动转发给父类过程（等价于 MFC「先建真 Edit 再子类化」的语义）；布局由父窗口随后的 `SetSymbolIcon`/`SetPromptText`/`OnSize` 触发。（注：先用 `OnCreate` 里手动 `DefWindowProc(WM_CREATE)` 的做法会让整个 QuickPaste 窗口创建失败直接退出，已废弃。）
  - **语言/主题下拉框空白**（`952f6e5`）：`GetPath(PATH_LANGUAGE/PATH_THEMES)` 指向 exe 目录，而清理把 `Language\`、`Themes\` 从 `Debug\` 移到仓库根后，四个输出目录旁都没有数据。给 `CP_Main.vcxproj` 全部配置加 `PostBuildEvent`，把 `$(ProjectDir)Language\*`、`$(ProjectDir)Themes\*` 拷到 `$(OutDir)`。
  - **下拉框少一项（如简体中文不列出）**：`FileCompat.h` 里替换 MFC 的 `CFileFind` 语义错了——MFC 的 `FindNextFile()` 首次调用返回 `FindFile()` 已找到的**第一个**文件，而移植版让 `FindFile()` 载入首个后 `FindNextFile()` 直接前进到第二个，于是每个枚举循环都漏掉第一个匹配项（语言、主题等）。加 `m_bFirstPending` 标志还原 MFC 语义。
  - **打包脚本按新 WTL 版本更新**：`dumpbin -dependents` 确认 `Ditto.exe`/`DittoUtil.dll` 已无 MFC 依赖（只剩 `MSVCP140`/`VCRUNTIME140`）。`BuildPortableZIP.bat` 删除两处 `mfc140u.dll` 拷贝、修正 `Changes.txt` 路径为 `..\Changes.txt`；`DittoSetup_10.iss`、`DittoSetup_arm64.iss` 同样删除 `mfc140u.dll` 源行。

## 5. 剩余工作

按风险从低到高：

1. **未注册的 OLE 拖放**：`CQPasteWnd::OnDragEnter/OnDragOver/OnDrop/OnDragLeave` 是空实现，上游 MFC 版本同样如此且 `CMyDropTarget`/`CDittoDropTarget` 从未注册，因此不是迁移丢失，但可顺手清理或真正实现拖放。
2. **告警治理**：C4xxx 系列（`LNK4098` 已随 CRT 统一消除）。
3. **运行验证**：第 6 节清单需人工逐项执行，尤其是本轮清理（死文件、旁路项目、产物出仓）之后重跑四个配置的冒烟测试。

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
