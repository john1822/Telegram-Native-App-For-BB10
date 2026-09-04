import bb.cascades 1.4

// =============================================================================
// TelegramBB10 — Custom Chat Screen (fully custom, no native chrome)
// -----------------------------------------------------------------------------
// Replaces the native TitleBar / back button / list with fully custom QML.
//   * Custom header with a custom back button (native back arrow hidden).
//   * Telegram-style message bubbles with date/time.
//   * Custom composer with a send button.
// All colors follow the 2026 Telegram dark design tokens.
// =============================================================================

Page {
    id: chatPage

    // Peer metadata (populated by loadChat())
    property string chatTitle: ""
    property string initials: "?"
    property string avatarColor: "#5288c1"
    property int peerType: 0
    property string peerId: ""
    property string accessHash: ""
    property string avatarPath: ""
    property bool canSend: true

    // Design tokens (kept local so ListItemComponent can use literals)
    property color tBg:         Color.create("#0e1621")
    property color tPanel:      Color.create("#17212b")
    property color tBubbleIn:   Color.create("#182533")
    property color tBubbleOut:  Color.create("#2b5278")
    property color tBlue:       Color.create("#2aabee")
    property color tPrimary:    Color.create("#ffffff")
    property color tSecondary:  Color.create("#7f8b99")
    property color tAuthor:     Color.create("#6cb5f3")

    content: Container {
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        background: chatPage.tBg

        Container {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            // =============================================================
            // CUSTOM HEADER with custom back button
            // =============================================================
            Container {
                id: headerBar
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Top
                minHeight: 56.0
                background: tPanel
                topPadding: 6.0
                bottomPadding: 6.0
                leftPadding: 4.0
                rightPadding: 8.0

                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                // Custom back button
                Container {
                    preferredWidth: 44.0
                    preferredHeight: 44.0
                    minWidth: 44.0
                    minHeight: 44.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                if (typeof rootNavPane !== "undefined") {
                                    rootNavPane.pop();
                                }
                            }
                        }
                    ]
                    Label {
                        text: "\u2039"
                        textStyle.color: tBlue
                        textStyle.fontSize: FontSize.XXLarge
                        textStyle.fontWeight: FontWeight.Bold
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                // Avatar
                Container {
                    preferredWidth: 40.0
                    preferredHeight: 40.0
                    minWidth: 40.0
                    minHeight: 40.0
                    background: Color.create(avatarColor ? avatarColor : "#5288c1")
                    rightMargin: 10.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}

                    Label {
                        visible: !avatarPath || avatarPath.length == 0
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        text: initials ? initials : "?"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Small
                        textStyle.fontWeight: FontWeight.Bold
                    }
                    ImageView {
                        visible: avatarPath && avatarPath.length > 0
                        imageSource: avatarPath ? (avatarPath.indexOf("file://") === 0 ? avatarPath : "file://" + avatarPath) : ""
                        preferredWidth: 40.0
                        preferredHeight: 40.0
                        scalingMethod: ScalingMethod.AspectFill
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                // Chat info
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                    Label {
                        text: chatTitle ? chatTitle : "Chat"
                        textStyle.color: tPrimary
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.Bold
                    }
                    Label {
                        text: peerType == 3 ? "Channel" : (peerType == 2 ? "Group" : (peerType == 1 ? "Private" : "Chat"))
                        textStyle.color: tSecondary
                        textStyle.fontSize: FontSize.XXSmall
                    }
                }
            }

            // =============================================================
            // MESSAGE LIST - custom Telegram bubbles
            // =============================================================
            ListView {
                layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                topMargin: headerBar.minHeight
                dataModel: chatList.messagesModel
                scrollRole: ScrollRole.Main

                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        Container {
                            horizontalAlignment: HorizontalAlignment.Fill
                            topPadding: 3.0
                            bottomPadding: 3.0
                            leftPadding: 10.0
                            rightPadding: 10.0

                            layout: DockLayout {}

                            // Date separator
                            Container {
                                visible: ListItemData.isDateHeader
                                horizontalAlignment: HorizontalAlignment.Center
                                topPadding: 8.0
                                bottomPadding: 8.0
                                Label {
                                    text: ListItemData.textHeaderText ? ListItemData.textHeaderText : ""
                                    textStyle.color: Color.create("#7f8b99")
                                    textStyle.fontSize: FontSize.XXSmall
                                    textStyle.textAlign: TextAlign.Center
                                }
                            }

                            // Message bubble row
                            Container {
                                horizontalAlignment: ListItemData.isOutgoing ? HorizontalAlignment.Right : HorizontalAlignment.Left
                                maxWidth: 500.0
                                background: ListItemData.isOutgoing ? Color.create("#2b5278") : Color.create("#182533")
                                topPadding: 6.0
                                bottomPadding: 6.0
                                leftPadding: 12.0
                                rightPadding: 12.0

                                // Author name for group/channel messages
                                Label {
                                    visible: (ListItemData.peerType == 2 || ListItemData.peerType == 3) && !ListItemData.isOutgoing && ListItemData.authorName && ListItemData.authorName.length > 0
                                    text: ListItemData.authorName ? ListItemData.authorName : ""
                                    textStyle.color: Color.create("#6cb5f3")
                                    textStyle.fontSize: FontSize.XSmall
                                    textStyle.fontWeight: FontWeight.Bold
                                    textStyle.textAlign: TextAlign.Left
                                    horizontalAlignment: HorizontalAlignment.Left
                                }

                                // Media (photo)
                                ImageView {
                                    visible: ListItemData.mediaPath && ListItemData.mediaPath.length > 0
                                    imageSource: ListItemData.mediaPath ? (ListItemData.mediaPath.indexOf("file://") === 0 ? ListItemData.mediaPath : "file://" + ListItemData.mediaPath) : ""
                                    scalingMethod: ScalingMethod.AspectFit
                                    maxWidth: 460.0
                                    maxHeight: 340.0
                                    bottomMargin: 6.0
                                    horizontalAlignment: HorizontalAlignment.Center
                                }

                                // Message text
                                Label {
                                    visible: ListItemData.text && ListItemData.text.length > 0
                                    text: ListItemData.text ? ListItemData.text : ""
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.Small
                                    multiline: true
                                }

                                // Time + read indicator row
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Right
                                    topMargin: 2.0
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                    Container {
                                        minWidth: 4.0
                                        minHeight: 4.0
                                        preferredWidth: 4.0
                                        preferredHeight: 4.0
                                        background: ListItemData.isOutgoing ? (ListItemData.mediaPath && ListItemData.mediaPath.length > 0 ? Color.White : Color.create("#6e7b8c")) : Color.create("#5b6572")
                                        verticalAlignment: VerticalAlignment.Center
                                        rightMargin: 4.0
                                    }
                                    Label {
                                        text: ListItemData.formattedTime ? ListItemData.formattedTime : ""
                                        textStyle.color: Color.create("#6e7b8c")
                                        textStyle.fontSize: FontSize.XXSmall
                                    }
                                }
                            }
                        }
                    }
                ]
            }

            // =============================================================
            // CUSTOM COMPOSER with send button
            // =============================================================
            Container {
                id: composerBar
                visible: canSend
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Bottom
                background: tPanel
                topPadding: 8.0
                bottomPadding: 8.0
                leftPadding: 10.0
                rightPadding: 10.0

                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                TextField {
                    id: messageInputField
                    hintText: "Message"
                    inputMode: TextFieldInputMode.Text
                    verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                }

                // Send button
                Container {
                    preferredWidth: 48.0
                    preferredHeight: 44.0
                    minWidth: 48.0
                    minHeight: 44.0
                    background: messageInputField.text.trim().length > 0 ? Color.create("#2aabee") : Color.create("#17212b")
                    layout: DockLayout {}
                    verticalAlignment: VerticalAlignment.Center
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                if (messageInputField.text.trim().length > 0) {
                                    chatList.sendMessage(peerType, peerId, accessHash, messageInputField.text);
                                    messageInputField.text = "";
                                }
                            }
                        }
                    ]
                    Label {
                        text: "\u27A4"
                        textStyle.color: messageInputField.text.trim().length > 0 ? Color.create("#ffffff") : Color.create("#7f8b99")
                        textStyle.fontSize: FontSize.XXLarge
                        textStyle.fontWeight: FontWeight.Bold
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
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
    }

    function loadChat(title, type, id, hash, init, color, pic, canSendFlag) {
        chatTitle = title;
        peerType = type;
        peerId = id;
        accessHash = hash;
        initials = init ? init : "?";
        avatarColor = color ? color : "#5288c1";
        avatarPath = pic ? pic : "";
        canSend = (canSendFlag !== undefined) ? canSendFlag : true;
    }
}
