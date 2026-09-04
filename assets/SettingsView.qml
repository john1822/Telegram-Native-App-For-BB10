import bb.cascades 1.4

// =============================================================================
// TelegramBB10 — Modern Native "Settings" View
// -----------------------------------------------------------------------------
// Pixel-accurate replication of modern Telegram mobile Settings interface:
//   * Top app bar: Search button (magnifying glass) + 3-dots overflow menu.
//   * Centered profile header:
//       - 96x96dp circular avatar with user initial fallback.
//       - Camera badge overlay at bottom-right corner of avatar.
//       - Bold white contact name (auth.userName).
//       - Subtitle: "+phone • @username" (auth.userPhone / auth.userHandle).
//   * Card 1: Core Settings (#17212b):
//       - Account (Number, Username, Bio) -> opens My Profile
//       - Chat Settings (Wallpaper, Night Mode, Animations)
//       - Privacy & Security (Last Seen, Devices, Passkeys)
//       - Notifications (Sounds, Calls, Badges)
//       - Data and Storage (Media download settings)
//       - Chat Folders (Sort chats into folders)
//       - Devices (Manage connected devices)
//       - Power Saving (Reduce power usage on low charge)
//       - Language (English)
//   * Card 2: Features & Monetization (#17212b):
//       - Telegram Premium
//       - Telegram Stars
//       - Telegram Business
//       - Send a Gift
//   * Card 3: Help Section (#17212b):
//       - "Help" section header in cyan
//       - Ask a Question
//       - Telegram FAQ
//       - Telegram Features
//       - Privacy Policy
//   * Footer: version & native architecture info.
//   * 3-dots popup menu: "Edit name" and real "Log out" (auth.logout()).
// =============================================================================

Container {
    id: settingsRoot
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    background: Color.create("#0e1621")
    layout: DockLayout {}

    signal openProfileRequested()
    property bool showOverflowMenu: false

    ScrollView {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        scrollRole: ScrollRole.Main

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            bottomPadding: 74.0

            // =============================================================
            // 1. TOP APP BAR (Search Button + 3-Dots Menu Button)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                preferredHeight: 58.0
                minHeight: 54.0
                topPadding: 8.0
                bottomPadding: 4.0
                leftPadding: 12.0
                rightPadding: 8.0
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                // Spacer pushing icons to the right
                Container {
                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                }

                // Search Button (Magnifying Glass)
                Container {
                    preferredWidth: 46.0
                    preferredHeight: 46.0
                    minWidth: 46.0
                    minHeight: 46.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("Settings: Search tapped");
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_search.png"
                        preferredWidth: 26.0
                        preferredHeight: 26.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }

                // 3-Dots Menu Button
                Container {
                    preferredWidth: 46.0
                    preferredHeight: 46.0
                    minWidth: 46.0
                    minHeight: 46.0
                    verticalAlignment: VerticalAlignment.Center
                    layout: DockLayout {}
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                settingsRoot.showOverflowMenu = !settingsRoot.showOverflowMenu;
                                chatList.logDiagnostic("Settings: Overflow menu toggled: " + settingsRoot.showOverflowMenu);
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/tg_more.png"
                        preferredWidth: 24.0
                        preferredHeight: 26.0
                        scalingMethod: ScalingMethod.AspectFit
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }

            // =============================================================
            // 2. PROFILE HEADER (Avatar with Camera Badge, Name, Subtitle)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                topMargin: 2.0
                bottomMargin: 14.0

                // Circular Avatar Container with Camera Badge Overlay
                Container {
                    preferredWidth: 96.0
                    preferredHeight: 96.0
                    minWidth: 96.0
                    minHeight: 96.0
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: DockLayout {}

                    // Large circular blue avatar base
                    Container {
                        preferredWidth: 96.0
                        preferredHeight: 96.0
                        minWidth: 96.0
                        minHeight: 96.0
                        background: Color.create("#24a1de")
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

                    // Attached circular camera badge at bottom-right
                    ImageView {
                        imageSource: "asset:///images/tg_camera_badge.png"
                        preferredWidth: 32.0
                        preferredHeight: 32.0
                        minWidth: 32.0
                        minHeight: 32.0
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Bottom
                    }
                }

                // User full name
                Label {
                    text: (auth.userName && auth.userName.length > 0) ? auth.userName : "John"
                    textStyle.color: Color.White
                    textStyle.fontSize: FontSize.Large
                    textStyle.fontWeight: FontWeight.Bold
                    textStyle.textAlign: TextAlign.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    topMargin: 10.0
                }

                // Subtitle: Phone • @Username
                Label {
                    text: ((auth.userPhone && auth.userPhone.length > 0) ? auth.userPhone : "+91 8950469287") +
                          ((auth.userHandle && auth.userHandle.length > 0) ? (" \u2022 " + (auth.userHandle.indexOf("@") === 0 ? auth.userHandle : "@" + auth.userHandle)) : " \u2022 @John_the_don_420")
                    textStyle.color: Color.create("#7f8c99")
                    textStyle.fontSize: FontSize.XSmall
                    horizontalAlignment: HorizontalAlignment.Center
                    topMargin: 3.0
                }
            }

            // =============================================================
            // 3. CARD 1: CORE SETTINGS (#17212b)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#17212b")
                leftMargin: 12.0
                rightMargin: 12.0
                topMargin: 4.0
                bottomMargin: 14.0
                topPadding: 4.0
                bottomPadding: 4.0

                // 1. Account (switches to My Profile)
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                chatList.logDiagnostic("Settings: Account tapped -> opening profile");
                                settingsRoot.openProfileRequested();
                            }
                        }
                    ]
                    ImageView {
                        imageSource: "asset:///images/set_account.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Account"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Number, Username, Bio"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 2. Chat Settings
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Chat Settings tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_chats.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Chat Settings"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Wallpaper, Night Mode, Animations"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 3. Privacy & Security
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Privacy & Security tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_privacy.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Privacy & Security"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Last Seen, Devices, Passkeys"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 4. Notifications
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Notifications tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_notifications.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Notifications"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Sounds, Calls, Badges"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 5. Data and Storage
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Data and Storage tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_storage.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Data and Storage"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Media download settings"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 6. Chat Folders
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Chat Folders tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_folders.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Chat Folders"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Sort chats into folders"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 7. Devices
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Devices tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_devices.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Devices"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Manage connected devices"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 8. Power Saving
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Power Saving tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_power.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Power Saving"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "Reduce power usage on low charge"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 9. Language
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Language tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_language.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Language"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                        Label {
                            text: "English"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.XXSmall
                            topMargin: 1.0
                        }
                    }
                }
            }

            // =============================================================
            // 4. CARD 2: FEATURES & MONETIZATION (#17212b)
            // =============================================================
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#17212b")
                leftMargin: 12.0
                rightMargin: 12.0
                topMargin: 4.0
                bottomMargin: 14.0
                topPadding: 4.0
                bottomPadding: 4.0

                // 1. Telegram Premium
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Premium tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_premium.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Telegram Premium"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 2. Telegram Stars
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Stars tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_stars.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Telegram Stars"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 3. Telegram Business
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Business tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_business.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Telegram Business"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 4. Send a Gift
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Send a Gift tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_gift.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Send a Gift"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }
            }

            // =============================================================
            // 5. CARD 3: HELP SECTION (#17212b)
            // =============================================================
            Label {
                text: "Help"
                textStyle.color: Color.create("#24a1de")
                textStyle.fontSize: FontSize.XSmall
                textStyle.fontWeight: FontWeight.Bold
                leftMargin: 18.0
                topMargin: 6.0
                bottomMargin: 6.0
            }

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#17212b")
                leftMargin: 12.0
                rightMargin: 12.0
                bottomMargin: 24.0
                topPadding: 4.0
                bottomPadding: 4.0

                // 1. Ask a Question
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Ask Question tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_help_ask.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Ask a Question"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 2. Telegram FAQ
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: FAQ tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_help_faq.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Telegram FAQ"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 3. Telegram Features
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Features tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_help_features.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Telegram Features"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }

                // Divider
                Container { horizontalAlignment: HorizontalAlignment.Fill; preferredHeight: 1.0; background: Color.create("#202d3b"); leftMargin: 64.0; }

                // 4. Privacy Policy
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: 52.0
                    leftPadding: 14.0
                    rightPadding: 14.0
                    topPadding: 6.0
                    bottomPadding: 6.0
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    gestureHandlers: [ TapHandler { onTapped: { chatList.logDiagnostic("Settings: Privacy Policy tapped"); } } ]
                    ImageView {
                        imageSource: "asset:///images/set_help_policy.png"
                        preferredWidth: 36.0
                        preferredHeight: 36.0
                        minWidth: 36.0
                        minHeight: 36.0
                        scalingMethod: ScalingMethod.AspectFit
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Container {
                        layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                        leftMargin: 14.0
                        verticalAlignment: VerticalAlignment.Center
                        Label {
                            text: "Privacy Policy"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Bold
                        }
                    }
                }
            }

            // =============================================================
            // 6. FOOTER: VERSION & BUILD INFO
            // =============================================================
            Label {
                text: "Telegram for BlackBerry 10 v1.0.0 (7038)"
                textStyle.color: Color.create("#606e7d")
                textStyle.fontSize: FontSize.XXSmall
                textStyle.textAlign: TextAlign.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                text: "store bundled armv7le native"
                textStyle.color: Color.create("#606e7d")
                textStyle.fontSize: FontSize.XXSmall
                textStyle.textAlign: TextAlign.Center
                horizontalAlignment: HorizontalAlignment.Center
                topMargin: 2.0
                bottomMargin: 24.0
            }
        }
    }

    // =============================================================
    // POPUP: 3-DOTS OVERFLOW MENU
    // =============================================================
    Container {
        visible: settingsRoot.showOverflowMenu
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Top
        topMargin: 56.0
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
                        settingsRoot.showOverflowMenu = false;
                        chatList.logDiagnostic("Settings: Edit name tapped");
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
                        settingsRoot.showOverflowMenu = false;
                        auth.logout();
                    }
                }
            ]
        }
    }
}
