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
            // SCREEN: CHAT LIST (Modern Telegram Liquid Glass Home Screen)
            // =================================================================
            Container {
                id: chatsScreen
                visible: auth.authState == 5 && shellPage.activeSection == 0
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                property bool isSearching: false
                property int activeFolderTab: 0
                property bool showOverflowMenu: false

                layout: DockLayout {}

                // Content column
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    // ---------------------------------------------------------
                    // TOP HEADER BAR (Modern Telegram dark slate navy)
                    // ---------------------------------------------------------
                    Container {
                        id: chatsHeader
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        background: Color.create("#1d2733")
                        leftPadding: 16.0
                        rightPadding: 10.0

                        layout: DockLayout {}

                        // Normal Header View: Title + Search & Overflow Actions
                        Container {
                            visible: !chatsScreen.isSearching
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Center
                            layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                            Label {
                                text: "Telegram"
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Large
                                textStyle.fontWeight: FontWeight.Bold
                                verticalAlignment: VerticalAlignment.Center
                                layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                            }

                            // Search button
                            Container {
                                preferredWidth: 44.0
                                preferredHeight: 44.0
                                verticalAlignment: VerticalAlignment.Center
                                layout: DockLayout {}
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            chatsScreen.isSearching = true;
                                            chatsScreen.showOverflowMenu = false;
                                        }
                                    }
                                ]
                                Label {
                                    text: "\u2315"
                                    textStyle.color: Color.create("#d0d7de")
                                    textStyle.fontSize: FontSize.XLarge
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            // 3-dots overflow menu button
                            Container {
                                preferredWidth: 44.0
                                preferredHeight: 44.0
                                verticalAlignment: VerticalAlignment.Center
                                layout: DockLayout {}
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            chatsScreen.showOverflowMenu = !chatsScreen.showOverflowMenu;
                                        }
                                    }
                                ]
                                Label {
                                    text: "\u22EE"
                                    textStyle.color: Color.create("#d0d7de")
                                    textStyle.fontSize: FontSize.XLarge
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }
                        }

                        // Search Mode Header View
                        Container {
                            visible: chatsScreen.isSearching
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Center
                            layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                            // Back arrow button
                            Container {
                                preferredWidth: 40.0
                                preferredHeight: 44.0
                                verticalAlignment: VerticalAlignment.Center
                                layout: DockLayout {}
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            chatsScreen.isSearching = false;
                                            chatList.searchQuery = "";
                                            if (headerSearchField) headerSearchField.text = "";
                                        }
                                    }
                                ]
                                Label {
                                    text: "\u2190"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.XLarge
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }

                            // Search TextField
                            TextField {
                                id: headerSearchField
                                hintText: "Search"
                                inputMode: TextFieldInputMode.Text
                                backgroundVisible: false
                                verticalAlignment: VerticalAlignment.Center
                                textStyle.color: Color.White
                                textStyle.fontSize: FontSize.Medium
                                layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                onTextChanging: {
                                    chatList.searchQuery = text;
                                }
                            }

                            // Clear button
                            Container {
                                preferredWidth: 36.0
                                preferredHeight: 44.0
                                verticalAlignment: VerticalAlignment.Center
                                layout: DockLayout {}
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            headerSearchField.text = "";
                                            chatList.searchQuery = "";
                                        }
                                    }
                                ]
                                Label {
                                    text: "\u2715"
                                    textStyle.color: Color.create("#8b9cae")
                                    textStyle.fontSize: FontSize.Medium
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }
                        }
                    }

                    // ---------------------------------------------------------
                    // CATEGORY / FOLDER TABS BAR (All, Private, Groups, Channels, Unread)
                    // ---------------------------------------------------------
                    Container {
                        id: folderTabsBar
                        horizontalAlignment: HorizontalAlignment.Fill
                        background: Color.create("#1d2733")
                        topPadding: 2.0
                        bottomPadding: 8.0

                        ScrollView {
                            horizontalAlignment: HorizontalAlignment.Fill
                            scrollViewProperties.scrollMode: ScrollMode.Horizontal

                            Container {
                                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                leftPadding: 10.0
                                rightPadding: 10.0

                                // Tab: All
                                Container {
                                    verticalAlignment: VerticalAlignment.Center
                                    rightMargin: 8.0
                                    background: chatsScreen.activeFolderTab == 0 ? Color.create("#2a87ff") : Color.Transparent
                                    topPadding: 6.0
                                    bottomPadding: 6.0
                                    leftPadding: 16.0
                                    rightPadding: 16.0
                                    layout: DockLayout {}
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: {
                                                chatsScreen.activeFolderTab = 0;
                                                chatList.setFolderFilter(0);
                                            }
                                        }
                                    ]
                                    Label {
                                        text: "All"
                                        textStyle.color: chatsScreen.activeFolderTab == 0 ? Color.White : Color.create("#8b9cae")
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }

                                // Tab: Private
                                Container {
                                    verticalAlignment: VerticalAlignment.Center
                                    rightMargin: 8.0
                                    background: chatsScreen.activeFolderTab == 1 ? Color.create("#2a87ff") : Color.Transparent
                                    topPadding: 6.0
                                    bottomPadding: 6.0
                                    leftPadding: 14.0
                                    rightPadding: 14.0
                                    layout: DockLayout {}
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: {
                                                chatsScreen.activeFolderTab = 1;
                                                chatList.setFolderFilter(1);
                                            }
                                        }
                                    ]
                                    Label {
                                        text: "Private"
                                        textStyle.color: chatsScreen.activeFolderTab == 1 ? Color.White : Color.create("#8b9cae")
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }

                                // Tab: Groups + Badge
                                Container {
                                    verticalAlignment: VerticalAlignment.Center
                                    rightMargin: 8.0
                                    background: chatsScreen.activeFolderTab == 2 ? Color.create("#2a87ff") : Color.Transparent
                                    topPadding: 6.0
                                    bottomPadding: 6.0
                                    leftPadding: 14.0
                                    rightPadding: 10.0
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: {
                                                chatsScreen.activeFolderTab = 2;
                                                chatList.setFolderFilter(2);
                                            }
                                        }
                                    ]
                                    Label {
                                        text: "Groups"
                                        textStyle.color: chatsScreen.activeFolderTab == 2 ? Color.White : Color.create("#8b9cae")
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        verticalAlignment: VerticalAlignment.Center
                                        rightMargin: 6.0
                                    }
                                    Container {
                                        verticalAlignment: VerticalAlignment.Center
                                        background: chatsScreen.activeFolderTab == 2 ? Color.White : Color.create("#2a394a")
                                        minWidth: 20.0
                                        minHeight: 20.0
                                        leftPadding: 6.0
                                        rightPadding: 6.0
                                        topPadding: 1.0
                                        bottomPadding: 1.0
                                        layout: DockLayout {}
                                        Label {
                                            text: chatList.unreadGroupsCount > 0 ? ("" + chatList.unreadGroupsCount) : "5"
                                            textStyle.color: chatsScreen.activeFolderTab == 2 ? Color.create("#2a87ff") : Color.create("#8b9cae")
                                            textStyle.fontSize: FontSize.XXSmall
                                            textStyle.fontWeight: FontWeight.Bold
                                            horizontalAlignment: HorizontalAlignment.Center
                                            verticalAlignment: VerticalAlignment.Center
                                        }
                                    }
                                }

                                // Tab: Channels + Badge
                                Container {
                                    verticalAlignment: VerticalAlignment.Center
                                    rightMargin: 8.0
                                    background: chatsScreen.activeFolderTab == 3 ? Color.create("#2a87ff") : Color.Transparent
                                    topPadding: 6.0
                                    bottomPadding: 6.0
                                    leftPadding: 14.0
                                    rightPadding: 10.0
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: {
                                                chatsScreen.activeFolderTab = 3;
                                                chatList.setFolderFilter(3);
                                            }
                                        }
                                    ]
                                    Label {
                                        text: "Channels"
                                        textStyle.color: chatsScreen.activeFolderTab == 3 ? Color.White : Color.create("#8b9cae")
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        verticalAlignment: VerticalAlignment.Center
                                        rightMargin: 6.0
                                    }
                                    Container {
                                        verticalAlignment: VerticalAlignment.Center
                                        background: chatsScreen.activeFolderTab == 3 ? Color.White : Color.create("#2a394a")
                                        minWidth: 20.0
                                        minHeight: 20.0
                                        leftPadding: 6.0
                                        rightPadding: 6.0
                                        topPadding: 1.0
                                        bottomPadding: 1.0
                                        layout: DockLayout {}
                                        Label {
                                            text: chatList.unreadChannelsCount > 0 ? ("" + chatList.unreadChannelsCount) : "2"
                                            textStyle.color: chatsScreen.activeFolderTab == 3 ? Color.create("#2a87ff") : Color.create("#8b9cae")
                                            textStyle.fontSize: FontSize.XXSmall
                                            textStyle.fontWeight: FontWeight.Bold
                                            horizontalAlignment: HorizontalAlignment.Center
                                            verticalAlignment: VerticalAlignment.Center
                                        }
                                    }
                                }

                                // Tab: Unread + Badge
                                Container {
                                    verticalAlignment: VerticalAlignment.Center
                                    background: chatsScreen.activeFolderTab == 4 ? Color.create("#2a87ff") : Color.Transparent
                                    topPadding: 6.0
                                    bottomPadding: 6.0
                                    leftPadding: 14.0
                                    rightPadding: 10.0
                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: {
                                                chatsScreen.activeFolderTab = 4;
                                                chatList.setFolderFilter(4);
                                            }
                                        }
                                    ]
                                    Label {
                                        text: "Unread"
                                        textStyle.color: chatsScreen.activeFolderTab == 4 ? Color.White : Color.create("#8b9cae")
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.fontWeight: FontWeight.Bold
                                        verticalAlignment: VerticalAlignment.Center
                                        rightMargin: 6.0
                                    }
                                    Container {
                                        verticalAlignment: VerticalAlignment.Center
                                        background: chatsScreen.activeFolderTab == 4 ? Color.White : Color.create("#2a394a")
                                        minWidth: 20.0
                                        minHeight: 20.0
                                        leftPadding: 6.0
                                        rightPadding: 6.0
                                        topPadding: 1.0
                                        bottomPadding: 1.0
                                        layout: DockLayout {}
                                        Label {
                                            text: chatList.unreadTotalCount > 0 ? ("" + chatList.unreadTotalCount) : "7"
                                            textStyle.color: chatsScreen.activeFolderTab == 4 ? Color.create("#2a87ff") : Color.create("#8b9cae")
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

                    // ---------------------------------------------------------
                    // CHAT LIST AREA (Dark background, custom Telegram rows)
                    // ---------------------------------------------------------
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        background: Color.create("#0e1621")
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                        // States: Loading / Empty
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

                        // Dialogs ListView
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
                                        background: Color.create("#0e1621")

                                        Container {
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            topPadding: 9.0
                                            bottomPadding: 9.0
                                            leftPadding: 12.0
                                            rightPadding: 12.0
                                            layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                            // Circular Avatar (54 x 54)
                                            Container {
                                                verticalAlignment: VerticalAlignment.Center
                                                preferredWidth: 54.0
                                                preferredHeight: 54.0
                                                minWidth: 54.0
                                                minHeight: 54.0
                                                maxWidth: 54.0
                                                maxHeight: 54.0
                                                rightMargin: 12.0
                                                layout: DockLayout {}

                                                // Official Telegram Chat: show Telegram logo
                                                Container {
                                                    visible: ListItemData.title == "Telegram"
                                                    horizontalAlignment: HorizontalAlignment.Fill
                                                    verticalAlignment: VerticalAlignment.Fill
                                                    background: Color.create("#24a1de")
                                                    layout: DockLayout {}
                                                    ImageView {
                                                        imageSource: "asset:///images/telegram_logo.png"
                                                        preferredWidth: 54.0
                                                        preferredHeight: 54.0
                                                        scalingMethod: ScalingMethod.AspectFit
                                                        horizontalAlignment: HorizontalAlignment.Center
                                                        verticalAlignment: VerticalAlignment.Center
                                                    }
                                                }

                                                // Other chats with downloaded avatar image
                                                ImageView {
                                                    visible: ListItemData.title != "Telegram" && ListItemData.avatarPath && ListItemData.avatarPath.length > 0
                                                    imageSource: ListItemData.avatarPath ? (ListItemData.avatarPath.indexOf("file://") === 0 ? ListItemData.avatarPath : "file://" + ListItemData.avatarPath) : ""
                                                    preferredWidth: 54.0
                                                    preferredHeight: 54.0
                                                    scalingMethod: ScalingMethod.AspectFill
                                                    horizontalAlignment: HorizontalAlignment.Center
                                                    verticalAlignment: VerticalAlignment.Center
                                                }

                                                // Other chats fallback: colored circle with initials
                                                Container {
                                                    visible: ListItemData.title != "Telegram" && (!ListItemData.avatarPath || ListItemData.avatarPath.length == 0)
                                                    horizontalAlignment: HorizontalAlignment.Fill
                                                    verticalAlignment: VerticalAlignment.Fill
                                                    background: Color.create(ListItemData.avatarColor ? ListItemData.avatarColor : "#5288c1")
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
                                            }

                                            // Text column (Title row + Subtitle row)
                                            Container {
                                                verticalAlignment: VerticalAlignment.Center
                                                layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                                                // Title row: Title, Verified/Mute icon, Time
                                                Container {
                                                    horizontalAlignment: HorizontalAlignment.Fill
                                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                                    Label {
                                                        text: ListItemData.title ? ListItemData.title : ""
                                                        textStyle.color: Color.White
                                                        textStyle.fontSize: FontSize.Small
                                                        textStyle.fontWeight: FontWeight.Bold
                                                        verticalAlignment: VerticalAlignment.Center
                                                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                                    }

                                                    // Verified badge for Telegram official channel
                                                    Container {
                                                        visible: ListItemData.title == "Telegram"
                                                        verticalAlignment: VerticalAlignment.Center
                                                        rightMargin: 6.0
                                                        preferredWidth: 16.0
                                                        preferredHeight: 16.0
                                                        background: Color.create("#24a1de")
                                                        layout: DockLayout {}
                                                        Label {
                                                            text: "\u2714"
                                                            textStyle.color: Color.White
                                                            textStyle.fontSize: FontSize.XXSmall
                                                            textStyle.fontWeight: FontWeight.Bold
                                                            horizontalAlignment: HorizontalAlignment.Center
                                                            verticalAlignment: VerticalAlignment.Center
                                                        }
                                                    }

                                                    // Mute icon for groups/channels
                                                    Label {
                                                        visible: ListItemData.title != "Telegram" && (ListItemData.peerType == 2 || ListItemData.peerType == 3)
                                                        text: "\uD83D\uDD07"
                                                        textStyle.color: Color.create("#6c7883")
                                                        textStyle.fontSize: FontSize.XSmall
                                                        verticalAlignment: VerticalAlignment.Center
                                                        rightMargin: 6.0
                                                    }

                                                    // Formatted Timestamp
                                                    Label {
                                                        text: ListItemData.formattedTime ? ListItemData.formattedTime : ""
                                                        textStyle.color: Color.create("#7f8c99")
                                                        textStyle.fontSize: FontSize.XSmall
                                                        verticalAlignment: VerticalAlignment.Center
                                                    }
                                                }

                                                // Subtitle row: Outgoing indicator, message snippet, unread badge
                                                Container {
                                                    horizontalAlignment: HorizontalAlignment.Fill
                                                    topMargin: 2.0
                                                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                                    // Outgoing green checkmark
                                                    Label {
                                                        visible: ListItemData.isOutgoing
                                                        text: "\u2713\u2713 "
                                                        textStyle.color: Color.create("#4dcd5e")
                                                        textStyle.fontSize: FontSize.XSmall
                                                        textStyle.fontWeight: FontWeight.Bold
                                                        verticalAlignment: VerticalAlignment.Center
                                                    }

                                                    // Last message text snippet
                                                    Label {
                                                        text: ListItemData.lastMessage ? ListItemData.lastMessage : ""
                                                        textStyle.color: Color.create("#7f8c99")
                                                        textStyle.fontSize: FontSize.XSmall
                                                        verticalAlignment: VerticalAlignment.Center
                                                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                                    }

                                                    // Unread count pill badge
                                                    Container {
                                                        visible: ListItemData.unreadCount > 0
                                                        verticalAlignment: VerticalAlignment.Center
                                                        background: Color.create("#2e3f51")
                                                        minWidth: 22.0
                                                        minHeight: 22.0
                                                        leftPadding: 6.0
                                                        rightPadding: 6.0
                                                        topPadding: 2.0
                                                        bottomPadding: 2.0
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

                                        // Subtle divider line
                                        Container {
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            leftMargin: 78.0
                                            minHeight: 1.0
                                            maxHeight: 1.0
                                            background: Color.create("#131d27")
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
                                    chatList.logDiagnostic("page null? " + (page === null ? ("YES (status=" + chatScreenDef.status + ", err=" + chatScreenDef.errorString() + ")") : "NO"));
                                    if (page) {
                                        page.loadChat(data.title, data.peerType, "" + data.peerId, "" + data.accessHash, data.initials, data.avatarColor, data.avatarPath ? data.avatarPath : "", chatList.canSend, rootNavPane, data.username ? data.username : "");
                                        chatList.logDiagnostic("about to push");
                                        rootNavPane.push(page);
                                        chatList.logDiagnostic("pushed");
                                    }
                                }
                            }
                        }
                    }
                }

                // -------------------------------------------------------------
                // FLOATING ACTION BUTTONS (Camera mini FAB + Compose main FAB)
                // -------------------------------------------------------------
                Container {
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Bottom
                    rightMargin: 18.0
                    bottomMargin: 76.0

                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                    // Upper Mini FAB: Camera
                    Container {
                        preferredWidth: 42.0
                        preferredHeight: 42.0
                        minWidth: 42.0
                        minHeight: 42.0
                        background: Color.create("#1e2b38")
                        horizontalAlignment: HorizontalAlignment.Center
                        bottomMargin: 12.0
                        layout: DockLayout {}
                        Label {
                            text: "\uD83D\uDCF7"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Medium
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatList.logDiagnostic("Camera FAB tapped");
                                }
                            }
                        ]
                    }

                    // Lower Main FAB: Compose New Message
                    Container {
                        preferredWidth: 56.0
                        preferredHeight: 56.0
                        minWidth: 56.0
                        minHeight: 56.0
                        background: Color.create("#24a1de")
                        horizontalAlignment: HorizontalAlignment.Center
                        layout: DockLayout {}
                        Label {
                            text: "\u270E"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Large
                            textStyle.fontWeight: FontWeight.Bold
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                        }
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    shellPage.activeSection = 2; // Open Contacts
                                }
                            }
                        ]
                    }
                }

                // -------------------------------------------------------------
                // 3-DOTS OVERFLOW POPUP MENU
                // -------------------------------------------------------------
                Container {
                    visible: chatsScreen.showOverflowMenu
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Top
                    topMargin: 56.0
                    rightMargin: 12.0
                    minWidth: 170.0
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
                            text: "Sync Chats"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            verticalAlignment: VerticalAlignment.Center
                        }
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatsScreen.showOverflowMenu = false;
                                    chatList.refreshDialogs();
                                }
                            }
                        ]
                    }

                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 38.0
                        layout: DockLayout {}
                        Label {
                            text: "About"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            verticalAlignment: VerticalAlignment.Center
                        }
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatsScreen.showOverflowMenu = false;
                                    shellPage.activeSection = 4;
                                }
                            }
                        ]
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

                MyProfileView {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    onOpenSettingsRequested: {
                        shellPage.activeSection = 3;
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
                                    page.loadChat(data.title, data.peerType, "" + data.peerId, "" + data.accessHash, data.initials, data.avatarColor, data.avatarPath ? data.avatarPath : "", chatList.canSend, rootNavPane, data.username ? data.username : "");
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

                SettingsView {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    onOpenProfileRequested: {
                        shellPage.activeSection = 1;
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
            // CUSTOM BOTTOM NAVIGATION RAIL (4 Tabs: Chats, Contacts, Settings, Profile)
            // =================================================================
            Container {
                id: bottomNavBar
                visible: auth.authState == 5
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Bottom
                minHeight: 62.0
                background: Color.create("#16202b")

                layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                // Top border line
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 1.0
                    maxHeight: 1.0
                    background: Color.create("#1e2c3a")
                }

                Container {
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    topPadding: 5.0
                    bottomPadding: 5.0
                    horizontalAlignment: HorizontalAlignment.Fill

                    // Tab 1: Chats
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    shellPage.activeSection = 0;
                                }
                            }
                        ]

                        // Active capsule / pill indicator around icon
                        Container {
                            horizontalAlignment: HorizontalAlignment.Center
                            minWidth: 54.0
                            minHeight: 28.0
                            background: shellPage.activeSection == 0 ? Color.create("#203850") : Color.Transparent
                            layout: DockLayout {}
                            Label {
                                text: "\uD83D\uDCAC"
                                textStyle.color: shellPage.activeSection == 0 ? Color.create("#24a1de") : Color.create("#7f8c99")
                                textStyle.fontSize: FontSize.Large
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
                            }
                        }
                        Label {
                            text: "Chats"
                            textStyle.color: shellPage.activeSection == 0 ? Color.create("#24a1de") : Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontWeight: shellPage.activeSection == 0 ? FontWeight.Bold : FontWeight.Normal
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 2.0
                        }
                    }

                    // Tab 2: Contacts
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    shellPage.activeSection = 2;
                                }
                            }
                        ]

                        Container {
                            horizontalAlignment: HorizontalAlignment.Center
                            minWidth: 54.0
                            minHeight: 28.0
                            background: shellPage.activeSection == 2 ? Color.create("#203850") : Color.Transparent
                            layout: DockLayout {}
                            Label {
                                text: "\uD83D\uDC64"
                                textStyle.color: shellPage.activeSection == 2 ? Color.create("#24a1de") : Color.create("#7f8c99")
                                textStyle.fontSize: FontSize.Large
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
                            }
                        }
                        Label {
                            text: "Contacts"
                            textStyle.color: shellPage.activeSection == 2 ? Color.create("#24a1de") : Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontWeight: shellPage.activeSection == 2 ? FontWeight.Bold : FontWeight.Normal
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 2.0
                        }
                    }

                    // Tab 3: Settings
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    shellPage.activeSection = 3;
                                }
                            }
                        ]

                        Container {
                            horizontalAlignment: HorizontalAlignment.Center
                            minWidth: 54.0
                            minHeight: 28.0
                            background: shellPage.activeSection == 3 ? Color.create("#203850") : Color.Transparent
                            layout: DockLayout {}
                            Label {
                                text: "\u2699"
                                textStyle.color: shellPage.activeSection == 3 ? Color.create("#24a1de") : Color.create("#7f8c99")
                                textStyle.fontSize: FontSize.Large
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
                            }
                        }
                        Label {
                            text: "Settings"
                            textStyle.color: shellPage.activeSection == 3 ? Color.create("#24a1de") : Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontWeight: shellPage.activeSection == 3 ? FontWeight.Bold : FontWeight.Normal
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 2.0
                        }
                    }

                    // Tab 4: Profile
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    shellPage.activeSection = 1;
                                }
                            }
                        ]

                        Container {
                            horizontalAlignment: HorizontalAlignment.Center
                            minWidth: 54.0
                            minHeight: 28.0
                            background: shellPage.activeSection == 1 ? Color.create("#203850") : Color.Transparent
                            layout: DockLayout {}

                            // Blue circular avatar with user initial
                            Container {
                                preferredWidth: 22.0
                                preferredHeight: 22.0
                                minWidth: 22.0
                                minHeight: 22.0
                                background: Color.create("#24a1de")
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
                                layout: DockLayout {}
                                Label {
                                    text: auth.userName.length > 0 ? auth.userName.trim().left(1).toUpperCase() : "J"
                                    textStyle.color: Color.White
                                    textStyle.fontSize: FontSize.XXSmall
                                    textStyle.fontWeight: FontWeight.Bold
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }
                        }
                        Label {
                            text: "Profile"
                            textStyle.color: shellPage.activeSection == 1 ? Color.create("#24a1de") : Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontWeight: shellPage.activeSection == 1 ? FontWeight.Bold : FontWeight.Normal
                            horizontalAlignment: HorizontalAlignment.Center
                            topMargin: 2.0
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
            source: "asset:///ChatScreen.qml"
        }
    ]
}