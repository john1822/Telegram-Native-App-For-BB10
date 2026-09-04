import bb.cascades 1.4

// =============================================================================
// TelegramBB10 — Modern Native "My Profile" View
// -----------------------------------------------------------------------------
// Pixel-accurate replication of modern Telegram mobile "My Profile" interface:
//   * Top app bar: QR scanner icon (left) + 3-dots overflow menu (right).
//   * Profile header:
//       - Centered 96x96dp circular avatar with user initial fallback.
//       - Bold white contact name ("John").
//       - Subtitle: "online" in light blue.
//   * 3 Action tiles row (#1c2836):
//       - "Set Photo" (camera + plus icon)
//       - "Edit Info" (pencil icon)
//       - "Settings" (gear icon -> switches to Settings tab)
//   * User details card (#17212b):
//       - Mobile number + "Mobile" label
//       - Bio text + "Bio" label
//       - Username + "Username" label
//   * Pill tabs switcher:
//       - "Posts" (selected blue capsule #2b5278)
//       - "Archived Posts" (inactive grey text)
//   * Posts content area:
//       - "No posts yet..." bold title
//       - "Publish photos and videos to display on your profile page" subtitle
//       - Cyan capsule button: "Add a post" with camera icon (#24a1de)
// =============================================================================

Container {
    id: myProfileRoot
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    background: Color.create("#0e1621")
    layout: DockLayout {}

    property bool showOverflowMenu: false
    property int activeTab: 0 // 0: Posts, 1: Archived

    signal openSettingsRequested()

    ScrollView {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        scrollRole: ScrollRole.Main

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            bottomPadding: 74.0

            // =============================================================
            // 1. TOP APP BAR (QR Scan Button + 3-Dots Menu Button)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                preferredHeight: 72.0
                minHeight: 68.0
                topPadding: 12.0
                bottomPadding: 6.0
                leftPadding: 8.0
                rightPadding: 8.0
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                // QR Scan Button
                Container {
                    preferredWidth: 48.0
                    preferredHeight: 50.0
                    minWidth: 48.0
                    minHeight: 50.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("MyProfile: QR scan button tapped");
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_qr_scan.png"
                        preferredWidth: 44.0
                        preferredHeight: 44.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                // Spacer
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                }

                // 3-Dots Menu Button
                Container {
                    preferredWidth: 48.0
                    preferredHeight: 50.0
                    minWidth: 48.0
                    minHeight: 50.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                myProfileRoot.showOverflowMenu = !myProfileRoot.showOverflowMenu;
                                chatList.logDiagnostic("MyProfile: Overflow menu tapped: " + myProfileRoot.showOverflowMenu);
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_more.png"
                        preferredWidth: 42.0
                        preferredHeight: 44.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }

            // =============================================================
            // 2. PROFILE HEADER (Centered Avatar, Name, Status)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                topMargin: 4.0
                bottomMargin: 14.0

                // Large Circular Avatar (96x96dp)
                Container {
                    preferredWidth: 96.0
                    preferredHeight: 96.0
                    minWidth: 96.0
                    minHeight: 96.0
                    background: Color.create("#24a1de")
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: DockLayout {}

                    Label {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        text: (auth.userName && auth.userName.length > 0) ? auth.userName.trim().left(1).toUpperCase() : "J"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.XLarge
                        textStyle.fontWeight: FontWeight.Bold
                    }
                }

                // Full Contact Name
                Label {
                    text: (auth.userName && auth.userName.length > 0) ? auth.userName : "John"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Large
                    textStyle.fontWeight: FontWeight.Bold
                    textStyle.textAlign: TextAlign.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    topMargin: 12.0
                }

                // Online Status
                Label {
                    text: "online"
                    textStyle.color: Color.create("#6cb5f3")
                    textStyle.fontSize: FontSize.XSmall
                    horizontalAlignment: HorizontalAlignment.Center
                    topMargin: 4.0
                }
            }

            // =============================================================
            // 3. ACTION TILES ROW (Set Photo, Edit Info, Settings)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                leftMargin: 14.0
                rightMargin: 14.0
                topMargin: 10.0
                bottomMargin: 16.0
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                // Tile 1: Set Photo
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    preferredHeight: 88.0
                    minHeight: 84.0
                    background: Color.create("#1c2836")
                    topPadding: 10.0
                    bottomPadding: 8.0
                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("MyProfile: Set Photo tapped");
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_set_photo.png"
                        preferredWidth: 46.0
                        preferredHeight: 46.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        text: "Set Photo"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.XSmall
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        topMargin: 4.0
                    }
                }

                // Tile 2: Edit Info
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    preferredHeight: 88.0
                    minHeight: 84.0
                    background: Color.create("#1c2836")
                    leftMargin: 8.0
                    topPadding: 10.0
                    bottomPadding: 8.0
                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("MyProfile: Edit Info tapped");
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_edit_info.png"
                        preferredWidth: 46.0
                        preferredHeight: 46.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        text: "Edit Info"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.XSmall
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        topMargin: 4.0
                    }
                }

                // Tile 3: Settings
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    preferredHeight: 88.0
                    minHeight: 84.0
                    background: Color.create("#1c2836")
                    leftMargin: 8.0
                    topPadding: 10.0
                    bottomPadding: 8.0
                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("MyProfile: Settings tile tapped");
                                myProfileRoot.openSettingsRequested();
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_settings_tile.png"
                        preferredWidth: 46.0
                        preferredHeight: 46.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        text: "Settings"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.XSmall
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        topMargin: 4.0
                    }
                }
            }

            // =============================================================
            // 4. USER DETAILS CARD (#17212b)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#17212b")
                leftMargin: 14.0
                rightMargin: 14.0
                topMargin: 4.0
                bottomMargin: 14.0
                topPadding: 16.0
                bottomPadding: 16.0
                leftPadding: 16.0
                rightPadding: 16.0

                // Row 1: Mobile
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    Label {
                        text: (auth.userPhone && auth.userPhone.length > 0) ? auth.userPhone : "+91 8950469287"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Medium
                    }
                    Label {
                        text: "Mobile"
                        textStyle.color: Color.create("#7f8c99")
                        textStyle.fontSize: FontSize.XXSmall
                        topMargin: 3.0
                    }
                }

                // Divider 1
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    preferredHeight: 1.0
                    background: Color.create("#223040")
                    topMargin: 12.0
                    bottomMargin: 12.0
                }

                // Row 2: Bio
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    Label {
                        text: (auth.userBio && auth.userBio.length > 0) ? auth.userBio : "Blackberry 10 native apps developer"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Medium
                        multiline: true
                    }
                    Label {
                        text: "Bio"
                        textStyle.color: Color.create("#7f8c99")
                        textStyle.fontSize: FontSize.XXSmall
                        topMargin: 3.0
                    }
                }

                // Divider 2
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    preferredHeight: 1.0
                    background: Color.create("#223040")
                    topMargin: 12.0
                    bottomMargin: 12.0
                }

                // Row 3: Username
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    Label {
                        text: (auth.userHandle && auth.userHandle.length > 0) ? (auth.userHandle.indexOf("@") === 0 ? auth.userHandle : "@" + auth.userHandle) : "@John_the_don_420"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Medium
                    }
                    Label {
                        text: "Username"
                        textStyle.color: Color.create("#7f8c99")
                        textStyle.fontSize: FontSize.XXSmall
                        topMargin: 3.0
                    }
                }
            }

            // =============================================================
            // 5. PILL TABS SWITCHER (Posts / Archived Posts)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                topMargin: 14.0
                bottomMargin: 16.0
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                // Posts Pill
                Container {
                    background: myProfileRoot.activeTab === 0 ? Color.create("#2b5278") : Color.Transparent
                    topPadding: 7.0
                    bottomPadding: 7.0
                    leftPadding: 22.0
                    rightPadding: 22.0
                    gestureHandlers: [
                        TapHandler {
                            onTapped: { myProfileRoot.activeTab = 0; }
                        }
                    ]
                    Label {
                        text: "Posts"
                        textStyle.color: myProfileRoot.activeTab === 0 ? Color.White : Color.create("#7f8c99")
                        textStyle.fontSize: FontSize.Small
                        textStyle.fontWeight: FontWeight.Bold
                        textStyle.textAlign: TextAlign.Center
                    }
                }

                // Archived Posts Pill
                Container {
                    background: myProfileRoot.activeTab === 1 ? Color.create("#2b5278") : Color.Transparent
                    leftMargin: 8.0
                    topPadding: 7.0
                    bottomPadding: 7.0
                    leftPadding: 16.0
                    rightPadding: 16.0
                    gestureHandlers: [
                        TapHandler {
                            onTapped: { myProfileRoot.activeTab = 1; }
                        }
                    ]
                    Label {
                        text: "Archived Posts"
                        textStyle.color: myProfileRoot.activeTab === 1 ? Color.White : Color.create("#7f8c99")
                        textStyle.fontSize: FontSize.Small
                        textStyle.textAlign: TextAlign.Center
                    }
                }
            }

            // =============================================================
            // 6. POSTS EMPTY STATE AREA ("No posts yet... / Add a post")
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                topMargin: 16.0
                bottomMargin: 24.0
                leftPadding: 24.0
                rightPadding: 24.0

                Label {
                    text: "No posts yet..."
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Large
                    textStyle.fontWeight: FontWeight.Bold
                    textStyle.textAlign: TextAlign.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }

                Label {
                    text: "Publish photos and videos to display on your profile page"
                    textStyle.color: Color.create("#7f8c99")
                    textStyle.fontSize: FontSize.XSmall
                    textStyle.textAlign: TextAlign.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    topMargin: 8.0
                    multiline: true
                }

                // Cyan Capsule Action Button: "📷 Add a post"
                Container {
                    background: Color.create("#24a1de")
                    topMargin: 26.0
                    topPadding: 10.0
                    bottomPadding: 10.0
                    leftPadding: 22.0
                    rightPadding: 26.0
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("MyProfile: Add a post tapped");
                            }
                        }
                    ]

                    ImageView {
                        imageSource: "asset:///images/tg_camera_white.png"
                        preferredWidth: 26.0
                        preferredHeight: 26.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }

                    Label {
                        text: "Add a post"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Small
                        textStyle.fontWeight: FontWeight.Bold
                        leftMargin: 8.0
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    }

    // =============================================================
    // POPUP: 3-DOTS OVERFLOW MENU
    // =============================================================
    Container {
        visible: myProfileRoot.showOverflowMenu
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Top
        topMargin: 64.0
        rightMargin: 12.0
        minWidth: 180.0
        background: Color.create("#1e2b38")
        topPadding: 8.0
        bottomPadding: 8.0
        leftPadding: 14.0
        rightPadding: 14.0

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            minHeight: 40.0
            layout: DockLayout {}
            Label {
                text: "\u270F  Edit name"
                textStyle.color: Color.White
                textStyle.fontSize: FontSize.Small
                verticalAlignment: VerticalAlignment.Center
            }
            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        myProfileRoot.showOverflowMenu = false;
                        chatList.logDiagnostic("MyProfile: Edit name tapped");
                    }
                }
            ]
        }

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            minHeight: 40.0
            layout: DockLayout {}
            Label {
                text: "\uD83D\uDEAA  Log out"
                textStyle.color: Color.create("#e53935")
                textStyle.fontSize: FontSize.Small
                verticalAlignment: VerticalAlignment.Center
            }
            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        myProfileRoot.showOverflowMenu = false;
                        auth.logout();
                    }
                }
            ]
        }
    }
}
