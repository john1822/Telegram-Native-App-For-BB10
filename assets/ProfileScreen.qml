import bb.cascades 1.4

// =============================================================================
// TelegramBB10 — Modern Native Person Profile Screen
// -----------------------------------------------------------------------------
// Pixel-accurate replication of modern Telegram mobile profile interface:
//   * Native ActionBar explicitly HIDDEN.
//   * Top app bar: Transparent header with Back arrow (<--) and 3-Dots overflow.
//   * Profile header:
//       - Centered large circular avatar (96x96dp) with verified initials fallback.
//       - Large bold contact name (e.g. "Deepak").
//       - Subtitle: "last seen recently" with "when?" info badge.
//   * Action tiles row:
//       - "Message" (returns to chat)
//       - "Mute" (toggles notification mute)
//       - "Call" (launches voice call overlay)
//       - "Video" (launches video call overlay)
//   * Bio & Username card (#17212b):
//       - Bio text & "Bio" label.
//       - Username (e.g. "@hyperionrover") & "Username" label + QR code button.
//   * "Add to contacts" row (#17212b):
//       - User bust plus icon + "Add to contacts" action.
//   * Common groups section:
//       - "Groups" selected capsule pill.
//       - List of common groups with avatars, names, and member counts.
// =============================================================================

Page {
    id: profilePage
    actionBarVisibility: ChromeVisibility.Hidden

    // Profile metadata
    property string profileTitle: "User"
    property string peerId: ""
    property string accessHash: ""
    property string initials: "?"
    property string avatarColor: "#5288c1"
    property string avatarPath: ""
    property string usernameStr: ""
    property string bioText: ""
    property bool isMuted: false
    property bool showOverflowMenu: false
    property bool showCallOverlay: false
    property NavigationPane navPane: null

    function goBack() {
        chatList.logDiagnostic("ProfileScreen: goBack() called");
        if (navPane) {
            navPane.pop();
        } else if (typeof rootNavPane !== "undefined" && rootNavPane) {
            rootNavPane.pop();
        } else if (profilePage.parent && typeof profilePage.parent.pop === "function") {
            profilePage.parent.pop();
        }
    }

    function loadProfile(title, id, hash, init, color, pic, uName, bio, nav) {
        profileTitle = title ? title : "User";
        peerId = id ? ("" + id) : "";
        accessHash = hash ? ("" + hash) : "";
        initials = init ? init : "?";
        avatarColor = color ? color : "#5288c1";
        avatarPath = pic ? pic : "";
        usernameStr = uName ? uName : "";
        bioText = bio ? bio : "";
        isMuted = false;
        showOverflowMenu = false;
        showCallOverlay = false;
        if (nav !== undefined && nav !== null) {
            navPane = nav;
        }

        // Trigger real-time MTProto loading
        chatList.loadUserProfile(peerId, accessHash, usernameStr);
    }

    content: Container {
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        background: Color.create("#0e1621")

        // -------------------------------------------------------------
        // MAIN SCROLLABLE CONTAINER
        // -------------------------------------------------------------
        ScrollView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            scrollRole: ScrollRole.Main

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                bottomPadding: 32.0

                // =============================================================
                // 1. TOP APP BAR (Back Button + Overflow Menu)
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    preferredHeight: 74.0
                    minHeight: 70.0
                    topPadding: 14.0
                    bottomPadding: 6.0
                    leftPadding: 6.0
                    rightPadding: 8.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                    // Back Button
                    Container {
                        preferredWidth: 50.0
                        preferredHeight: 52.0
                        minWidth: 50.0
                        minHeight: 52.0
                        verticalAlignment: VerticalAlignment.Center
                        layout: DockLayout {}
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    profilePage.goBack();
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_back.png"
                            preferredWidth: 46.0
                            preferredHeight: 46.0
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
                        preferredHeight: 52.0
                        minWidth: 48.0
                        minHeight: 52.0
                        verticalAlignment: VerticalAlignment.Center
                        layout: DockLayout {}
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    profilePage.showOverflowMenu = !profilePage.showOverflowMenu;
                                    chatList.logDiagnostic("ProfileScreen: Overflow menu tapped");
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_more.png"
                            preferredWidth: 44.0
                            preferredHeight: 46.0
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
                    bottomMargin: 16.0

                    // Large Circular Avatar (96x96dp)
                    Container {
                        preferredWidth: 96.0
                        preferredHeight: 96.0
                        minWidth: 96.0
                        minHeight: 96.0
                        background: Color.create(avatarColor ? avatarColor : "#5288c1")
                        horizontalAlignment: HorizontalAlignment.Center
                        layout: DockLayout {}

                        Label {
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            text: initials ? initials : "?"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.XLarge
                            textStyle.fontWeight: FontWeight.Bold
                        }

                        ImageView {
                            visible: avatarPath && avatarPath.length > 0 && chatList.fileExists(avatarPath)
                            imageSource: (avatarPath && avatarPath.length > 0 && chatList.fileExists(avatarPath)) ? (avatarPath.indexOf("file://") === 0 ? avatarPath : "file://" + avatarPath) : ""
                            preferredWidth: 96.0
                            preferredHeight: 96.0
                            scalingMethod: ScalingMethod.AspectFill
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    // Full Contact Name
                    Label {
                        text: profileTitle ? profileTitle : "User"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        topMargin: 12.0
                    }

                    // Status and "when?" link badge
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        topMargin: 4.0
                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                        Label {
                            text: chatList.profileStatus && chatList.profileStatus.length > 0 ? chatList.profileStatus : "last seen recently"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XSmall
                            verticalAlignment: VerticalAlignment.Center
                        }

                        Container {
                            background: Color.create("#223344")
                            leftMargin: 6.0
                            topPadding: 2.0
                            bottomPadding: 2.0
                            leftPadding: 6.0
                            rightPadding: 6.0
                            verticalAlignment: VerticalAlignment.Center
                            Label {
                                text: "when?"
                                textStyle.color: Color.create("#d0d7de")
                                textStyle.fontSize: FontSize.XXSmall
                                textStyle.fontWeight: FontWeight.Bold
                            }
                        }
                    }
                }

                // =============================================================
                // 3. ACTION TILES ROW (Message, Mute, Call, Video)
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    leftMargin: 14.0
                    rightMargin: 14.0
                    topMargin: 8.0
                    bottomMargin: 16.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                    // Message Tile
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
                                    chatList.logDiagnostic("ProfileScreen: Message tapped");
                                    profilePage.goBack();
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_msg.png"
                            preferredWidth: 48.0
                            preferredHeight: 48.0
                            scalingMethod: ScalingMethod.AspectFit
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            text: "Message"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.XSmall
                            textStyle.textAlign: TextAlign.Center
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 4.0
                        }
                    }

                    // Mute Tile
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
                                    profilePage.isMuted = !profilePage.isMuted;
                                    chatList.logDiagnostic("ProfileScreen: Mute toggled: " + profilePage.isMuted);
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_mute.png"
                            preferredWidth: 48.0
                            preferredHeight: 48.0
                            scalingMethod: ScalingMethod.AspectFit
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            text: profilePage.isMuted ? "Unmute" : "Mute"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.XSmall
                            textStyle.textAlign: TextAlign.Center
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 4.0
                        }
                    }

                    // Call Tile
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
                                    chatList.logDiagnostic("ProfileScreen: Call tapped");
                                    profilePage.showCallOverlay = true;
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_call.png"
                            preferredWidth: 48.0
                            preferredHeight: 48.0
                            scalingMethod: ScalingMethod.AspectFit
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            text: "Call"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.XSmall
                            textStyle.textAlign: TextAlign.Center
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 4.0
                        }
                    }

                    // Video Tile
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
                                    chatList.logDiagnostic("ProfileScreen: Video call tapped");
                                    profilePage.showCallOverlay = true;
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_video.png"
                            preferredWidth: 48.0
                            preferredHeight: 48.0
                            scalingMethod: ScalingMethod.AspectFit
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            text: "Video"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.XSmall
                            textStyle.textAlign: TextAlign.Center
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 4.0
                        }
                    }
                }

                // =============================================================
                // 4. BIO & USERNAME CARD (#17212b)
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    background: Color.create("#17212b")
                    leftMargin: 14.0
                    rightMargin: 14.0
                    topMargin: 8.0
                    topPadding: 16.0
                    bottomPadding: 16.0
                    leftPadding: 16.0
                    rightPadding: 16.0

                    // Bio Text
                    Label {
                        text: (chatList.profileBio && chatList.profileBio.length > 0) ? chatList.profileBio : ((bioText && bioText.length > 0) ? bioText : "Retro tech Lover Nokia Fan And Loves Everything Linux")
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Medium
                        multiline: true
                    }
                    Label {
                        text: "Bio"
                        textStyle.color: Color.create("#7f8c99")
                        textStyle.fontSize: FontSize.XXSmall
                        topMargin: 4.0
                    }

                    // Divider Line
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        preferredHeight: 1.0
                        background: Color.create("#223040")
                        topMargin: 12.0
                        bottomMargin: 12.0
                    }

                    // Username Row
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                        Container {
                            layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                            verticalAlignment: VerticalAlignment.Center

                            Label {
                                text: (chatList.profileUsername && chatList.profileUsername.length > 0) ? (chatList.profileUsername.indexOf("@") === 0 ? chatList.profileUsername : "@" + chatList.profileUsername) : ((usernameStr && usernameStr.length > 0) ? (usernameStr.indexOf("@") === 0 ? usernameStr : "@" + usernameStr) : "@hyperionrover")
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Medium
                            }
                            Label {
                                text: "Username"
                                textStyle.color: Color.create("#7f8c99")
                                textStyle.fontSize: FontSize.XXSmall
                                topMargin: 4.0
                            }
                        }

                        // QR Code Icon Button
                        Container {
                            preferredWidth: 46.0
                            preferredHeight: 46.0
                            verticalAlignment: VerticalAlignment.Center
                            layout: DockLayout {}
                            gestureHandlers: [
                                TapHandler {
                                    onTapped: {
                                        chatList.logDiagnostic("ProfileScreen: QR Code tapped");
                                    }
                                }
                            ]
                            ImageView {
                                imageSource: "asset:///images/tg_qrcode.png"
                                preferredWidth: 38.0
                                preferredHeight: 38.0
                                scalingMethod: ScalingMethod.AspectFit
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
                            }
                        }
                    }
                }

                // =============================================================
                // 5. ADD TO CONTACTS ROW (#17212b)
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    background: Color.create("#17212b")
                    leftMargin: 14.0
                    rightMargin: 14.0
                    topMargin: 10.0
                    topPadding: 14.0
                    bottomPadding: 14.0
                    leftPadding: 16.0
                    rightPadding: 16.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("ProfileScreen: Add to contacts tapped for " + profileTitle);
                            }
                        }
                    ]

                    ImageView {
                        imageSource: "asset:///images/tg_add_user.png"
                        preferredWidth: 38.0
                        preferredHeight: 38.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }

                    Label {
                        text: "Add to contacts"
                        textStyle.color: Color.create("#6cb5f3")
                        textStyle.fontSize: FontSize.Medium
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                // =============================================================
                // 6. GROUPS TAB PILL
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    topMargin: 18.0
                    bottomMargin: 10.0

                    Container {
                        background: Color.create("#2b5278")
                        topPadding: 6.0
                        bottomPadding: 6.0
                        leftPadding: 18.0
                        rightPadding: 18.0

                        Label {
                            text: "Groups"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                            textStyle.textAlign: TextAlign.Center
                        }
                    }
                }

                // =============================================================
                // 7. COMMON GROUPS LIST CARD (#17212b)
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    background: Color.create("#17212b")
                    leftMargin: 14.0
                    rightMargin: 14.0
                    bottomMargin: 24.0
                    topPadding: 8.0
                    bottomPadding: 8.0

                    Label {
                        visible: !chatList.commonChatsModel || chatList.commonChatsModel.size() === 0
                        text: "No common groups"
                        textStyle.color: Color.create("#7f8c99")
                        textStyle.fontSize: FontSize.Small
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        topMargin: 12.0
                        bottomMargin: 12.0
                    }

                    ListView {
                        visible: chatList.commonChatsModel && chatList.commonChatsModel.size() > 0
                        horizontalAlignment: HorizontalAlignment.Fill
                        preferredHeight: chatList.commonChatsModel ? (chatList.commonChatsModel.size() * 58.0) : 58.0
                        dataModel: chatList.commonChatsModel

                        listItemComponents: [
                            ListItemComponent {
                                type: "item"
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    topPadding: 8.0
                                    bottomPadding: 8.0
                                    leftPadding: 14.0
                                    rightPadding: 14.0
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                    // Circular Group Avatar (42x42dp)
                                    Container {
                                        preferredWidth: 42.0
                                        preferredHeight: 42.0
                                        minWidth: 42.0
                                        minHeight: 42.0
                                        background: Color.create(ListItemData.avatarColor ? ListItemData.avatarColor : "#5288c1")
                                        verticalAlignment: VerticalAlignment.Center
                                        layout: DockLayout {}

                                        Label {
                                            horizontalAlignment: HorizontalAlignment.Center
                                            verticalAlignment: VerticalAlignment.Center
                                            text: ListItemData.initials ? ListItemData.initials : "G"
                                            textStyle.color: Color.White
                                            textStyle.fontSize: FontSize.Small
                                            textStyle.fontWeight: FontWeight.Bold
                                        }

                                        ImageView {
                                            visible: ListItemData.avatarPath && ListItemData.avatarPath.length > 0 && chatList.fileExists(ListItemData.avatarPath)
                                            imageSource: (ListItemData.avatarPath && ListItemData.avatarPath.length > 0 && chatList.fileExists(ListItemData.avatarPath)) ? (ListItemData.avatarPath.indexOf("file://") === 0 ? ListItemData.avatarPath : "file://" + ListItemData.avatarPath) : ""
                                            preferredWidth: 42.0
                                            preferredHeight: 42.0
                                            scalingMethod: ScalingMethod.AspectFill
                                            horizontalAlignment: HorizontalAlignment.Center
                                            verticalAlignment: VerticalAlignment.Center
                                        }
                                    }

                                    // Group Title and Member Count
                                    Container {
                                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                        verticalAlignment: VerticalAlignment.Center
                                        leftMargin: 12.0

                                        Label {
                                            text: ListItemData.title ? ListItemData.title : "Group"
                                            textStyle.color: Color.White
                                            textStyle.fontSize: FontSize.Medium
                                            textStyle.fontWeight: FontWeight.Bold
                                        }
                                        Label {
                                            text: ListItemData.membersText ? ListItemData.membersText : "members"
                                            textStyle.color: Color.create("#7f8c99")
                                            textStyle.fontSize: FontSize.XXSmall
                                        }
                                    }
                                }
                            }
                        ]
                    }
                }
            }
        }

        // =============================================================
        // POPUP: 3-DOTS OVERFLOW MENU
        // =============================================================
        Container {
            visible: profilePage.showOverflowMenu
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Top
            topMargin: 74.0
            rightMargin: 12.0
            minWidth: 190.0
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
                    text: "\uD83D\uDCE4  Share contact"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            profilePage.showOverflowMenu = false;
                            chatList.logDiagnostic("ProfileScreen: Share contact clicked");
                        }
                    }
                ]
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 40.0
                layout: DockLayout {}
                Label {
                    text: "\u270F  Edit contact"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            profilePage.showOverflowMenu = false;
                            chatList.logDiagnostic("ProfileScreen: Edit contact clicked");
                        }
                    }
                ]
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 40.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDDD1  Delete contact"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            profilePage.showOverflowMenu = false;
                            chatList.logDiagnostic("ProfileScreen: Delete contact clicked");
                        }
                    }
                ]
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 40.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDEAB  Block user"
                    textStyle.color: Color.create("#e53935")
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            profilePage.showOverflowMenu = false;
                            chatList.logDiagnostic("ProfileScreen: Block user clicked");
                        }
                    }
                ]
            }
        }

        // =============================================================
        // OVERLAY: CALL SCREEN
        // =============================================================
        Container {
            visible: profilePage.showCallOverlay
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            background: Color.create("#0f1b26")
            topPadding: 32.0
            bottomPadding: 32.0
            leftPadding: 24.0
            rightPadding: 24.0
            layout: DockLayout {}

            Container {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center

                Container {
                    preferredWidth: 84.0
                    preferredHeight: 84.0
                    background: Color.create(avatarColor ? avatarColor : "#5288c1")
                    horizontalAlignment: HorizontalAlignment.Center
                    bottomMargin: 16.0
                    layout: DockLayout {}

                    Label {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        text: initials ? initials : "?"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                    }

                    ImageView {
                        visible: avatarPath && avatarPath.length > 0 && chatList.fileExists(avatarPath)
                        imageSource: (avatarPath && avatarPath.length > 0 && chatList.fileExists(avatarPath)) ? (avatarPath.indexOf("file://") === 0 ? avatarPath : "file://" + avatarPath) : ""
                        preferredWidth: 84.0
                        preferredHeight: 84.0
                        scalingMethod: ScalingMethod.AspectFill
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                Label {
                    text: profileTitle ? profileTitle : "Telegram User"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.XLarge
                    textStyle.fontWeight: FontWeight.Bold
                    horizontalAlignment: HorizontalAlignment.Center
                    bottomMargin: 6.0
                }

                Label {
                    text: "calling..."
                    textStyle.color: Color.create("#6cb5f3")
                    textStyle.fontSize: FontSize.Medium
                    horizontalAlignment: HorizontalAlignment.Center
                    bottomMargin: 40.0
                }

                Container {
                    preferredWidth: 64.0
                    preferredHeight: 64.0
                    background: Color.create("#e53935")
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: DockLayout {}
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                profilePage.showCallOverlay = false;
                                chatList.logDiagnostic("ProfileScreen: Call ended");
                            }
                        }
                    ]
                    Label {
                        text: "\u2716"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    }
}
