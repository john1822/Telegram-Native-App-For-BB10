import bb.cascades 1.4

Page {
    id: chatPage

    property string chatTitle: ""
    property string initials: "?"
    property string avatarColor: "#5288c1"
    property int peerType: 0
    property string peerId: ""
    property string accessHash: ""

    titleBar: TitleBar {
        id: chatTitleBar
        title: chatTitle
        visibility: ChromeVisibility.Visible
    }

    Container {
        layout: DockLayout {}
        background: Color.create("#0e1621") // 2026 Telegram Deep Dark Background

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            // Subtitle header / info bar
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#17212b")
                topPadding: 8.0
                bottomPadding: 8.0
                leftPadding: 14.0
                rightPadding: 14.0

                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                // Circular Avatar
                Container {
                    preferredWidth: 44.0
                    preferredHeight: 44.0
                    minWidth: 44.0
                    minHeight: 44.0
                    background: Color.create(avatarColor ? avatarColor : "#5288c1")
                    rightMargin: 12.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}

                    Label {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        text: initials ? initials : "?"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Small
                        textStyle.fontWeight: FontWeight.Bold
                    }
                }

                // Chat Info
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1.0
                    }

                    Label {
                        text: chatTitle ? chatTitle : "Chat"
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.Bold
                    }

                    Label {
                        text: peerType == 3 ? "Channel" : (peerType == 2 ? "Group Chat" : "Private Chat")
                        textStyle.color: Color.create("#708499")
                        textStyle.fontSize: FontSize.XXSmall
                    }
                }
            }

            // Message History Area
            ListView {
                id: messagesListView
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1.0
                }
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                dataModel: chatList.messagesModel

                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        Container {
                            horizontalAlignment: HorizontalAlignment.Fill
                            topPadding: 4.0
                            bottomPadding: 4.0
                            leftPadding: 10.0
                            rightPadding: 10.0

                            Container {
                                horizontalAlignment: ListItemData.isOutgoing ? HorizontalAlignment.Right : HorizontalAlignment.Left
                                maxWidth: 560.0
                                background: ListItemData.isOutgoing ? Color.create("#2b5278") : Color.create("#182533")
                                topPadding: 8.0
                                bottomPadding: 8.0
                                leftPadding: 12.0
                                rightPadding: 12.0

                                Label {
                                    text: ListItemData.text ? ListItemData.text : ""
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.Small
                                    multiline: true
                                }

                                Label {
                                    horizontalAlignment: HorizontalAlignment.Right
                                    text: ListItemData.formattedTime ? ListItemData.formattedTime : ""
                                    textStyle.color: Color.create("#708499")
                                    textStyle.fontSize: FontSize.XXSmall
                                    topMargin: 2.0
                                }
                            }
                        }
                    }
                ]
            }

            // Bottom Input Dock
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#17212b")
                topPadding: 8.0
                bottomPadding: 8.0
                leftPadding: 10.0
                rightPadding: 10.0

                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                TextField {
                    id: messageInputField
                    hintText: "Message"
                    inputMode: TextFieldInputMode.Text
                    verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1.0
                    }
                }

                Button {
                    text: "Send"
                    preferredWidth: 120.0
                    verticalAlignment: VerticalAlignment.Center
                    enabled: messageInputField.text.trim().length > 0
                    onClicked: {
                        chatList.sendMessage(peerType, peerId, accessHash, messageInputField.text);
                        messageInputField.text = "";
                    }
                }
            }
        }
    }

    function loadChat(title, type, id, hash, init, color) {
        chatTitle = title;
        peerType = type;
        peerId = id;
        accessHash = hash;
        initials = init ? init : "?";
        avatarColor = color ? color : "#5288c1";
        chatList.loadHistory(peerType, peerId, accessHash);
    }
}
