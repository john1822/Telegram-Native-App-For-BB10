import bb.cascades 1.4

// =============================================================================
// TelegramBB10 — 2026 Telegram "Liquid Glass" custom UI
// -----------------------------------------------------------------------------
// Fully custom-drawn shell (no TabbedPane, no TitleBar, no native tab bar).
//   * NavigationPane hosts the shell Page and the pushed ChatScreen.
//   * Custom application header.
//   * Custom bottom navigation rail (Chats / Account / Contacts / Settings / About).
//   * Custom flat list rows, custom fields & buttons (built from primitives).
// Controllers preserved unchanged: auth.*, chatList.*, diagnostic.*
// =============================================================================

NavigationPane {
    id: rootNavPane

    property color tBg:         Color.create("#0e1621")
    property color tPanel:      Color.create("#17212b")
    property color tBubbleIn:   Color.create("#182533")
    property color tBubbleOut:  Color.create("#2b5278")
    property color tBlue:       Color.create("#2aabee")
    property color tPrimary:    Color.create("#ffffff")
    property color tSecondary:  Color.create("#7f8b99")
    property color tGreen:      Color.create("#4dcd5e")
    property color tDanger:     Color.create("#e53935")
    property color tDivider:    Color.create("#0f1c26")

    // -------------------------------------------------------------------------
    // SHELL PAGE — the whole custom app UI
    // -------------------------------------------------------------------------
    Page {
        id: shellPage

        // Which of the screens is shown (0=Chats,1=Account,2=Contacts,3=Settings,4=About)
        property int activeSection: 0
        property string selectedCountry: "USA"
        property string selectedCountryCode: "+1"
        property int countryIndex: 0

        function cycleCountry() {
            var list = [
                { name: "USA", code: "+1" },
                { name: "India", code: "+91" },
                { name: "United Kingdom", code: "+44" },
                { name: "Canada", code: "+1" },
                { name: "Germany", code: "+49" },
                { name: "France", code: "+33" },
                { name: "Australia", code: "+61" },
                { name: "Russia", code: "+7" },
                { name: "Brazil", code: "+55" }
            ];
            countryIndex = (countryIndex + 1) % list.length;
            selectedCountry = list[countryIndex].name;
            selectedCountryCode = list[countryIndex].code;
            if (phoneInputField) {
                phoneInputField.text = selectedCountryCode + " ";
            }
        }

        content: Container {
            layout: DockLayout {}
            background: tBg

            // =================================================================
            // SCREEN: TELEGRAM MODERN LOGIN (Same to same pixel replication)
            // =================================================================
            Container {
                id: loginScreen
                visible: auth.authState != 5
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                background: Color.White

                ScrollView {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Center
                        leftPadding: 28.0
                        rightPadding: 28.0
                        topPadding: 32.0
                        bottomPadding: 32.0

                        // -----------------------------------------------------
                        // STATE: PHONE NUMBER INPUT (Default state 0, 1, 2)
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 2 || auth.authState <= 1
                            horizontalAlignment: HorizontalAlignment.Fill

                            // Telegram Circular Blue Badge Logo
                            ImageView {
                                imageSource: "asset:///images/telegram_logo.png"
                                preferredWidth: 106.0
                                preferredHeight: 106.0
                                minWidth: 106.0
                                minHeight: 106.0
                                maxWidth: 106.0
                                maxHeight: 106.0
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 22.0
                            }

                            // Heading
                            Label {
                                text: "Sign in to Telegram"
                                textStyle.color: Color.create("#222222")
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 8.0
                            }

                            // Subtitle
                            Label {
                                text: "Please confirm your country code\nand enter your phone number."
                                textStyle.color: Color.create("#707579")
                                textStyle.fontSize: FontSize.Small
                                textStyle.textAlign: TextAlign.Center
                                multiline: true
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 28.0
                            }

                            // Country Field (Outlined box with floating label)
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 60.0
                                bottomMargin: 16.0
                                background: Color.create("#dadce0")
                                topPadding: 1.5
                                bottomPadding: 1.5
                                leftPadding: 1.5
                                rightPadding: 1.5

                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    verticalAlignment: VerticalAlignment.Fill
                                    background: Color.White
                                    topPadding: 6.0
                                    bottomPadding: 6.0
                                    leftPadding: 14.0
                                    rightPadding: 14.0
                                    layout: DockLayout {}

                                    Container {
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        verticalAlignment: VerticalAlignment.Center
                                        Label {
                                            text: "Country"
                                            textStyle.color: Color.create("#707579")
                                            textStyle.fontSize: FontSize.XXSmall
                                            bottomMargin: 0.0
                                        }
                                        Label {
                                            id: countryNameLabel
                                            text: shellPage.selectedCountry
                                            textStyle.color: Color.create("#222222")
                                            textStyle.fontSize: FontSize.Medium
                                        }
                                    }

                                    Label {
                                        text: "\u25BE"
                                        textStyle.color: Color.create("#707579")
                                        textStyle.fontSize: FontSize.Large
                                        horizontalAlignment: HorizontalAlignment.Right
                                        verticalAlignment: VerticalAlignment.Center
                                    }

                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: {
                                                shellPage.cycleCountry();
                                            }
                                        }
                                    ]
                                }
                            }

                            // Phone Number Field (Outlined box with active blue border)
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 60.0
                                bottomMargin: 24.0
                                background: Color.create("#24A1DE")
                                topPadding: 1.5
                                bottomPadding: 1.5
                                leftPadding: 1.5
                                rightPadding: 1.5

                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    verticalAlignment: VerticalAlignment.Fill
                                    background: Color.White
                                    topPadding: 4.0
                                    bottomPadding: 4.0
                                    leftPadding: 14.0
                                    rightPadding: 14.0
                                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                                    Label {
                                        text: "Phone Number"
                                        textStyle.color: Color.create("#24A1DE")
                                        textStyle.fontSize: FontSize.XXSmall
                                        bottomMargin: 0.0
                                    }

                                    TextField {
                                        id: phoneInputField
                                        hintText: shellPage.selectedCountryCode + " --- --- ----"
                                        text: shellPage.selectedCountryCode + " "
                                        inputMode: TextFieldInputMode.PhoneNumber
                                        backgroundVisible: false
                                        textStyle.fontSize: FontSize.Medium
                                        textStyle.color: Color.create("#222222")
                                        onTextChanging: {
                                            var txt = text.trim();
                                            if (txt.indexOf("+1") === 0 && shellPage.selectedCountryCode !== "+1") {
                                                shellPage.selectedCountry = "USA";
                                                shellPage.selectedCountryCode = "+1";
                                            } else if (txt.indexOf("+91") === 0 && shellPage.selectedCountryCode !== "+91") {
                                                shellPage.selectedCountry = "India";
                                                shellPage.selectedCountryCode = "+91";
                                            } else if (txt.indexOf("+44") === 0 && shellPage.selectedCountryCode !== "+44") {
                                                shellPage.selectedCountry = "United Kingdom";
                                                shellPage.selectedCountryCode = "+44";
                                            } else if (txt.indexOf("+49") === 0 && shellPage.selectedCountryCode !== "+49") {
                                                shellPage.selectedCountry = "Germany";
                                                shellPage.selectedCountryCode = "+49";
                                            } else if (txt.indexOf("+33") === 0 && shellPage.selectedCountryCode !== "+33") {
                                                shellPage.selectedCountry = "France";
                                                shellPage.selectedCountryCode = "+33";
                                            } else if (txt.indexOf("+7") === 0 && shellPage.selectedCountryCode !== "+7") {
                                                shellPage.selectedCountry = "Russia";
                                                shellPage.selectedCountryCode = "+7";
                                            }
                                        }
                                    }
                                }
                            }

                            // Status / Error message
                            Container {
                                visible: auth.statusMessage.length > 0
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 12.0
                                Label {
                                    text: auth.statusMessage
                                    textStyle.color: Color.create("#24A1DE")
                                    textStyle.fontSize: FontSize.XSmall
                                    textStyle.textAlign: TextAlign.Center
                                    multiline: true
                                }
                            }

                            // NEXT Button (Pill shaped, full width, Telegram Blue)
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 50.0
                                background: Color.create("#24A1DE")
                                layout: DockLayout {}
                                bottomMargin: 22.0
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            var num = phoneInputField.text.trim();
                                            if (num.length > 0 && num !== shellPage.selectedCountryCode) {
                                                auth.submitPhoneNumber(num);
                                            }
                                        }
                                    }
                                ]
                                Label {
                                    text: auth.isBusy ? "PLEASE WAIT..." : "NEXT"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.Medium
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            // LOG IN BY QR CODE Link
                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            auth.requestQrLogin();
                                        }
                                    }
                                ]
                                Label {
                                    text: "LOG IN BY QR CODE"
                                    textStyle.color: Color.create("#24A1DE")
                                    textStyle.fontSize: FontSize.Small
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                            }
                        }

                        // -----------------------------------------------------
                        // STATE: QR CODE LOGIN (state 6)
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 6
                            horizontalAlignment: HorizontalAlignment.Fill

                            ImageView {
                                imageSource: "asset:///images/telegram_logo.png"
                                preferredWidth: 84.0
                                preferredHeight: 84.0
                                minWidth: 84.0
                                minHeight: 84.0
                                maxWidth: 84.0
                                maxHeight: 84.0
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 16.0
                            }

                            Label {
                                text: "Log in to Telegram by QR Code"
                                textStyle.color: Color.create("#222222")
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 8.0
                            }

                            Label {
                                text: "1. Open Telegram on your phone\n2. Go to Settings > Devices > Link Desktop\n3. Point your phone at this screen"
                                textStyle.color: Color.create("#707579")
                                textStyle.fontSize: FontSize.XSmall
                                textStyle.textAlign: TextAlign.Center
                                multiline: true
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 18.0
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                background: Color.White
                                topPadding: 10.0
                                bottomPadding: 10.0
                                leftPadding: 10.0
                                rightPadding: 10.0
                                bottomMargin: 20.0
                                ImageView {
                                    imageSource: auth.qrImagePath
                                    preferredWidth: 240.0
                                    preferredHeight: 240.0
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            auth.cancelQrLogin();
                                        }
                                    }
                                ]
                                Label {
                                    text: "LOG IN BY PHONE NUMBER"
                                    textStyle.color: Color.create("#24A1DE")
                                    textStyle.fontSize: FontSize.Small
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                            }
                        }

                        // -----------------------------------------------------
                        // STATE: VERIFICATION CODE INPUT (state 3)
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 3
                            horizontalAlignment: HorizontalAlignment.Fill

                            ImageView {
                                imageSource: "asset:///images/telegram_logo.png"
                                preferredWidth: 84.0
                                preferredHeight: 84.0
                                minWidth: 84.0
                                minHeight: 84.0
                                maxWidth: 84.0
                                maxHeight: 84.0
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 16.0
                            }

                            Label {
                                text: "Enter Code"
                                textStyle.color: Color.create("#222222")
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 8.0
                            }

                            Label {
                                text: "We've sent an SMS with an activation code to your phone\n" + auth.phoneNumber
                                textStyle.color: Color.create("#707579")
                                textStyle.fontSize: FontSize.Small
                                textStyle.textAlign: TextAlign.Center
                                multiline: true
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 24.0
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 60.0
                                bottomMargin: 20.0
                                background: Color.create("#24A1DE")
                                topPadding: 1.5
                                bottomPadding: 1.5
                                leftPadding: 1.5
                                rightPadding: 1.5

                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    verticalAlignment: VerticalAlignment.Fill
                                    background: Color.White
                                    topPadding: 4.0
                                    bottomPadding: 4.0
                                    leftPadding: 14.0
                                    rightPadding: 14.0
                                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                                    Label {
                                        text: "Code"
                                        textStyle.color: Color.create("#24A1DE")
                                        textStyle.fontSize: FontSize.XXSmall
                                    }

                                    TextField {
                                        id: codeInputField
                                        hintText: "Code"
                                        inputMode: TextFieldInputMode.NumbersAndPunctuation
                                        backgroundVisible: false
                                        textStyle.fontSize: FontSize.Medium
                                        textStyle.color: Color.create("#222222")
                                    }
                                }
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 50.0
                                background: Color.create("#24A1DE")
                                layout: DockLayout {}
                                bottomMargin: 16.0
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            auth.submitLoginCode(codeInputField.text);
                                        }
                                    }
                                ]
                                Label {
                                    text: auth.isBusy ? "VERIFYING..." : "NEXT"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.Medium
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            auth.changePhoneNumber();
                                        }
                                    }
                                ]
                                Label {
                                    text: "Wrong number?"
                                    textStyle.color: Color.create("#24A1DE")
                                    textStyle.fontSize: FontSize.Small
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                            }
                        }

                        // -----------------------------------------------------
                        // STATE: 2FA PASSWORD INPUT (state 4)
                        // -----------------------------------------------------
                        Container {
                            visible: auth.authState == 4
                            horizontalAlignment: HorizontalAlignment.Fill

                            ImageView {
                                imageSource: "asset:///images/telegram_logo.png"
                                preferredWidth: 84.0
                                preferredHeight: 84.0
                                minWidth: 84.0
                                minHeight: 84.0
                                maxWidth: 84.0
                                maxHeight: 84.0
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 16.0
                            }

                            Label {
                                text: "Two-Step Verification"
                                textStyle.color: Color.create("#222222")
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 8.0
                            }

                            Label {
                                text: auth.passwordHint.length > 0 ? ("Your account is protected with a password.\nHint: " + auth.passwordHint) : "Enter your cloud password"
                                textStyle.color: Color.create("#707579")
                                textStyle.fontSize: FontSize.Small
                                textStyle.textAlign: TextAlign.Center
                                multiline: true
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 24.0
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 60.0
                                bottomMargin: 20.0
                                background: Color.create("#24A1DE")
                                topPadding: 1.5
                                bottomPadding: 1.5
                                leftPadding: 1.5
                                rightPadding: 1.5

                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    verticalAlignment: VerticalAlignment.Fill
                                    background: Color.White
                                    topPadding: 4.0
                                    bottomPadding: 4.0
                                    leftPadding: 14.0
                                    rightPadding: 14.0
                                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                                    Label {
                                        text: "Password"
                                        textStyle.color: Color.create("#24A1DE")
                                        textStyle.fontSize: FontSize.XXSmall
                                    }

                                    TextField {
                                        id: passwordInputField
                                        hintText: "Password"
                                        inputMode: TextFieldInputMode.Password
                                        backgroundVisible: false
                                        textStyle.fontSize: FontSize.Medium
                                        textStyle.color: Color.create("#222222")
                                    }
                                }
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 50.0
                                background: Color.create("#24A1DE")
                                layout: DockLayout {}
                                bottomMargin: 16.0
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            auth.submitPassword(passwordInputField.text);
                                        }
                                    }
                                ]
                                Label {
                                    text: auth.isBusy ? "VERIFYING..." : "NEXT"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.Medium
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }
                        }
                    }
                }
            }

            // =================================================================
            // SCREEN: CHAT LIST
            // =================================================================
            Container {
                id: chatsScreen
                visible: auth.authState == 5 && shellPage.activeSection == 0
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                layout: DockLayout {}

                // Content column
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    // Custom application header
                    Container {
                        id: chatsHeader
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        background: tPanel
                        leftPadding: 14.0
                        rightPadding: 14.0

                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }

                        Container {
                            verticalAlignment: VerticalAlignment.Center
                            layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                            Label {
                                text: "Telegram"
                                textStyle.color: tPrimary
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                            }
                        }

                        // Sync button (custom)
                        Container {
                            id: syncBtn
                            verticalAlignment: VerticalAlignment.Center
                            preferredWidth: 52.0
                            preferredHeight: 52.0
                            layout: DockLayout {}
                            gestureHandlers: [
                                TapHandler {
                                    onTapped: {
                                        chatList.refreshDialogs();
                                    }
                                }
                            ]
                            Label {
                                text: "\u21BB"
                                textStyle.color: tBlue
                                textStyle.fontSize: FontSize.XXLarge
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
                            }
                        }
                    }

                    // Search field (custom field look)
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 46.0
                        background: tPanel
                        topPadding: 6.0
                        bottomPadding: 6.0
                        leftPadding: 14.0
                        rightPadding: 14.0

                        Container {
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                            minHeight: 40.0
                            background: tBg
                            layout: DockLayout {}

                            Label {
                                text: "\u2315"
                                textStyle.color: tSecondary
                                textStyle.fontSize: FontSize.Medium
                                verticalAlignment: VerticalAlignment.Center
                                leftMargin: 12.0
                            }

                            TextField {
                                id: searchField
                                hintText: "Search"
                                inputMode: TextFieldInputMode.Text
                                backgroundVisible: false
                                verticalAlignment: VerticalAlignment.Center
                                leftMargin: 34.0
                                rightMargin: 8.0
                                onTextChanging: {
                                    chatList.searchQuery = text;
                                }
                            }
                        }
                    }

                    // List area
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                        // States
                        Container {
                            visible: chatList.dialogsCount == 0 || auth.authState != 5
                            layout: DockLayout {}
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill

                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center

                                Label {
                                    visible: auth.authState != 5
                                    text: "Please sign in to view your chats."
                                    textStyle.color: tSecondary
                                    textStyle.fontSize: FontSize.Medium
                                    multiline: true
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                                Label {
                                    visible: auth.authState == 5 && chatList.isLoading
                                    text: "Syncing your chats..."
                                    textStyle.color: tBlue
                                    textStyle.fontSize: FontSize.Medium
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                                Label {
                                    visible: auth.authState == 5 && !chatList.isLoading && chatList.dialogsCount == 0
                                    text: "No chats yet."
                                    textStyle.color: tSecondary
                                    textStyle.fontSize: FontSize.Medium
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                                Container {
                                    visible: auth.authState == 5 && !chatList.isLoading && chatList.dialogsCount == 0
                                    horizontalAlignment: HorizontalAlignment.Center
                                    topMargin: 16.0
                                    minWidth: 160.0
                                    minHeight: 44.0
                                    background: tBlue
                                    layout: DockLayout {}
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: { chatList.refreshDialogs(); }
                                        }
                                    ]
                                    Label {
                                        text: "Sync Chats"
                                        textStyle.color: Color.White
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                            }
                        }

                        // Custom dialog list (flat Telegram rows)
                        ListView {
                            visible: auth.authState == 5 && chatList.dialogsCount > 0
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                            dataModel: chatList.model
                            scrollRole: ScrollRole.Main

                            listItemComponents: [
                                ListItemComponent {
                                    type: "item"
                                    Container {
                                        id: rowRoot
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        topPadding: 10.0
                                        bottomPadding: 10.0
                                        leftPadding: 12.0
                                        rightPadding: 12.0

                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }

                                        // Avatar
                                        Container {
                                            verticalAlignment: VerticalAlignment.Center
                                            preferredWidth: 54.0
                                            preferredHeight: 54.0
                                            minWidth: 54.0
                                            minHeight: 54.0
                                            background: Color.create(ListItemData.avatarColor ? ListItemData.avatarColor : "#5288c1")
                                            rightMargin: 12.0
                                            layout: DockLayout {}

                                            Label {
                                                visible: !ListItemData.avatarPath || ListItemData.avatarPath.length == 0
                                                horizontalAlignment: HorizontalAlignment.Center
                                                verticalAlignment: VerticalAlignment.Center
                                                text: ListItemData.initials ? ListItemData.initials : "?"
                                                textStyle.color: Color.White
                                                textStyle.fontSize: FontSize.Medium
                                                textStyle.fontWeight: FontWeight.Bold
                                            }
                                            ImageView {
                                                visible: ListItemData.avatarPath && ListItemData.avatarPath.length > 0
                                                imageSource: ListItemData.avatarPath ? (ListItemData.avatarPath.indexOf("file://") === 0 ? ListItemData.avatarPath : "file://" + ListItemData.avatarPath) : ""
                                                preferredWidth: 54.0
                                                preferredHeight: 54.0
                                                scalingMethod: ScalingMethod.AspectFill
                                                horizontalAlignment: HorizontalAlignment.Center
                                                verticalAlignment: VerticalAlignment.Center
                                            }
                                        }

                                        // Text column
                                        Container {
                                            verticalAlignment: VerticalAlignment.Center
                                            layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                                            Container {
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                                Label {
                                                    text: ListItemData.title ? ListItemData.title : ""
                                                    textStyle.color: Color.create("#ffffff")
                                                    textStyle.fontSize: FontSize.Small
                                                    textStyle.fontWeight: FontWeight.Bold
                                                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                                }
                                                Label {
                                                    text: ListItemData.formattedTime ? ListItemData.formattedTime : ""
                                                    textStyle.color: Color.create("#7f8b99")
                                                    textStyle.fontSize: FontSize.XSmall
                                                    verticalAlignment: VerticalAlignment.Center
                                                }
                                            }

                                            Container {
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                topMargin: 2.0
                                                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                                Label {
                                                    text: ListItemData.lastMessage ? ListItemData.lastMessage : ""
                                                    textStyle.color: ListItemData.isOutgoing ? Color.create("#4dcd5e") : Color.create("#7f8b99")
                                                    textStyle.fontSize: FontSize.XSmall
                                                    textStyle.fontWeight: ListItemData.unreadCount > 0 ? FontWeight.Bold : FontWeight.Normal
                                                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                                }
                                                Container {
                                                    visible: ListItemData.unreadCount > 0
                                                    verticalAlignment: VerticalAlignment.Center
                                                    minWidth: 22.0
                                                    minHeight: 22.0
                                                    background: Color.create("#2ea043")
                                                    topPadding: 2.0
                                                    bottomPadding: 2.0
                                                    leftPadding: 6.0
                                                    rightPadding: 6.0
                                                    layout: DockLayout {}
                                                    Label {
                                                        text: "" + ListItemData.unreadCount
                                                        textStyle.color: Color.White
                                                        textStyle.fontSize: FontSize.XXSmall
                                                        textStyle.fontWeight: FontWeight.Bold
                                                        horizontalAlignment: HorizontalAlignment.Center
                                                        verticalAlignment: VerticalAlignment.Center
                }
            }
                                            }
                                        }
                                    }
                                }
                            ]

                            onTriggered: {
                                var data = chatList.model.data(indexPath);
                                if (data) {
                                    chatList.selectDialog(indexPath);
                                    chatList.logDiagnostic("before createObject");
                                    var page = chatScreenDef.createObject();
                                    chatList.logDiagnostic("page null? " + (page === null ? "YES" : "NO"));
                                    if (page) {
                                        page.loadChat(data.title, data.peerType, "" + data.peerId, "" + data.accessHash, data.initials, data.avatarColor, data.avatarPath ? data.avatarPath : "", chatList.canSend);
                                        chatList.logDiagnostic("about to push");
                                        rootNavPane.push(page);
                                        chatList.logDiagnostic("pushed");
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // =================================================================
            // SCREEN: ACCOUNT (auth / profile)
            // =================================================================
            Container {
                id: accountScreen
                visible: auth.authState == 5 && shellPage.activeSection == 1
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                layout: DockLayout {}

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    // Custom header
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        background: tPanel
                        leftPadding: 14.0
                        rightPadding: 14.0
                        layout: DockLayout {}
                        Label {
                            text: "Account"
                            textStyle.color: tPrimary
                            textStyle.fontSize: FontSize.Large
                            textStyle.fontWeight: FontWeight.Bold
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    ScrollView {
                        id: accountScroll
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                        Container {
                            id: accountScrollContent
                            horizontalAlignment: HorizontalAlignment.Fill
                            topPadding: 14.0
                            bottomPadding: 96.0
                            leftPadding: 14.0
                            rightPadding: 14.0

                            // Brand icon
                            Container {
                                preferredWidth: 72.0
                                preferredHeight: 72.0
                                minWidth: 72.0
                                minHeight: 72.0
                                background: tBlue
                                layout: DockLayout {}
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 10.0
                                Label {
                                    text: "\u2708"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.XXLarge
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            // Status pill
                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                background: diagnostic.isEncrypted ? tGreen : (diagnostic.isConnected ? tBlue : tDanger)
                                topPadding: 4.0
                                bottomPadding: 4.0
                                leftPadding: 12.0
                                rightPadding: 12.0
                                bottomMargin: 14.0
                                Label {
                                    text: diagnostic.isEncrypted ? "ONLINE" : (diagnostic.isConnected ? "CONNECTING" : "OFFLINE")
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.XXSmall
                                    textStyle.fontWeight: FontWeight.Bold
                                }
                            }

                            // Status message banner
                            Container {
                                visible: auth.statusMessage.length > 0
                                horizontalAlignment: HorizontalAlignment.Fill
                                background: tPanel
                                topPadding: 10.0
                                bottomPadding: 10.0
                                leftPadding: 14.0
                                rightPadding: 14.0
                                bottomMargin: 14.0
                                Label {
                                    text: auth.statusMessage
                                    textStyle.color: tBlue
                                    textStyle.fontSize: FontSize.Small
                                    multiline: true
                                }
                            }

                            // LOGGED IN PROFILE (state 5)
                            Container {
                                visible: auth.authState == 5
                                horizontalAlignment: HorizontalAlignment.Fill
                                verticalAlignment: VerticalAlignment.Center
                                topPadding: 18.0
                                bottomPadding: 18.0
                                leftPadding: 18.0
                                rightPadding: 18.0

                                Container {
                                    preferredWidth: 84.0
                                    preferredHeight: 84.0
                                    minWidth: 84.0
                                    minHeight: 84.0
                                    background: tBlue
                                    layout: DockLayout {}
                                    horizontalAlignment: HorizontalAlignment.Center
                                    bottomMargin: 10.0
                                    Label {
                                        text: auth.userName.length > 0 ? auth.userName.trim().left(1).toUpperCase() : "?"
                                        textStyle.color: Color.White
                                        textStyle.fontSize: FontSize.XXLarge
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                                Label {
                                    text: auth.userName
                                    textStyle.color: tPrimary
                                    textStyle.fontSize: FontSize.Large
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                                Label {
                                    visible: auth.userHandle.length > 0
                                    text: auth.userHandle
                                    textStyle.color: tBlue
                                    textStyle.fontSize: FontSize.Small
                                    horizontalAlignment: HorizontalAlignment.Center
                                    bottomMargin: 14.0
                                }
                                Label {
                                    text: "ID: " + auth.userId
                                    textStyle.color: tSecondary
                                    textStyle.fontSize: FontSize.XSmall
                                    horizontalAlignment: HorizontalAlignment.Center
                                }
                                Label {
                                    text: "Phone: " + auth.userPhone
                                    textStyle.color: tSecondary
                                    textStyle.fontSize: FontSize.XSmall
                                    horizontalAlignment: HorizontalAlignment.Center
                                    bottomMargin: 16.0
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    minHeight: 48.0
                                    background: tBlue
                                    layout: DockLayout {}
                                    bottomMargin: 10.0
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: { shellPage.activeSection = 0; }
                                        }
                                    ]
                                    Label {
                                        text: "View Chats"
                                        textStyle.color: Color.White
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    minHeight: 48.0
                                    background: tDanger
                                    layout: DockLayout {}
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: { auth.logout(); }
                                        }
                                    ]
                                    Label {
                                        text: "Log Out"
                                        textStyle.color: Color.White
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // ================================================================= =================================================================
            // SCREEN: CONTACTS (peers from the dialog list)
            // =================================================================
            Container {
                id: contactsScreen
                visible: auth.authState == 5 && shellPage.activeSection == 2
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                layout: DockLayout {}

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    // Custom header
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        background: tPanel
                        leftPadding: 14.0
                        rightPadding: 14.0
                        layout: DockLayout {}
                        Label {
                            text: "Contacts"
                            textStyle.color: tPrimary
                            textStyle.fontSize: FontSize.Large
                            textStyle.fontWeight: FontWeight.Bold
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    // States
                    Container {
                        visible: auth.authState != 5 || chatList.dialogsCount == 0
                        layout: DockLayout {}
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        Label {
                            text: auth.authState != 5 ? "Please sign in to see your contacts." : "No contacts yet."
                            textStyle.color: tSecondary
                            textStyle.fontSize: FontSize.Medium
                            multiline: true
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    // Contacts list (same peer data as chats)
                    ListView {
                        visible: auth.authState == 5 && chatList.dialogsCount > 0
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        dataModel: chatList.model
                        scrollRole: ScrollRole.Main

                        listItemComponents: [
                            ListItemComponent {
                                type: "item"
                                Container {
                                    id: contactRow
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    topPadding: 8.0
                                    bottomPadding: 8.0
                                    leftPadding: 12.0
                                    rightPadding: 12.0

                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                    Container {
                                        verticalAlignment: VerticalAlignment.Center
                                        preferredWidth: 46.0
                                        preferredHeight: 46.0
                                        minWidth: 46.0
                                        minHeight: 46.0
                                        background: Color.create(ListItemData.avatarColor ? ListItemData.avatarColor : "#5288c1")
                                        rightMargin: 12.0
                                        layout: DockLayout {}
                                        Label {
                                            visible: !ListItemData.avatarPath || ListItemData.avatarPath.length == 0
                                            horizontalAlignment: HorizontalAlignment.Center
                                            verticalAlignment: VerticalAlignment.Center
                                            text: ListItemData.initials ? ListItemData.initials : "?"
                                            textStyle.color: Color.White
                                            textStyle.fontSize: FontSize.Medium
                                            textStyle.fontWeight: FontWeight.Bold
                                        }
                                        ImageView {
                                            visible: ListItemData.avatarPath && ListItemData.avatarPath.length > 0
                                            imageSource: ListItemData.avatarPath ? (ListItemData.avatarPath.indexOf("file://") === 0 ? ListItemData.avatarPath : "file://" + ListItemData.avatarPath) : ""
                                            preferredWidth: 46.0
                                            preferredHeight: 46.0
                                            scalingMethod: ScalingMethod.AspectFill
                                            horizontalAlignment: HorizontalAlignment.Center
                                            verticalAlignment: VerticalAlignment.Center
                                        }
                                    }

                                    Container {
                                        verticalAlignment: VerticalAlignment.Center
                                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                        Label {
                                            text: ListItemData.title ? ListItemData.title : ""
                                            textStyle.color: Color.create("#ffffff")
                                            textStyle.fontSize: FontSize.Small
                                            textStyle.fontWeight: FontWeight.Bold
                                        }
                                        Label {
                                            visible: ListItemData.peerType && ListItemData.peerType.length > 0
                                            text: ListItemData.peerType ? ListItemData.peerType.toUpperCase() : ""
                                            textStyle.color: Color.create("#4dcd5e")
                                            textStyle.fontSize: FontSize.XXSmall
                                        }
                                    }
                                }
                            }
                        ]

                        onTriggered: {
                            var data = chatList.model.data(indexPath);
                            if (data) {
                                chatList.selectDialog(indexPath);
                                chatList.logDiagnostic("before createObject");
                                var page = chatScreenDef.createObject();
                                chatList.logDiagnostic("page null? " + (page === null ? "YES" : "NO"));
                                if (page) {
                                    page.loadChat(data.title, data.peerType, "" + data.peerId, "" + data.accessHash, data.initials, data.avatarColor, data.avatarPath ? data.avatarPath : "", chatList.canSend);
                                    chatList.logDiagnostic("about to push");
                                    rootNavPane.push(page);
                                    chatList.logDiagnostic("pushed");
                                }
                            }
                        }
                    }
                }
            }

            // =================================================================
            // SCREEN: SETTINGS (connection / account info)
            // =================================================================
            Container {
                id: settingsScreen
                visible: auth.authState == 5 && shellPage.activeSection == 3
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                layout: DockLayout {}

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    // Custom header
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        background: tPanel
                        leftPadding: 14.0
                        rightPadding: 14.0
                        layout: DockLayout {}
                        Label {
                            text: "Settings"
                            textStyle.color: tPrimary
                            textStyle.fontSize: FontSize.Large
                            textStyle.fontWeight: FontWeight.Bold
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    ScrollView {
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                        Container {
                            horizontalAlignment: HorizontalAlignment.Fill
                            topPadding: 12.0
                            bottomPadding: 20.0
                            leftPadding: 14.0
                            rightPadding: 14.0

                            // Section: Connection
                            Label {
                                text: "CONNECTION"
                                textStyle.color: tSecondary
                                textStyle.fontSize: FontSize.XXSmall
                                textStyle.fontWeight: FontWeight.Bold
                                bottomMargin: 6.0
                            }
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                background: tPanel
                                topPadding: 12.0
                                bottomPadding: 12.0
                                leftPadding: 14.0
                                rightPadding: 14.0
                                bottomMargin: 16.0

                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    Label { text: "Status"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label {
                                        text: diagnostic.isEncrypted ? "Connected (encrypted)" : (diagnostic.isConnected ? "Connecting..." : "Offline")
                                        textStyle.color: diagnostic.isEncrypted ? tGreen : (diagnostic.isConnected ? tBlue : tDanger)
                                    }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "Data center"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: diagnostic.dcInfo; textStyle.color: tPrimary }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "Auth key"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: diagnostic.authKeyId; textStyle.color: tPrimary; textStyle.fontSize: FontSize.XXSmall }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "Signed in"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: auth.authState == 5 ? "Yes" : "No"; textStyle.color: auth.authState == 5 ? tGreen : tSecondary }
                                }
                            }

                            // Section: Account
                            Label {
                                text: "ACCOUNT"
                                textStyle.color: tSecondary
                                textStyle.fontSize: FontSize.XXSmall
                                textStyle.fontWeight: FontWeight.Bold
                                bottomMargin: 6.0
                            }
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                background: tPanel
                                topPadding: 12.0
                                bottomPadding: 12.0
                                leftPadding: 14.0
                                rightPadding: 14.0
                                bottomMargin: 16.0

                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    Label { text: "Name"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: auth.userName.length > 0 ? auth.userName : "Not signed in"; textStyle.color: tPrimary }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "Username"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: auth.userHandle.length > 0 ? auth.userHandle : "—"; textStyle.color: tPrimary }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "Phone"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: auth.userPhone.length > 0 ? auth.userPhone : "—"; textStyle.color: tPrimary }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "User ID"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: auth.userId; textStyle.color: tPrimary }
                                }
                            }

                            // Section: Actions
                            Label {
                                text: "ACTIONS"
                                textStyle.color: tSecondary
                                textStyle.fontSize: FontSize.XXSmall
                                textStyle.fontWeight: FontWeight.Bold
                                bottomMargin: 6.0
                            }
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                background: tPanel
                                topPadding: 4.0
                                bottomPadding: 4.0
                                leftPadding: 4.0
                                rightPadding: 4.0

                                Container {
                                    visible: auth.authState == 5
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    minHeight: 48.0
                                    layout: DockLayout {}
                                    gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 2; } } ]
                                    Label {
                                        text: "View Contacts"
                                        textStyle.color: tBlue
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Left
                                        leftMargin: 12.0
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    minHeight: 48.0
                                    layout: DockLayout {}
                                    gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 4; } } ]
                                    Label {
                                        text: "About TelegramBB10"
                                        textStyle.color: tBlue
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Left
                                        leftMargin: 12.0
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // =================================================================
            // SCREEN: ABOUT
            // =================================================================
            Container {
                id: aboutScreen
                visible: auth.authState == 5 && shellPage.activeSection == 4
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                layout: DockLayout {}

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    // Custom header
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        background: tPanel
                        leftPadding: 14.0
                        rightPadding: 14.0
                        layout: DockLayout {}
                        Label {
                            text: "About"
                            textStyle.color: tPrimary
                            textStyle.fontSize: FontSize.Large
                            textStyle.fontWeight: FontWeight.Bold
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    ScrollView {
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                        Container {
                            horizontalAlignment: HorizontalAlignment.Fill
                            topPadding: 30.0
                            bottomPadding: 20.0
                            leftPadding: 16.0
                            rightPadding: 16.0

                            // App icon
                            Container {
                                preferredWidth: 84.0
                                preferredHeight: 84.0
                                minWidth: 84.0
                                minHeight: 84.0
                                background: tBlue
                                layout: DockLayout {}
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 12.0
                                Label {
                                    text: "\u2708"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.XXLarge
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            Label {
                                text: "TelegramBB10"
                                textStyle.color: tPrimary
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                                horizontalAlignment: HorizontalAlignment.Center
                            }
                            Label {
                                text: "A faithful 2026 'Liquid Glass' custom UI\nfor the BlackBerry 10 Telegram client."
                                textStyle.color: tSecondary
                                textStyle.fontSize: FontSize.Small
                                textStyle.textAlign: TextAlign.Center
                                multiline: true
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 18.0
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                background: tPanel
                                topPadding: 12.0
                                bottomPadding: 12.0
                                leftPadding: 14.0
                                rightPadding: 14.0
                                bottomMargin: 18.0

                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    Label { text: "Version"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: "1.6.0 (custom UI)"; textStyle.color: tPrimary }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "MTProto layer"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: "195"; textStyle.color: tPrimary }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    topMargin: 4.0
                                    Label { text: "Transport"; textStyle.color: tSecondary; layoutProperties: StackLayoutProperties { spaceQuota: 1.0 } }
                                    Label { text: "MTProto Intermediate (TCP)"; textStyle.color: tPrimary }
                                }
                            }

                            Label {
                                text: "This is an independent, unofficial client.\nTelegram is a trademark of Telegram Group Ltd.\nEverything is rendered fully custom in QML — no\nnative BB10 chrome, no web views."
                                textStyle.color: tSecondary
                                textStyle.fontSize: FontSize.XXSmall
                                textStyle.textAlign: TextAlign.Center
                                multiline: true
                                horizontalAlignment: HorizontalAlignment.Center
                                bottomMargin: 18.0
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                minHeight: 46.0
                                background: tBlue
                                layout: DockLayout {}
                                gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 0; } } ]
                                Label {
                                    text: "Start Using"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.Small
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }
                        }
                    }
                }
            }

            // =================================================================
            // CUSTOM BOTTOM NAVIGATION RAIL
            // =================================================================
            Container {
                id: bottomNavBar
                visible: auth.authState == 5
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Bottom
                minHeight: 62.0
                background: tPanel

                layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 1.0
                    maxHeight: 1.0
                    background: Color.create("#1e2c3a")
                }

                Container {
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    topPadding: 4.0
                    bottomPadding: 4.0
                    horizontalAlignment: HorizontalAlignment.Fill

                // Chats
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    horizontalAlignment: HorizontalAlignment.Fill
                    gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 0; } } ]

                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Bottom
                        minWidth: 44.0
                        minHeight: 2.0
                        background: shellPage.activeSection == 0 ? tBlue : Color.Transparent
                    }
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "\u2709" // envelope
                            textStyle.color: shellPage.activeSection == 0 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXLarge
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Label {
                            text: "Chats"
                            textStyle.color: shellPage.activeSection == 0 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXSmall
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 18.0
                        }
                    }
                }

                // Account
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    horizontalAlignment: HorizontalAlignment.Fill
                    gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 1; } } ]

                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Bottom
                        minWidth: 44.0
                        minHeight: 2.0
                        background: shellPage.activeSection == 1 ? tBlue : Color.Transparent
                    }
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "\u25C9" // circle user
                            textStyle.color: shellPage.activeSection == 1 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXLarge
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Label {
                            text: "Account"
                            textStyle.color: shellPage.activeSection == 1 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXSmall
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 18.0
                        }
                    }
                }

                // Contacts
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    horizontalAlignment: HorizontalAlignment.Fill
                    gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 2; } } ]

                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Bottom
                        minWidth: 32.0
                        minHeight: 2.0
                        background: shellPage.activeSection == 2 ? tBlue : Color.Transparent
                    }
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "\u260E" // telephone glyph
                            textStyle.color: shellPage.activeSection == 2 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXLarge
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Label {
                            text: "Contacts"
                            textStyle.color: shellPage.activeSection == 2 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXSmall
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 18.0
                        }
                    }
                }

                // Settings
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    horizontalAlignment: HorizontalAlignment.Fill
                    gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 3; } } ]

                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Bottom
                        minWidth: 32.0
                        minHeight: 2.0
                        background: shellPage.activeSection == 3 ? tBlue : Color.Transparent
                    }
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "\u2699" // gear glyph
                            textStyle.color: shellPage.activeSection == 3 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXLarge
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Label {
                            text: "Settings"
                            textStyle.color: shellPage.activeSection == 3 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXSmall
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 18.0
                        }
                    }
                }

                // About
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    horizontalAlignment: HorizontalAlignment.Fill
                    gestureHandlers: [ TapHandler { onTapped: { shellPage.activeSection = 4; } } ]

                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Bottom
                        minWidth: 32.0
                        minHeight: 2.0
                        background: shellPage.activeSection == 4 ? tBlue : Color.Transparent
                    }
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "\u2139" // info glyph
                            textStyle.color: shellPage.activeSection == 4 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXLarge
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Label {
                            text: "About"
                            textStyle.color: shellPage.activeSection == 4 ? tBlue : tSecondary
                            textStyle.fontSize: FontSize.XXSmall
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 18.0
                        }
                    }
                }
                }
            }
        }

        onCreationCompleted: {
            shellPage.activeSection = 0;
        }
    }

    onPopTransitionEnded: {
        page.destroy();
    }

    attachedObjects: [
        ComponentDefinition {
            id: chatScreenDef
            source: "ChatScreen.qml"
        }
    ]
}