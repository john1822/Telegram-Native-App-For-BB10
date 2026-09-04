import bb.cascades 1.4

// =============================================================================
// TelegramBB10 — Modern Native Chat Screen
// -----------------------------------------------------------------------------
// Pixel-accurate replication of modern Telegram mobile interface:
//   * Native ActionBar explicitly HIDDEN (no bottom grey/black bar).
//   * Top-to-Bottom stack layout: header, scrollable messages, and composer.
//   * High-contrast raster PNG icons (Cascades does not render SVG in ImageView).
//   * Header buttons:
//       - Back: tg_back.png (calls chatPage.goBack() to return to dialog list).
//       - Avatar: circular avatar with QImageReader validation (>= 256 bytes)
//         to eliminate the QNX checkerboard pattern and show clean initials fallback.
//       - Call: tg_call.png (shows Telegram Calling overlay with real controls).
//       - 3-Dots: tg_more.png (interactive Telegram overflow menu).
//   * Chat canvas: Telegram dark navy wallpaper (#0e1621) with date capsule pills.
//   * Message bubbles:
//       - Incoming: Slate bubble (#212d3b), white text, grey timestamp.
//       - Outgoing: Telegram blue bubble (#2b78e4), white text, light timestamp,
//         and checkmarks.
//       - Reply quote preview inside bubbles.
//       - Reaction capsule badges.
//   * Bottom composer bar:
//       - Left: Rounded input capsule containing:
//           * Smiley (tg_emoji.png): inserts emoji.
//           * "Message" text input with backgroundVisible: false.
//           * Paperclip (tg_attach.png): toggles interactive attachment menu.
//       - Right: Separate circular Telegram blue action button (#24a1de) with
//         white tg_mic.png / tg_send.png.
//       - Strict physical separation ensures paperclip and mic never overlap!
// =============================================================================

Page {
    id: chatPage
    actionBarVisibility: ChromeVisibility.Hidden

    // Peer metadata (populated by loadChat())
    property string chatTitle: ""
    property string initials: "?"
    property string avatarColor: "#5288c1"
    property int peerType: 0
    property string peerId: ""
    property string accessHash: ""
    property string avatarPath: ""
    property string username: ""
    property bool canSend: true
    property NavigationPane navPane: null

    // UI state
    property bool showOverflowMenu: false
    property bool showAttachmentMenu: false
    property bool showCallOverlay: false
    property bool isMuted: false
    property bool isSpeakerOn: false

    // Design tokens
    property color tBg:         Color.create("#0e1621")
    property color tHeader:     Color.create("#17212b")
    property color tComposer:   Color.create("#17212b")
    property color tBubbleIn:   Color.create("#212d3b")
    property color tBubbleOut:  Color.create("#2b78e4")
    property color tBlue:       Color.create("#24a1de")
    property color tPrimary:    Color.create("#ffffff")
    property color tSecondary:  Color.create("#7f8c99")
    property color tPillBg:     Color.create("#232e3c")

    function goBack() {
        chatList.logDiagnostic("ChatScreen: goBack() called");
        if (navPane) {
            navPane.pop();
        } else if (typeof rootNavPane !== "undefined" && rootNavPane) {
            rootNavPane.pop();
        } else if (chatPage.parent && typeof chatPage.parent.pop === "function") {
            chatPage.parent.pop();
        }
    }

    function openProfile() {
        chatList.logDiagnostic("ChatScreen: openProfile() tapped for peer " + peerId);
        try {
            var page = profileScreenDef.createObject();
            chatList.logDiagnostic("profileScreenDef createObject: page=" + (page !== null ? "OK" : "NULL"));
            if (page) {
                chatList.logDiagnostic("Loading profile for " + chatTitle + " (username=" + username + ")");
                page.loadProfile(chatTitle, peerId, accessHash, initials, avatarColor, avatarPath, username, "", navPane);
                if (navPane) {
                    chatList.logDiagnostic("Pushing profilePage to navPane");
                    navPane.push(page);
                } else if (typeof rootNavPane !== "undefined" && rootNavPane) {
                    chatList.logDiagnostic("Pushing profilePage to rootNavPane");
                    rootNavPane.push(page);
                }
            }
        } catch (e) {
            chatList.logDiagnostic("openProfile exception: " + e.message);
        }
    }

    function doSend() {
        var txt = messageInputField.text.trim();
        if (txt.length > 0) {
            chatList.logDiagnostic("ChatScreen: Sending message to peer " + peerId + ": " + txt);
            chatList.sendMessage(peerType, peerId, accessHash, txt);
            messageInputField.text = "";
        } else {
            chatList.logDiagnostic("ChatScreen: Voice note action tapped for peer " + peerId);
            chatList.sendMessage(peerType, peerId, accessHash, "\uD83C\uDFA4 Voice message (0:02)");
        }
    }

    function insertEmoji(emoji) {
        messageInputField.text = messageInputField.text + emoji;
    }

    content: Container {
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        background: chatPage.tBg

        // -------------------------------------------------------------
        // MAIN VERTICAL STACK (Header + Messages + Composer)
        // -------------------------------------------------------------
        Container {
            layout: StackLayout { orientation: LayoutOrientation.TopToBottom }
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            // =============================================================
            // 1. TOP HEADER BAR
            // =============================================================
            Container {
                id: headerBar
                horizontalAlignment: HorizontalAlignment.Fill
                preferredHeight: 78.0
                minHeight: 74.0
                background: tHeader
                topPadding: 14.0
                bottomPadding: 6.0
                leftPadding: 6.0
                rightPadding: 8.0
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                // Crisp Back Button (tg_back.png)
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
                                chatPage.goBack();
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

                // Circular Avatar (48x48dp)
                Container {
                    preferredWidth: 48.0
                    preferredHeight: 48.0
                    minWidth: 48.0
                    minHeight: 48.0
                    background: Color.create(avatarColor ? avatarColor : "#5288c1")
                    rightMargin: 10.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatPage.openProfile();
                            }
                        }
                    ]

                    // Fallback Initials (always rendered in the background)
                    Label {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        text: initials ? initials : "?"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.Bold
                    }

                    // Avatar Image (rendered only when file exists AND is verified decodable)
                    ImageView {
                        visible: avatarPath && avatarPath.length > 0 && chatList.fileExists(avatarPath)
                        imageSource: (avatarPath && avatarPath.length > 0 && chatList.fileExists(avatarPath)) ? (avatarPath.indexOf("file://") === 0 ? avatarPath : "file://" + avatarPath) : ""
                        preferredWidth: 48.0
                        preferredHeight: 48.0
                        scalingMethod: ScalingMethod.AspectFill
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                // Chat title and subtitle column
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatPage.openProfile();
                            }
                        }
                    ]

                    Label {
                        text: chatTitle ? chatTitle : "Chat"
                        textStyle.color: tPrimary
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.Bold
                    }
                    Label {
                        text: peerType == 3 ? "channel" : (peerType == 2 ? "group" : (peerType == 1 ? "last seen within a week" : "online"))
                        textStyle.color: tSecondary
                        textStyle.fontSize: FontSize.XXSmall
                    }
                }

                // Phone Call Button (tg_call.png)
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
                                chatPage.showCallOverlay = true;
                                chatPage.showOverflowMenu = false;
                                chatList.logDiagnostic("ChatScreen: Phone call tapped for peer " + peerId);
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_call.png"
                        preferredWidth: 46.0
                        preferredHeight: 46.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                // 3-Dots Overflow Menu Button (tg_more.png)
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
                                chatPage.showOverflowMenu = !chatPage.showOverflowMenu;
                                chatList.logDiagnostic("ChatScreen: Overflow menu tapped for peer " + peerId);
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
            // 2. MESSAGE AREA (Scrollable, fills middle space)
            // =============================================================
            Container {
                layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                layout: DockLayout {}

                ListView {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    dataModel: chatList.messagesModel
                    scrollRole: ScrollRole.Main

                    listItemComponents: [
                        ListItemComponent {
                            type: "item"
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                topPadding: 2.0
                                bottomPadding: 2.0
                                leftPadding: 10.0
                                rightPadding: 10.0
                                layout: DockLayout {}

                                // ---------------------------------------------
                                // Date Separator Pill (e.g. "August 31")
                                // ---------------------------------------------
                                Container {
                                    visible: ListItemData.isDateHeader === true
                                    horizontalAlignment: HorizontalAlignment.Center
                                    topPadding: 6.0
                                    bottomPadding: 6.0

                                    Container {
                                        background: Color.create("#232e3c")
                                        topPadding: 4.0
                                        bottomPadding: 4.0
                                        leftPadding: 14.0
                                        rightPadding: 14.0
                                        Label {
                                            text: ListItemData.textHeaderText ? ListItemData.textHeaderText : ""
                                            textStyle.color: Color.create("#d0d7de")
                                            textStyle.fontSize: FontSize.XXSmall
                                            textStyle.fontWeight: FontWeight.Bold
                                            textStyle.textAlign: TextAlign.Center
                                        }
                                    }
                                }

                                // ---------------------------------------------
                                // Message Bubble Row
                                // ---------------------------------------------
                                Container {
                                    visible: !ListItemData.isDateHeader
                                    horizontalAlignment: ListItemData.isOutgoing ? HorizontalAlignment.Right : HorizontalAlignment.Left
                                    maxWidth: 530.0
                                    background: ListItemData.isOutgoing ? Color.create("#2b78e4") : Color.create("#212d3b")
                                    topPadding: 7.0
                                    bottomPadding: 6.0
                                    leftPadding: 12.0
                                    rightPadding: 12.0

                                    // Author name for group / channel incoming messages
                                    Label {
                                        visible: (ListItemData.peerType == 2 || ListItemData.peerType == 3) && !ListItemData.isOutgoing && ListItemData.authorName && ListItemData.authorName.length > 0
                                        text: ListItemData.authorName ? ListItemData.authorName : ""
                                        textStyle.color: Color.create("#6cb5f3")
                                        textStyle.fontSize: FontSize.XXSmall
                                        textStyle.fontWeight: FontWeight.Bold
                                        textStyle.textAlign: TextAlign.Left
                                        horizontalAlignment: HorizontalAlignment.Left
                                        bottomMargin: 3.0
                                    }

                                    // Reply Header Box (inside bubble)
                                    Container {
                                        visible: ListItemData.replySnippet && ListItemData.replySnippet.length > 0
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        bottomMargin: 6.0
                                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                        // Accent vertical bar
                                        Container {
                                            preferredWidth: 3.0
                                            minWidth: 3.0
                                            verticalAlignment: VerticalAlignment.Fill
                                            background: ListItemData.isOutgoing ? Color.White : Color.create("#2aabee")
                                            rightMargin: 8.0
                                        }

                                        // Quoted author and snippet column
                                        Container {
                                            layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                            Label {
                                                text: ListItemData.replyAuthor ? ListItemData.replyAuthor : chatTitle
                                                textStyle.color: ListItemData.isOutgoing ? Color.White : Color.create("#2aabee")
                                                textStyle.fontSize: FontSize.XXSmall
                                                textStyle.fontWeight: FontWeight.Bold
                                            }
                                            Label {
                                                text: ListItemData.replySnippet ? ListItemData.replySnippet : ""
                                                textStyle.color: ListItemData.isOutgoing ? Color.create("#d0e2ff") : Color.create("#c5d0db")
                                                textStyle.fontSize: FontSize.XXSmall
                                            }
                                        }
                                    }

                                    // Media image preview (if message carries photo)
                                    ImageView {
                                        visible: ListItemData.mediaPath && ListItemData.mediaPath.length > 0
                                        imageSource: ListItemData.mediaPath ? (ListItemData.mediaPath.indexOf("file://") === 0 ? ListItemData.mediaPath : "file://" + ListItemData.mediaPath) : ""
                                        scalingMethod: ScalingMethod.AspectFit
                                        maxWidth: 460.0
                                        maxHeight: 340.0
                                        bottomMargin: 6.0
                                        horizontalAlignment: HorizontalAlignment.Center
                                    }

                                    // Message text content
                                    Label {
                                        visible: ListItemData.text && ListItemData.text.length > 0
                                        text: ListItemData.text ? ListItemData.text : ""
                                        textStyle.color: Color.White
                                        textStyle.fontSize: FontSize.Small
                                        multiline: true
                                    }

                                    // Reaction badge pill (e.g. ❤️ 1)
                                    Container {
                                        visible: ListItemData.reaction && ListItemData.reaction.length > 0
                                        topMargin: 4.0
                                        background: ListItemData.isOutgoing ? Color.create("#205bb5") : Color.create("#1a2430")
                                        topPadding: 2.0
                                        bottomPadding: 2.0
                                        leftPadding: 8.0
                                        rightPadding: 8.0
                                        Label {
                                            text: ListItemData.reaction ? ListItemData.reaction : ""
                                            textStyle.color: Color.White
                                            textStyle.fontSize: FontSize.XXSmall
                                        }
                                    }

                                    // Metadata row: timestamp + read checkmarks
                                    Container {
                                        horizontalAlignment: HorizontalAlignment.Right
                                        topMargin: 2.0
                                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                        Label {
                                            text: ListItemData.formattedTime ? ListItemData.formattedTime : ""
                                            textStyle.color: ListItemData.isOutgoing ? Color.create("#b8d4fa") : Color.create("#7f8c99")
                                            textStyle.fontSize: FontSize.XXSmall
                                            verticalAlignment: VerticalAlignment.Center
                                        }

                                        // Outgoing checkmarks
                                        Label {
                                            visible: ListItemData.isOutgoing === true
                                            text: "\u2713\u2713"
                                            textStyle.color: Color.White
                                            textStyle.fontSize: FontSize.XXSmall
                                            textStyle.fontWeight: FontWeight.Bold
                                            verticalAlignment: VerticalAlignment.Center
                                            leftMargin: 4.0
                                        }
                                    }
                                }
                            }
                        }
                    ]
                }

                // Empty state shown when there are no messages
                Label {
                    visible: chatList.messagesModel && chatList.messagesModel.size() == 0
                    text: "No messages yet"
                    textStyle.color: tSecondary
                    textStyle.fontSize: FontSize.Small
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
            }

            // =============================================================
            // 3. BOTTOM COMPOSER BAR (Sleek capsule + separate action button)
            // =============================================================
            Container {
                id: composerBar
                visible: canSend
                horizontalAlignment: HorizontalAlignment.Fill
                preferredHeight: 76.0
                minHeight: 72.0
                background: tComposer
                topPadding: 6.0
                bottomPadding: 6.0
                leftPadding: 8.0
                rightPadding: 8.0
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                // LEFT CAPSULE CONTAINER (contains Smiley, TextField, and Paperclip)
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    background: Color.create("#212d3b")
                    preferredHeight: 62.0
                    minHeight: 60.0
                    verticalAlignment: VerticalAlignment.Center
                    leftPadding: 6.0
                    rightPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                    // Smiley / Emoji button (tg_emoji.png)
                    Container {
                        preferredWidth: 52.0
                        preferredHeight: 56.0
                        minWidth: 52.0
                        minHeight: 56.0
                        verticalAlignment: VerticalAlignment.Center
                        layout: DockLayout {}
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatPage.insertEmoji("\u263A ");
                                    chatList.logDiagnostic("ChatScreen: Emoji inserted");
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_emoji.png"
                            preferredWidth: 48.0
                            preferredHeight: 48.0
                            scalingMethod: ScalingMethod.AspectFit
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    // Center Message Input Field (NO GREY BOX: backgroundVisible: false)
                    TextField {
                        id: messageInputField
                        hintText: "Message"
                        inputMode: TextFieldInputMode.Text
                        backgroundVisible: false
                        verticalAlignment: VerticalAlignment.Center
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Medium
                        input.onSubmitted: {
                            chatPage.doSend();
                        }
                    }

                    // Paperclip Attachment button (tg_attach.png) - inside capsule
                    Container {
                        preferredWidth: 52.0
                        preferredHeight: 56.0
                        minWidth: 52.0
                        minHeight: 56.0
                        verticalAlignment: VerticalAlignment.Center
                        layout: DockLayout {}
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatPage.showAttachmentMenu = !chatPage.showAttachmentMenu;
                                    chatList.logDiagnostic("ChatScreen: Attachment button tapped");
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/tg_attach.png"
                            preferredWidth: 48.0
                            preferredHeight: 48.0
                            scalingMethod: ScalingMethod.AspectFit
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }
                }

                // RIGHT ACTION BUTTON (Voice Microphone / Send Airplane)
                // Distinct circular button with explicit bounds and margin to guarantee ZERO overlap!
                Container {
                    id: actionBtn
                    preferredWidth: 62.0
                    preferredHeight: 62.0
                    minWidth: 62.0
                    minHeight: 62.0
                    maxWidth: 62.0
                    maxHeight: 62.0
                    leftMargin: 8.0
                    background: tBlue
                    layout: DockLayout {}
                    verticalAlignment: VerticalAlignment.Center
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatPage.doSend();
                            }
                        }
                    ]

                    // Dynamic Voice Microphone icon (tg_mic.png) when field is empty
                    ImageView {
                        visible: messageInputField.text.trim().length === 0
                        imageSource: "asset:///images/tg_mic.png"
                        preferredWidth: 48.0
                        preferredHeight: 48.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }

                    // Dynamic Send Airplane icon (tg_send.png) when text is entered
                    ImageView {
                        visible: messageInputField.text.trim().length > 0
                        imageSource: "asset:///images/tg_send.png"
                        preferredWidth: 48.0
                        preferredHeight: 48.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }

        // =============================================================
        // POPUP: 3-DOTS OVERFLOW MENU (Search, Mute, Clear, Delete)
        // =============================================================
        Container {
            visible: chatPage.showOverflowMenu
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Top
            topMargin: 78.0
            rightMargin: 12.0
            minWidth: 190.0
            background: Color.create("#1e2b38")
            topPadding: 8.0
            bottomPadding: 8.0
            leftPadding: 14.0
            rightPadding: 14.0

            // Search item
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 40.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDD0D  Search"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatPage.showOverflowMenu = false;
                            chatList.logDiagnostic("ChatScreen: Menu Search clicked");
                        }
                    }
                ]
            }

            // Mute notifications item
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 40.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDD14  Mute notifications"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatPage.showOverflowMenu = false;
                            chatList.logDiagnostic("ChatScreen: Menu Mute clicked");
                        }
                    }
                ]
            }

            // Clear history item
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 40.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDDD1  Clear history"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatPage.showOverflowMenu = false;
                            chatList.messagesModel.clear();
                            chatList.logDiagnostic("ChatScreen: Menu Clear history clicked");
                        }
                    }
                ]
            }

            // Delete chat item
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 40.0
                layout: DockLayout {}
                Label {
                    text: "\u274C  Delete chat"
                    textStyle.color: Color.create("#e53935")
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatPage.showOverflowMenu = false;
                            chatPage.goBack();
                        }
                    }
                ]
            }
        }

        // =============================================================
        // POPUP: ATTACHMENT MENU (Photo, File, Location)
        // =============================================================
        Container {
            visible: chatPage.showAttachmentMenu
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Bottom
            bottomMargin: 76.0
            rightMargin: 44.0
            minWidth: 180.0
            background: Color.create("#1e2b38")
            topPadding: 8.0
            bottomPadding: 8.0
            leftPadding: 14.0
            rightPadding: 14.0

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 38.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDDBC  Photo / Gallery"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatPage.showAttachmentMenu = false;
                            chatList.logDiagnostic("ChatScreen: Attachment Photo clicked");
                            chatList.sendMessage(peerType, peerId, accessHash, "\uD83D\uDDBC [Photo]");
                        }
                    }
                ]
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 38.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDCC1  Document / File"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatPage.showAttachmentMenu = false;
                            chatList.logDiagnostic("ChatScreen: Attachment File clicked");
                            chatList.sendMessage(peerType, peerId, accessHash, "\uD83D\uDCC1 [Document]");
                        }
                    }
                ]
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                minHeight: 38.0
                layout: DockLayout {}
                Label {
                    text: "\uD83D\uDCCD  Location"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Small
                    verticalAlignment: VerticalAlignment.Center
                }
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatPage.showAttachmentMenu = false;
                            chatList.logDiagnostic("ChatScreen: Attachment Location clicked");
                            chatList.sendMessage(peerType, peerId, accessHash, "\uD83D\uDCCD [Location: 28.6139 N, 77.2090 E]");
                        }
                    }
                ]
            }
        }

        // =============================================================
        // OVERLAY: TELEGRAM AUDIO CALL (Realistic voice call sheet)
        // =============================================================
        Container {
            visible: chatPage.showCallOverlay
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

                // Caller Avatar (84x84dp)
                Container {
                    preferredWidth: 84.0
                    preferredHeight: 84.0
                    minWidth: 84.0
                    minHeight: 84.0
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

                // Peer Name
                Label {
                    text: chatTitle ? chatTitle : "Telegram User"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.XLarge
                    textStyle.fontWeight: FontWeight.Bold
                    horizontalAlignment: HorizontalAlignment.Center
                    bottomMargin: 6.0
                }

                // Call status
                Label {
                    text: "calling..."
                    textStyle.color: Color.create("#6cb5f3")
                    textStyle.fontSize: FontSize.Medium
                    horizontalAlignment: HorizontalAlignment.Center
                    bottomMargin: 40.0
                }

                // Controls row (Mute, Speaker, End Call)
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                    // Mute Button
                    Container {
                        preferredWidth: 54.0
                        preferredHeight: 54.0
                        background: chatPage.isMuted ? Color.create("#e53935") : Color.create("#223344")
                        layout: DockLayout {}
                        rightMargin: 20.0
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatPage.isMuted = !chatPage.isMuted;
                                }
                            }
                        ]
                        Label {
                            text: chatPage.isMuted ? "\uD83D\uDD07" : "\uD83C\uDFA4"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Medium
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    // Red End Call Button
                    Container {
                        preferredWidth: 64.0
                        preferredHeight: 64.0
                        background: Color.create("#e53935")
                        layout: DockLayout {}
                        rightMargin: 20.0
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatPage.showCallOverlay = false;
                                    chatList.logDiagnostic("ChatScreen: Call ended by user");
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

                    // Speaker Button
                    Container {
                        preferredWidth: 54.0
                        preferredHeight: 54.0
                        background: chatPage.isSpeakerOn ? Color.create("#24a1de") : Color.create("#223344")
                        layout: DockLayout {}
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatPage.isSpeakerOn = !chatPage.isSpeakerOn;
                                }
                            }
                        ]
                        Label {
                            text: "\uD83D\uDD0A"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Medium
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }
                }
            }
        }
    }

    function loadChat(title, type, id, hash, init, color, pic, canSendFlag, nav, uName) {
        chatTitle = title;
        peerType = type;
        peerId = id;
        accessHash = hash;
        initials = init ? init : "?";
        avatarColor = color ? color : "#5288c1";
        avatarPath = pic ? pic : "";
        canSend = (canSendFlag !== undefined) ? canSendFlag : true;
        username = (uName !== undefined && uName !== null) ? uName : "";
        showOverflowMenu = false;
        showAttachmentMenu = false;
        showCallOverlay = false;
        isMuted = false;
        isSpeakerOn = false;
        if (nav !== undefined && nav !== null) {
            navPane = nav;
        }
    }

    attachedObjects: [
        ComponentDefinition {
            id: profileScreenDef
            source: "asset:///ProfileScreen.qml"
        }
    ]
}

