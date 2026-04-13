# Ditto项目MFC到WTL转换详细计划

## 当前状态分析

### ✅ 已完成 (40%)
- 项目配置和依赖设置
- 基础头文件(stdafx.h)更新
- 应用程序类(CP_Main)转换
- 主框架窗口(MainFrm)转换
- 关于对话框(About)转换
- WTL兼容性层创建

### 🔄 需要完成 (60%)
- 所有其他对话框类转换
- 所有控件类转换
- 消息映射全面更新
- 数据交换机制替换
- 测试和调试

## 详细转换计划

### 第一阶段：对话框类转换 (优先级：高)

#### 1.1 简单对话框类
- [ ] `AddType` (CAddType) - 添加类型对话框
- [ ] `SelectDB` (CSelectDB) - 选择数据库对话框
- [ ] `GroupName` (CGroupName) - 组名称对话框
- [ ] `MoveToGroupDlg` (CMoveToGroupDlg) - 移动到组对话框

#### 1.2 属性页对话框
- [ ] `OptionsSheet` (COptionsSheet) - 选项属性表
- [ ] `OptionsGeneral` (COptionsGeneral) - 常规选项页
- [ ] `OptionsKeyBoard` (COptionsKeyBoard) - 键盘选项页
- [ ] `OptionsTypes` (COptionsTypes) - 类型选项页
- [ ] `OptionsUtilities` (COptionsUtilities) - 工具选项页
- [ ] `OptionsStats` (COptionsStats) - 统计选项页
- [ ] `OptionsCopyBuffers` (COptionsCopyBuffers) - 复制缓冲区选项页
- [ ] `OptionsFriends` (COptionFriends) - 朋友选项页
- [ ] `OptionsQuickPaste` (COptionsQuickPaste) - 快速粘贴选项页

#### 1.3 复杂对话框
- [ ] `AdvGeneral` (CAdvGeneral) - 高级常规设置
- [ ] `CopyProperties` (CCopyProperties) - 复制属性
- [ ] `FriendDetails` (CFriendDetails) - 朋友详情
- [ ] `FriendPromptDlg` (CFriendPromptDlg) - 朋友提示
- [ ] `FileTransferProgressDlg` (CFileTransferProgressDlg) - 文件传输进度

### 第二阶段：控件类转换 (优先级：高)

#### 2.1 列表控件
- [ ] `CQListCtrl` (主要的列表控件) - **关键组件**
- [ ] `CClipFormatQListCtrl` (剪贴板格式列表)
- [ ] `CGroupTree` (组树形控件)

#### 2.2 编辑控件
- [ ] `CSearchEditBox` (搜索编辑框)
- [ ] `CEditWithButton` (带按钮的编辑框)
- [ ] `CNumberEdit` (数字编辑框)
- [ ] `CSymbolEdit` (符号编辑框)

#### 2.3 组合框控件
- [ ] `CComboBoxSearch` (搜索组合框)
- [ ] `CGroupCombo` (组组合框)

#### 2.4 其他控件
- [ ] `CHListBox` (水平列表框)
- [ ] `CTabCtrl` (标签控件)
- [ ] `CRichEditCtrlEx` (富文本编辑控件)

### 第三阶段：工具窗口转换 (优先级：中)

#### 3.1 弹出窗口
- [ ] `CQPasteWnd` (快速粘贴窗口) - **关键组件**
- [ ] `CPopup` (弹出窗口)
- [ ] `CDittoPopupWindow` (Ditto弹出窗口)
- [ ] `CNoDbFrameWnd` (无数据库框架窗口)

#### 3.2 特殊窗口
- [ ] `CTrayWnd` (托盘窗口)
- [ ] `CImageViewer` (图像查看器)
- [ ] `CQRCodeViewer` (二维码查看器)
- [ ] `CProgressWnd` (进度窗口)

### 第四阶段：辅助类和工具类转换 (优先级：中)

#### 4.1 绘图和图像
- [ ] `CImageHelper` (图像帮助类)
- [ ] `CGdiImageDrawer` (GDI图像绘制)
- [ ] `CFormattedTextDraw` (格式化文本绘制)
- [ ] `CAlphaBlend` (Alpha混合)

#### 4.2 系统工具
- [ ] `CNTray` (新托盘类)
- [ ] `CSystemTray` (系统托盘)
- [ ] `CPowerManager` (电源管理)
- [ ] `CMagneticWnd` (磁性窗口)
- [ ] `CSnapWindow` (快照窗口)

#### 4.3 用户界面工具
- [ ] `CToolTipEx` (扩展工具提示)
- [ ] `CHyperLink` (超链接控件)
- [ ] `CGdipButton` (GDI+按钮)
- [ ] `CDialogResizer` (对话框调整器)

### 第五阶段：核心功能类转换 (优先级：高)

#### 5.1 剪贴板相关
- [ ] `CClipboardViewer` (剪贴板查看器)
- [ ] `CClipboardSaveRestore` (剪贴板保存恢复)
- [ ] `COleClipSource` (OLE剪贴板源)

#### 5.2 数据处理
- [ ] `CClip` (剪贴板项) - **核心类**
- [ ] `CClipIDs` (剪贴板ID管理)
- [ ] `CMainTable` (主表)
- [ ] `CDataTable` (数据表)
- [ ] `CTypesTable` (类型表)

#### 5.3 网络和通信
- [ ] `CClient` (客户端)
- [ ] `CServer` (服务器)
- [ ] `CSendSocket` (发送套接字)
- [ ] `CRecieveSocket` (接收套接字)
- [ ] `CFileSend` (文件发送)
- [ ] `CFileRecieve` (文件接收)

### 第六阶段：消息映射和事件处理 (优先级：高)

#### 6.1 更新所有消息映射
- [ ] 将 `BEGIN_MESSAGE_MAP` → `BEGIN_MSG_MAP`
- [ ] 将 `ON_COMMAND` → `COMMAND_ID_HANDLER`
- [ ] 将 `ON_NOTIFY` → `NOTIFY_HANDLER`
- [ ] 将 `ON_WM_*` → `MSG_WM_*`

#### 6.2 更新消息处理函数签名
- [ ] `afx_msg void` → `LRESULT`
- [ ] 更新所有参数类型
- [ ] 添加返回值处理

#### 6.3 数据交换机制
- [ ] 替换 `DoDataExchange` 机制
- [ ] 实现WTL版本的数据绑定
- [ ] 更新控件变量声明

### 第七阶段：测试和调试 (优先级：最高)

#### 7.1 编译测试
- [ ] 解决所有编译错误
- [ ] 处理链接器问题
- [ ] 检查依赖项

#### 7.2 功能测试
- [ ] 基本界面功能
- [ ] 剪贴板捕获功能
- [ ] 搜索和过滤功能
- [ ] 网络同步功能
- [ ] 数据库操作

#### 7.3 性能测试
- [ ] 内存使用对比
- [ ] 启动时间对比
- [ ] 响应速度对比

#### 7.4 兼容性测试
- [ ] Windows 10/11兼容性
- [ ] 不同DPI设置
- [ ] 多显示器支持

## 转换策略

### 1. 渐进式转换
- 每次只转换一个类或模块
- 转换后立即进行编译测试
- 保持功能完整性

### 2. 备份和版本控制
- 转换前创建完整备份
- 使用Git分支管理转换过程
- 记录每个转换步骤

### 3. 测试驱动
- 为关键功能编写测试用例
- 转换后验证功能正确性
- 性能对比测试

### 4. 优先级排序
1. **关键路径**: 主窗口、列表控件、剪贴板处理
2. **用户界面**: 对话框、菜单、工具栏
3. **辅助功能**: 网络、数据库、系统集成
4. **优化**: 性能、内存、用户体验

## 技术细节

### MFC到WTL类映射表

| MFC类 | WTL替代类 | 转换难度 | 状态 |
|-------|-----------|----------|------|
| CWinApp | CAppModule | 低 | ✅ 完成 |
| CFrameWnd | CFrameWindowImpl | 中 | ✅ 完成 |
| CDialog | CDialogImpl | 中 | 🔄 进行中 |
| CPropertyPage | CPropertyPageImpl | 中 | ✅ 部分完成 |
| CPropertySheet | CPropertySheetImpl | 高 | ⏳ 待开始 |
| CListCtrl | CListViewCtrl | 高 | ⏳ 待开始 |
| CTreeCtrl | CTreeViewCtrl | 中 | ⏳ 待开始 |
| CComboBox | CComboBox | 低 | ⏳ 待开始 |
| CEdit | CEdit | 低 | ⏳ 待开始 |
| CButton | CButton | 低 | ⏳ 待开始 |
| CString | CString (WTL) | 低 | ✅ 完成 |
| CRect | CRect (WTL) | 低 | ✅ 完成 |
| CPoint | CPoint (WTL) | 低 | ✅ 完成 |
| CSize | CSize (WTL) | 低 | ✅ 完成 |

### 消息映射对比

| MFC宏 | WTL宏 | 备注 |
|-------|-------|------|
| BEGIN_MESSAGE_MAP | BEGIN_MSG_MAP | 基本消息映射 |
| END_MESSAGE_MAP | END_MSG_MAP | 结束消息映射 |
| ON_COMMAND | COMMAND_ID_HANDLER | 命令消息处理 |
| ON_NOTIFY | NOTIFY_HANDLER | 通知消息处理 |
| ON_WM_CREATE | MSG_WM_CREATE | 窗口创建消息 |
| ON_WM_DESTROY | MSG_WM_DESTROY | 窗口销毁消息 |
| ON_WM_SIZE | MSG_WM_SIZE | 窗口大小改变 |
| ON_WM_PAINT | MSG_WM_PAINT | 绘图消息 |
| ON_BN_CLICKED | BN_CLICKED_HANDLER | 按钮点击 |

### 函数签名变更

| MFC函数 | WTL函数 | 变化说明 |
|---------|---------|----------|
| virtual BOOL InitInstance() | BOOL InitInstance(HINSTANCE, int) | 添加参数 |
| afx_msg void OnCreate(LPCREATESTRUCT) | LRESULT OnCreate(LPCREATESTRUCT) | 添加返回值 |
| virtual BOOL OnInitDialog() | BOOL OnInitDialog(CWindow, LPARAM) | 参数变化 |
| afx_msg void OnCommand(UINT) | LRESULT OnCommand(WORD, WORD, HWND) | 参数细化 |

## 风险评估和缓解

### 高风险项目
1. **CQListCtrl转换** - 核心功能，复杂度高
   - 缓解：分步骤转换，充分测试
2. **CPropertySheet转换** - 选项对话框复杂
   - 缓解：先转换单个属性页
3. **消息映射全面更新** - 容易出错
   - 缓解：使用脚本辅助，逐步验证

### 中等风险项目
1. **自定义控件绘制** - WTL绘制机制不同
2. **数据交换机制** - 需要重新实现DDX/DDV
3. **第三方库集成** - 可能需要适配

### 低风险项目
1. **简单对话框转换** - 标准模式
2. **基本控件转换** - 直接映射
3. **字符串和几何类** - 兼容性好

## 时间估算

### 阶段时间分配
- **第一阶段** (对话框): 2-3周
- **第二阶段** (控件): 3-4周
- **第三阶段** (工具窗口): 2-3周
- **第四阶段** (辅助类): 1-2周
- **第五阶段** (核心功能): 2-3周
- **第六阶段** (消息映射): 2-3周
- **第七阶段** (测试调试): 3-4周

**总计时间**: 15-22周 (约4-5个月)

## 成功标准

### 功能完整性
- [ ] 所有原有功能正常工作
- [ ] 用户界面保持一致
- [ ] 数据兼容性完好
- [ ] 网络功能正常

### 性能指标
- [ ] 可执行文件大小减少30-50%
- [ ] 内存占用减少20-40%
- [ ] 启动时间提升2-3倍
- [ ] 响应速度提升20-30%

### 质量标准
- [ ] 无内存泄漏
- [ ] 无崩溃问题
- [ ] 多线程安全
- [ ] 高DPI支持良好

## 下一步行动

1. **立即开始**: 转换关键对话框类
2. **建立测试**: 创建基本功能测试用例
3. **逐步推进**: 按照计划顺序执行
4. **持续验证**: 每步都进行编译和功能测试

---

*计划制定时间: 2025-10-11*
*基于当前转换进度: 40%*