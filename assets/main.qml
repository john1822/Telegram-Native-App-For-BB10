import bb.cascades 1.4

TabbedPane {
    id: rootTabbedPane
    showTabsOnActionBar: true

    onCreationCompleted: {
        if (auth.authState == 5) {
            rootTabbedPane.activeTab = chatsTab;
        } else {
            rootTabbedPane.activeTab = accountTab;
        }
    }

    // =========================================================================
    // TAB 1: CHATS & DIALOGS
    // =========================================================================
    Tab {
        id: chatsTab
        title: "Chats"
        description: "Messages & Channels"

        Page {
            titleBar: TitleBar {
                title: "Chats"
                visibility: ChromeVisibility.Visible
            }

            actions: [
                ActionItem {
                    title: "Sync"
                    ActionBar.placement: ActionBarPlacement.OnBar
                    onTriggered: {
                        chatList.refreshDialogs();
                    }
                }
            ]

            Container {
                layout: DockLayout {}
                background: Color.create("#0e1621") // 2026 Telegram Deep Dark Background

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    topPadding: 8.0
                    leftPadding: 10.0
                    rightPadding: 10.0
                    bottomPadding: 8.0

                    // Search Input Bar
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        bottomMargin: 8.0

                        TextField {
                            id: searchField
                            hintText: "Search chats and contacts..."
                            inputMode: TextFieldInputMode.Text
                            onTextChanging: {
                                chatList.searchQuery = text;
                            }
                        }
                    }

                    // Empty / Loading / Login prompt State
                    Container {
                        visible: chatList.dialogsCount == 0 || auth.authState != 5
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        topPadding: 40.0
                        bottomPadding: 40.0

                        Label {
                            visible: auth.authState != 5
                            text: "Please sign in on the Account tab to view your chats."
                            textStyle.color: Color.create("#708499")
                            textStyle.fontSize: FontSize.Medium
                            multiline: true
                            horizontalAlignment: HorizontalAlignment.Center
                        }

                        Label {
                            visible: auth.authState == 5 && chatList.isLoading
                            text: "Syncing your Telegram chats..."
                            textStyle.color: Color.create("#5288c1")
                            textStyle.fontSize: FontSize.Medium
                            horizontalAlignment: HorizontalAlignment.Center
                        }

                        Label {
                            visible: auth.authState == 5 && !chatList.isLoading && chatList.dialogsCount == 0
                            text: "No active chats found.\nTap Sync to refresh with Telegram."
                            textStyle.color: Color.create("#708499")
                            textStyle.fontSize: FontSize.Medium
                            multiline: true
                            horizontalAlignment: HorizontalAlignment.Center
                        }

                        Button {
                            visible: auth.authState == 5 && !chatList.isLoading && chatList.dialogsCount == 0
                            text: "Sync Chats"
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 16.0
                            onClicked: {
                                chatList.refreshDialogs();
                            }
                        }
                    }

                    // Native Cascades ListView for Dialogs
                    ListView {
                        visible: auth.authState == 5 && chatList.dialogsCount > 0
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        dataModel: chatList.model

                        listItemComponents: [
                            ListItemComponent {
                                type: "item"
                                Container {
                                    id: chatItemRoot
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    background: Color.create("#17212b")
                                    topPadding: 12.0
                                    bottomPadding: 12.0
                                    leftPadding: 14.0
                                    rightPadding: 14.0
                                    bottomMargin: 4.0

                                    layout: StackLayout {
                                        orientation: LayoutOrientation.LeftToRight
                                    }

                                    // Vibrant Circular Avatar with Initials
                                    Container {
                                        verticalAlignment: VerticalAlignment.Center
                                        preferredWidth: 64.0
                                        preferredHeight: 64.0
                                        minWidth: 64.0
                                        minHeight: 64.0
                                        background: Color.create(ListItemData.avatarColor ? ListItemData.avatarColor : "#5288c1")
                                        rightMargin: 12.0
                                        layout: DockLayout {}

                                        Label {
                                            horizontalAlignment: HorizontalAlignment.Center
                                            verticalAlignment: VerticalAlignment.Center
                                            text: ListItemData.initials ? ListItemData.initials : "?"
                                            textStyle.color: Color.White
                                            textStyle.fontSize: FontSize.Medium
                                            textStyle.fontWeight: FontWeight.Bold
                                        }
                                    }

                                    // Chat Content Column
                                    Container {
                                        verticalAlignment: VerticalAlignment.Center
                                        layoutProperties: StackLayoutProperties {
                                            spaceQuota: 1.0
                                        }

                                        // Title & Time Row
                                        Container {
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            layout: StackLayout {
                                                orientation: LayoutOrientation.LeftToRight
                                            }

                                            Label {
                                                text: ListItemData.title ? ListItemData.title : ""
                                                textStyle.color: Color.White
                                                textStyle.fontSize: FontSize.Small
                                                textStyle.fontWeight: FontWeight.Bold
                                                layoutProperties: StackLayoutProperties {
                                                    spaceQuota: 1.0
                                                }
                                            }

                                            Label {
                                                text: ListItemData.formattedTime ? ListItemData.formattedTime : ""
                                                textStyle.color: Color.create("#708499")
                                                textStyle.fontSize: FontSize.XXSmall
                                                verticalAlignment: VerticalAlignment.Center
                                            }
                                        }

                                        // Last Message & Badges Row
                                        Container {
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            topMargin: 2.0
                                            layout: StackLayout {
                                                orientation: LayoutOrientation.LeftToRight
                                            }

                                            Label {
                                                text: ListItemData.lastMessage ? ListItemData.lastMessage : ""
                                                textStyle.color: Color.create("#708499")
                                                textStyle.fontSize: FontSize.XSmall
                                                layoutProperties: StackLayoutProperties {
                                                    spaceQuota: 1.0
                                                }
                                            }

                                            // Unread Badge Pill
                                            Container {
                                                visible: ListItemData.unreadCount > 0
                                                verticalAlignment: VerticalAlignment.Center
                                                background: Color.create("#2ea043") // Telegram Green Pill
                                                topPadding: 2.0
                                                bottomPadding: 2.0
                                                leftPadding: 6.0
                                                rightPadding: 6.0

                                                Label {
                                                    text: "" + ListItemData.unreadCount
                                                    textStyle.color: Color.White
                                                    textStyle.fontSize: FontSize.XXSmall
                                                    textStyle.fontWeight: FontWeight.Bold
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        ]

                        onTriggered: {
                            chatList.selectDialog(indexPath);
                        }
                    }
                }
            }
        }
    }

    // =========================================================================
    // TAB 2: TELEGRAM AUTHENTICATION & PROFILE
    // =========================================================================
    Tab {
        id: accountTab
        title: "Account"
        description: "Login & Profile"

        Page {
            Container {
                layout: DockLayout {}
                background: Color.create("#0e1621") // 2026 Telegram Deep Dark Background

                ScrollView {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        topPadding: 16.0
                        bottomPadding: 20.0
                        leftPadding: 16.0
                        rightPadding: 16.0

                        // Header Bar
                        Container {
                            horizontalAlignment: HorizontalAlignment.Fill
                            background: Color.create("#17212b")
                            topPadding: 14.0
                            bottomPadding: 14.0
                            leftPadding: 16.0
                            rightPadding: 16.0
                            bottomMargin: 14.0

                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }

                            Container {
                                verticalAlignment: VerticalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1.0
                                }

                                Label {
                                    text: "Telegram"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.Large
                                    textStyle.fontWeight: FontWeight.Bold
                                }

                                Label {
                                    text: "Native MTProto 2.0 for BlackBerry 10"
                                    textStyle.color: Color.create("#708499")
                                    textStyle.fontSize: FontSize.XXSmall
                                }
                            }

                            Container {
                                verticalAlignment: VerticalAlignment.Center
                                background: diagnostic.isEncrypted ? Color.create("#2ea043") : (diagnostic.isConnected ? Color.create("#2481cc") : Color.create("#cf222e"))
                                topPadding: 4.0
                                bottomPadding: 4.0
                                leftPadding: 10.0
                                rightPadding: 10.0

                                Label {
                                    text: diagnostic.isEncrypted ? "ONLINE" : (diagnostic.isConnected ? "CONNECTING" : "OFFLINE")
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.XXSmall
                                    textStyle.fontWeight: FontWeight.Bold
                                }
                            }
                        }

                        // Status & Error Notice Banner
                        Container {
                            visible: auth.statusMessage.length > 0
                            horizontalAlignment: HorizontalAlignment.Fill
                            background: Color.create("#242f3d")
                            topPadding: 10.0
                            bottomPadding: 10.0
                            leftPadding: 14.0
                            rightPadding: 14.0
                            bottomMargin: 14.0

                            Label {
                                text: auth.statusMessage
                                textStyle.color: Color.create("#5288c1")
                                textStyle.fontSize: FontSize.Small
                                textStyle.fontWeight: FontWeight.Medium
                                multiline: true
                            }
                        }

                        // -----------------------------------------------------
                        // STATE 2: PHONE NUMBER INPUT
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 2 || auth.authState <= 1
                            horizontalAlignment: HorizontalAlignment.Fill
                            background: Color.create("#17212b")
                            topPadding: 18.0
                            bottomPadding: 18.0
                            leftPadding: 18.0
                            rightPadding: 18.0
                            bottomMargin: 14.0

                            Label {
                                text: "Sign in to Telegram"
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Medium
                                textStyle.fontWeight: FontWeight.Bold
                                bottomMargin: 6.0
                            }

                            Label {
                                text: "Please enter your phone number with your country code (e.g. +91XXXXXXXXXX):"
                                textStyle.color: Color.create("#708499")
                                textStyle.fontSize: FontSize.XSmall
                                multiline: true
                                bottomMargin: 12.0
                            }

                            TextField {
                                id: phoneInput
                                hintText: "+918950469287"
                                text: "+918950469287"
                                inputMode: TextFieldInputMode.PhoneNumber
                                bottomMargin: 14.0
                            }

                            Button {
                                text: auth.isBusy ? "Sending Code..." : "Next (Send Code)"
                                enabled: !auth.isBusy && diagnostic.isEncrypted
                                horizontalAlignment: HorizontalAlignment.Fill
                                bottomMargin: 10.0
                                onClicked: {
                                    auth.submitPhoneNumber(phoneInput.text);
                                }
                            }

                            Button {
                                text: "Login via QR Code"
                                enabled: !auth.isBusy && diagnostic.isEncrypted
                                horizontalAlignment: HorizontalAlignment.Fill
                                onClicked: {
                                    auth.requestQrLogin();
                                }
                            }
                        }

                        // -----------------------------------------------------
                        // STATE 6: QR CODE LOGIN
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 6
                            horizontalAlignment: HorizontalAlignment.Fill
                            background: Color.create("#17212b")
                            topPadding: 18.0
                            bottomPadding: 18.0
                            leftPadding: 18.0
                            rightPadding: 18.0
                            bottomMargin: 14.0

                            Label {
                                text: "Scan QR Code with Telegram"
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Medium
                                textStyle.fontWeight: FontWeight.Bold
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 6.0
                            }

                            Label {
                                text: "1. Open Telegram on your phone\n2. Go to Settings > Devices > Link Desktop\n3. Point camera at this screen:"
                                textStyle.color: Color.create("#708499")
                                textStyle.fontSize: FontSize.XSmall
                                multiline: true
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 14.0
                            }

                            // QR Image Display Container
                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                background: Color.White
                                topPadding: 10.0
                                bottomPadding: 10.0
                                leftPadding: 10.0
                                rightPadding: 10.0
                                bottomMargin: 16.0

                                ImageView {
                                    id: qrImage
                                    imageSource: auth.qrImagePath
                                    preferredWidth: 260.0
                                    preferredHeight: 260.0
                                    minWidth: 260.0
                                    minHeight: 260.0
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            Button {
                                text: "Cancel / Use Phone Number"
                                horizontalAlignment: HorizontalAlignment.Fill
                                onClicked: {
                                    auth.cancelQrLogin();
                                }
                            }
                        }

                        // -----------------------------------------------------
                        // STATE 3: VERIFICATION CODE INPUT
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 3
                            horizontalAlignment: HorizontalAlignment.Fill
                            background: Color.create("#17212b")
                            topPadding: 18.0
                            bottomPadding: 18.0
                            leftPadding: 18.0
                            rightPadding: 18.0
                            bottomMargin: 14.0

                            Label {
                                text: "Enter Verification Code"
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Medium
                                textStyle.fontWeight: FontWeight.Bold
                                bottomMargin: 6.0
                            }

                            Label {
                                text: "Telegram sent the code to your active Telegram App (Chat 777000 / Telegram Notifications) for " + auth.phoneNumber + ". Please check your phone:"
                                textStyle.color: Color.create("#5288c1")
                                textStyle.fontSize: FontSize.XSmall
                                multiline: true
                                bottomMargin: 12.0
                            }

                            TextField {
                                id: codeInput
                                hintText: "5-digit code"
                                inputMode: TextFieldInputMode.NumbersAndPunctuation
                                bottomMargin: 14.0
                            }

                            Button {
                                text: auth.isBusy ? "Verifying..." : "Verify Code"
                                enabled: !auth.isBusy
                                horizontalAlignment: HorizontalAlignment.Fill
                                bottomMargin: 10.0
                                onClicked: {
                                    auth.submitLoginCode(codeInput.text);
                                }
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }

                                Button {
                                    text: "Resend Code"
                                    enabled: !auth.isBusy
                                    layoutProperties: StackLayoutProperties {
                                        spaceQuota: 1.0
                                    }
                                    onClicked: {
                                        auth.resendCode();
                                    }
                                }

                                Button {
                                    text: "Change Phone"
                                    enabled: !auth.isBusy
                                    layoutProperties: StackLayoutProperties {
                                        spaceQuota: 1.0
                                    }
                                    onClicked: {
                                        auth.changePhoneNumber();
                                    }
                                }
                            }
                        }

                        // -----------------------------------------------------
                        // STATE 4: 2FA CLOUD PASSWORD INPUT
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 4
                            horizontalAlignment: HorizontalAlignment.Fill
                            background: Color.create("#17212b")
                            topPadding: 18.0
                            bottomPadding: 18.0
                            leftPadding: 18.0
                            rightPadding: 18.0
                            bottomMargin: 14.0

                            Label {
                                text: "Two-Step Verification"
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Medium
                                textStyle.fontWeight: FontWeight.Bold
                                bottomMargin: 6.0
                            }

                            Label {
                                text: "Your account is protected by an additional 2FA Cloud Password." + (auth.passwordHint.length > 0 ? (" (Hint: " + auth.passwordHint + ")") : "")
                                textStyle.color: Color.create("#708499")
                                textStyle.fontSize: FontSize.XSmall
                                multiline: true
                                bottomMargin: 12.0
                            }

                            TextField {
                                id: passwordInput
                                hintText: "Password"
                                inputMode: TextFieldInputMode.Password
                                bottomMargin: 14.0
                            }

                            Button {
                                text: auth.isBusy ? "Checking SRP-6A Proof..." : "Submit Password"
                                enabled: !auth.isBusy
                                horizontalAlignment: HorizontalAlignment.Fill
                                onClicked: {
                                    auth.submitPassword(passwordInput.text);
                                }
                            }
                        }

                        // -----------------------------------------------------
                        // STATE 5: LOGGED IN / USER PROFILE
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 5
                            horizontalAlignment: HorizontalAlignment.Fill
                            background: Color.create("#17212b")
                            topPadding: 18.0
                            bottomPadding: 18.0
                            leftPadding: 18.0
                            rightPadding: 18.0
                            bottomMargin: 14.0

                            Label {
                                text: "ACCOUNT PROFILE"
                                textStyle.color: Color.create("#5288c1")
                                textStyle.fontSize: FontSize.XXSmall
                                textStyle.fontWeight: FontWeight.Bold
                                bottomMargin: 8.0
                            }

                            Label {
                                text: auth.userName
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                            }

                            Label {
                                visible: auth.userHandle.length > 0
                                text: auth.userHandle
                                textStyle.color: Color.create("#5288c1")
                                textStyle.fontSize: FontSize.Small
                            }

                            Divider {
                                topMargin: 10.0
                                bottomMargin: 10.0
                            }

                            Label {
                                text: "User ID: " + auth.userId
                                textStyle.color: Color.create("#708499")
                                textStyle.fontSize: FontSize.XSmall
                            }

                            Label {
                                text: "Phone: " + auth.userPhone
                                textStyle.color: Color.create("#708499")
                                textStyle.fontSize: FontSize.XSmall
                                bottomMargin: 14.0
                            }

                            Button {
                                text: "View Chats"
                                horizontalAlignment: HorizontalAlignment.Fill
                                bottomMargin: 10.0
                                onClicked: {
                                    rootTabbedPane.activeTab = chatsTab;
                                }
                            }

                            Button {
                                text: "Log Out"
                                enabled: !auth.isBusy
                                horizontalAlignment: HorizontalAlignment.Fill
                                onClicked: {
                                    auth.logout();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // =========================================================================
    // TAB 2: DIAGNOSTICS & LIVE MTPROTO LOGS
    // =========================================================================
    Tab {
        title: "Diagnostics"
        description: "MTProto Logs"

        Page {
            Container {
                layout: DockLayout {}
                background: Color.create("#0e1621")

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    layout: StackLayout {}

                    // Status Bar
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        background: Color.create("#17212b")
                        topPadding: 12.0
                        bottomPadding: 12.0
                        leftPadding: 16.0
                        rightPadding: 16.0

                        Label {
                            text: diagnostic.statusText
                            textStyle.color: Color.create("#f5f5f5")
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Medium
                            multiline: true
                        }

                        Label {
                            text: "Auth Key: " + diagnostic.authKeyId + " | " + diagnostic.dcInfo
                            textStyle.color: Color.create("#708499")
                            textStyle.fontSize: FontSize.XXSmall
                        }
                    }

                    // Action Controls
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        topPadding: 8.0
                        bottomPadding: 8.0
                        leftPadding: 12.0
                        rightPadding: 12.0
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }

                        Button {
                            text: "Reconnect"
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 1.0
                            }
                            onClicked: {
                                diagnostic.reconnect();
                            }
                        }

                        Button {
                            text: "Test RPC"
                            enabled: diagnostic.isEncrypted
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 1.2
                            }
                            onClicked: {
                                diagnostic.sendTestRpc();
                            }
                        }

                        Button {
                            text: "Clear"
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 0.8
                            }
                            onClicked: {
                                diagnostic.clearLogs();
                            }
                        }
                    }

                    // Live Log Terminal Card
                    Container {
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1.0
                        }
                        horizontalAlignment: HorizontalAlignment.Fill
                        background: Color.create("#111a24")
                        topPadding: 8.0
                        bottomPadding: 8.0
                        leftPadding: 10.0
                        rightPadding: 10.0

                        TextArea {
                            text: diagnostic.logsText
                            editable: false
                            textStyle.color: Color.create("#00ff66")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontFamily: "Courier, monospace"
                            backgroundVisible: false
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                        }
                    }
                }
            }
        }
    }
}
