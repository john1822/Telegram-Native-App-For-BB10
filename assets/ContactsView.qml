import bb.cascades 1.4

// =============================================================================
// TelegramBB10 — Modern Native "Contacts" View
// -----------------------------------------------------------------------------
// Pixel-accurate replication of modern Telegram mobile Contacts interface:
//   * Top app bar: Bold "Contacts" title + Sort button (tg_sort.png).
//   * Search Contacts field: Rounded dark bar with search icon.
//   * Action Card (#17212b):
//       - "Invite Friends" with cyan badge (set_invite.png).
//       - "Recent calls" with green badge (set_calls.png).
//   * Section Label: "Sorted by last seen time" in cyan/blue (#5294e2).
//   * Contacts List Card (#17212b):
//       - Real-time MTProto contacts from chatList.contactsModel.
//       - Circular avatar with initials / photo.
//       - Contact name + "last seen recently" status.
//       - Tap contact -> opens full ChatScreen.
//   * Floating Action Button (FAB):
//       - Cyan 56x56dp circular button with tg_fab_add.png icon.
// =============================================================================

Container {
    id: contactsRoot
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    background: Color.create("#0e1621")
    layout: DockLayout {}

    signal contactSelected(variant contactData)

    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        // =============================================================
        // 1. TOP APP BAR ("Contacts" + Sort Button)
        // =============================================================
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            minHeight: 56.0
            topPadding: 10.0
            bottomPadding: 6.0
            leftPadding: 16.0
            rightPadding: 14.0
            layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

            Label {
                text: "Contacts"
                textStyle.color: Color.White
                textStyle.fontSize: FontSize.Large
                textStyle.fontWeight: FontWeight.Bold
                verticalAlignment: VerticalAlignment.Center
                layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
            }

            // Sort Button (3 horizontal bars + A)
            Container {
                preferredWidth: 48.0
                preferredHeight: 48.0
                minWidth: 48.0
                minHeight: 48.0
                verticalAlignment: VerticalAlignment.Center
                layout: DockLayout {}
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            chatList.logDiagnostic("Contacts: Sort button tapped");
                        }
                    }
                ]
                ImageView {
                    imageSource: "asset:///images/tg_sort.png"
                    preferredWidth: 32.0
                    preferredHeight: 32.0
                    scalingMethod: ScalingMethod.AspectFit
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
            }
        }

        // =============================================================
        // 2. SEARCH CONTACTS FIELD
        // =============================================================
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            background: Color.create("#17212b")
            leftMargin: 12.0
            rightMargin: 12.0
            topMargin: 2.0
            bottomMargin: 10.0
            minHeight: 48.0
            topPadding: 2.0
            bottomPadding: 2.0
            leftPadding: 14.0
            rightPadding: 14.0
            layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

            ImageView {
                imageSource: "asset:///images/tg_search.png"
                preferredWidth: 26.0
                preferredHeight: 26.0
                scalingMethod: ScalingMethod.AspectFit
                verticalAlignment: VerticalAlignment.Center
            }

            TextField {
                id: contactsSearchInput
                hintText: "Search Contacts"
                backgroundVisible: false
                textStyle.color: Color.White
                textStyle.fontSize: FontSize.Small
                verticalAlignment: VerticalAlignment.Center
                layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                onTextChanging: {
                    chatList.contactsSearchQuery = text;
                }
            }

            // Clear search button
            Container {
                visible: contactsSearchInput.text.length > 0
                preferredWidth: 36.0
                preferredHeight: 36.0
                verticalAlignment: VerticalAlignment.Center
                layout: DockLayout {}
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            contactsSearchInput.text = "";
                            chatList.contactsSearchQuery = "";
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

        // =============================================================
        // SCROLLABLE CONTENT: ACTIONS + CONTACTS LIST
        // =============================================================
        ScrollView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
            scrollRole: ScrollRole.Main

            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                bottomPadding: 80.0

                // =============================================================
                // 3. ACTIONS CARD ("Invite Friends" + "Recent calls")
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    background: Color.create("#17212b")
                    leftMargin: 12.0
                    rightMargin: 12.0
                    topMargin: 2.0
                    bottomMargin: 10.0
                    topPadding: 4.0
                    bottomPadding: 4.0

                    // Row 1: Invite Friends
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        leftPadding: 14.0
                        rightPadding: 14.0
                        topPadding: 6.0
                        bottomPadding: 6.0
                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatList.logDiagnostic("Contacts: Invite Friends tapped");
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/set_invite.png"
                            preferredWidth: 44.0
                            preferredHeight: 44.0
                            minWidth: 44.0
                            minHeight: 44.0
                            scalingMethod: ScalingMethod.AspectFit
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Label {
                            text: "Invite Friends"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Medium
                            textStyle.fontWeight: FontWeight.Bold
                            leftMargin: 16.0
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }

                    // Divider
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        preferredHeight: 1.0
                        background: Color.create("#202d3b")
                        leftMargin: 74.0
                    }

                    // Row 2: Recent calls
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        minHeight: 56.0
                        leftPadding: 14.0
                        rightPadding: 14.0
                        topPadding: 6.0
                        bottomPadding: 6.0
                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                        gestureHandlers: [
                            TapHandler {
                                onTapped: {
                                    chatList.logDiagnostic("Contacts: Recent calls tapped");
                                }
                            }
                        ]
                        ImageView {
                            imageSource: "asset:///images/set_calls.png"
                            preferredWidth: 44.0
                            preferredHeight: 44.0
                            minWidth: 44.0
                            minHeight: 44.0
                            scalingMethod: ScalingMethod.AspectFit
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Label {
                            text: "Recent calls"
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.Medium
                            textStyle.fontWeight: FontWeight.Bold
                            leftMargin: 16.0
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }
                }

                // =============================================================
                // 4. SECTION HEADER ("Sorted by last seen time")
                // =============================================================
                Label {
                    text: "Sorted by last seen time"
                    textStyle.color: Color.create("#5294e2")
                    textStyle.fontSize: FontSize.Small
                    textStyle.fontWeight: FontWeight.Bold
                    leftMargin: 18.0
                    topMargin: 8.0
                    bottomMargin: 6.0
                }

                // =============================================================
                // 5. CONTACTS LIST CARD (#17212b)
                // =============================================================
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    background: Color.create("#17212b")
                    leftMargin: 12.0
                    rightMargin: 12.0
                    topMargin: 2.0
                    bottomMargin: 16.0
                    topPadding: 4.0
                    bottomPadding: 4.0

                    // Empty state when no contacts loaded yet
                    Container {
                        visible: chatList.contactsCount == 0
                        horizontalAlignment: HorizontalAlignment.Center
                        topPadding: 24.0
                        bottomPadding: 24.0
                        Label {
                            text: "No contacts found"
                            textStyle.color: Color.create("#7f8c99")
                            textStyle.fontSize: FontSize.Small
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                    }

                    // Real-Time Contacts ListView
                    ListView {
                        visible: chatList.contactsCount > 0
                        horizontalAlignment: HorizontalAlignment.Fill
                        dataModel: chatList.contactsModel

                        listItemComponents: [
                            ListItemComponent {
                                type: "item"
                                Container {
                                    id: contactRowRoot
                                    horizontalAlignment: HorizontalAlignment.Fill

                                    Container {
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        minHeight: 58.0
                                        topPadding: 8.0
                                        bottomPadding: 8.0
                                        leftPadding: 14.0
                                        rightPadding: 14.0
                                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                                        // Circular Avatar (48 x 48)
                                        Container {
                                            verticalAlignment: VerticalAlignment.Center
                                            preferredWidth: 48.0
                                            preferredHeight: 48.0
                                            minWidth: 48.0
                                            minHeight: 48.0
                                            maxWidth: 48.0
                                            maxHeight: 48.0
                                            rightMargin: 14.0
                                            layout: DockLayout {}

                                            // Fallback Colored Circle with Initials
                                            Container {
                                                visible: !ListItemData.avatarPath || ListItemData.avatarPath.length == 0
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                verticalAlignment: VerticalAlignment.Fill
                                                background: Color.create(ListItemData.avatarColor ? ListItemData.avatarColor : "#24a1de")
                                                layout: DockLayout {}
                                                Label {
                                                    text: ListItemData.initials ? ListItemData.initials : "?"
                                                    textStyle.color: Color.White
                                                    textStyle.fontSize: FontSize.Medium
                                                    textStyle.fontWeight: FontWeight.Bold
                                                    horizontalAlignment: HorizontalAlignment.Center
                                                    verticalAlignment: VerticalAlignment.Center
                                                }
                                            }

                                            // Actual Contact Avatar Photo
                                            ImageView {
                                                visible: ListItemData.avatarPath && ListItemData.avatarPath.length > 0
                                                imageSource: ListItemData.avatarPath ? (ListItemData.avatarPath.indexOf("file://") === 0 ? ListItemData.avatarPath : "file://" + ListItemData.avatarPath) : ""
                                                preferredWidth: 48.0
                                                preferredHeight: 48.0
                                                scalingMethod: ScalingMethod.AspectFill
                                                horizontalAlignment: HorizontalAlignment.Center
                                                verticalAlignment: VerticalAlignment.Center
                                            }
                                        }

                                        // Contact Name & Status
                                        Container {
                                            verticalAlignment: VerticalAlignment.Center
                                            layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }
                                            layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                                            Label {
                                                text: ListItemData.title ? ListItemData.title : ""
                                                textStyle.color: Color.White
                                                textStyle.fontSize: FontSize.Small
                                                textStyle.fontWeight: FontWeight.Bold
                                            }

                                            Label {
                                                text: ListItemData.status ? ListItemData.status : "last seen recently"
                                                textStyle.color: (ListItemData.status && ListItemData.status.indexOf("online") === 0) ? Color.create("#6cb5f3") : Color.create("#7f8c99")
                                                textStyle.fontSize: FontSize.XXSmall
                                                topMargin: 2.0
                                            }
                                        }
                                    }

                                    // Divider between contact items
                                    Container {
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        preferredHeight: 1.0
                                        background: Color.create("#202d3b")
                                        leftMargin: 76.0
                                    }
                                }
                            }
                        ]

                        onTriggered: {
                            var data = chatList.contactsModel.data(indexPath);
                            if (data) {
                                contactsRoot.contactSelected(data);
                            }
                        }
                    }
                }
            }
        }
    }

    // =============================================================
    // 6. FLOATING ACTION BUTTON (FAB) - Cyan Circular "+ Contact"
    // =============================================================
    Container {
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Bottom
        rightMargin: 16.0
        bottomMargin: 72.0

        Container {
            preferredWidth: 62.0
            preferredHeight: 62.0
            minWidth: 62.0
            minHeight: 62.0
            maxWidth: 62.0
            maxHeight: 62.0
            layout: DockLayout {}
            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        chatList.logDiagnostic("Contacts: Add Contact FAB tapped");
                    }
                }
            ]

            ImageView {
                imageSource: "asset:///images/tg_fab_button.png"
                preferredWidth: 62.0
                preferredHeight: 62.0
                scalingMethod: ScalingMethod.AspectFit
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
            }
        }
    }
}
