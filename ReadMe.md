What is DittoWTL?
- DittoWTL 4.0.0.1 is the WTL port of Ditto, an extension to the Windows Clipboard. You copy something to the Clipboard and Ditto takes what you copied and stores it in a database to retrieve at a later time.
- Ditto 本身的行为、数据库格式和数据目录 (`%APPDATA%\Ditto`) 都没有变化，可执行文件仍为 `Ditto.exe`。界面与窗口标题显示为 DittoWTL，用于区分这个不再依赖 MFC 的分支。

Basic Usage
- Run Ditto
- Copy things to the clipboard, e.g. using Ctrl-C with text selected in a text editor.
- Open Ditto by clicking its icon in the system tray or by pressing its Hot Key which defaults to Ctrl + ` – i.e. hold down Ctrl and press the back-quote (tilde ~) key.
- Double click or press enter on the item to paste it to the previous window.

Building
- Open `CP_Main_10.sln` with Visual Studio 18 (MSVC 14.51, platform toolset v145) and build `Release|x64`; the WTL 10 headers are vendored in `wtl\include`, so no external WTL install is needed.
- Migration status, what is done and what is left: [WTL_Migration_Status.md](WTL_Migration_Status.md).
