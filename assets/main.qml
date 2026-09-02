import bb.cascades 1.4

Page {
    id: rootPage

    Container {
        layout: DockLayout {}
        background: Color.create("#0e1621") // 2026 Telegram Deep Dark Background

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            layout: StackLayout {}

            // 1. Modern 2026 Telegram Header
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#17212b")
                topPadding: 16.0
                bottomPadding: 16.0
                leftPadding: 20.0
                rightPadding: 20.0

                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                Container {
                    verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1.0
                    }

                    Label {
                        text: "Telegram MTProto 2.0"
                        textStyle.color: Color.create("#ffffff")
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.Bold
                    }

                    Label {
                        text: "Native BlackBerry 10 Client Engine"
                        textStyle.color: Color.create("#708499")
                        textStyle.fontSize: FontSize.XSmall
                    }
                }

                // Connection Status Pill
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    background: diagnostic.isEncrypted ? Color.create("#2ea043") : (diagnostic.isConnected ? Color.create("#2481cc") : Color.create("#cf222e"))
                    topPadding: 6.0
                    bottomPadding: 6.0
                    leftPadding: 12.0
                    rightPadding: 12.0
                    
                    Label {
                        text: diagnostic.isEncrypted ? "ENCRYPTED" : (diagnostic.isConnected ? "CONNECTING" : "OFFLINE")
                        textStyle.color: Color.White
                        textStyle.fontSize: FontSize.XXSmall
                        textStyle.fontWeight: FontWeight.Bold
                    }
                }
            }

            // Scrollable Content Area
            ScrollView {
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1.0
                }
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    topPadding: 12.0
                    bottomPadding: 12.0
                    leftPadding: 12.0
                    rightPadding: 12.0

                    // 2. MTProto 2.0 Session Status Card
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        background: Color.create("#17212b")
                        topPadding: 14.0
                        bottomPadding: 14.0
                        leftPadding: 16.0
                        rightPadding: 16.0
                        bottomMargin: 10.0

                        Label {
                            text: "SESSION & PROTOCOL STATUS"
                            textStyle.color: Color.create("#5288c1")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontWeight: FontWeight.Bold
                        }

                        Label {
                            text: diagnostic.statusText
                            textStyle.color: Color.create("#f5f5f5")
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.Medium
                            multiline: true
                        }

                        Divider {}

                        Label {
                            text: "Auth Key ID: " + diagnostic.authKeyId
                            textStyle.color: Color.create("#708499")
                            textStyle.fontSize: FontSize.XSmall
                        }

                        Label {
                            text: diagnostic.dcInfo
                            textStyle.color: Color.create("#708499")
                            textStyle.fontSize: FontSize.XSmall
                        }
                    }

                    // 3. Action Buttons
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        bottomMargin: 10.0
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
                            text: "Test RPC (getNearestDc)"
                            enabled: diagnostic.isEncrypted
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 1.5
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

                    // 4. Live Diagnostic Log Console Card
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        background: Color.create("#17212b")
                        topPadding: 12.0
                        bottomPadding: 12.0
                        leftPadding: 14.0
                        rightPadding: 14.0

                        Label {
                            text: "LIVE MTPROTO PROTOCOL LOGS"
                            textStyle.color: Color.create("#5288c1")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontWeight: FontWeight.Bold
                        }

                        TextArea {
                            text: diagnostic.logsText
                            editable: false
                            textStyle.color: Color.create("#00ff66")
                            textStyle.fontSize: FontSize.XXSmall
                            textStyle.fontFamily: "Courier, monospace"
                            backgroundVisible: false
                            minHeight: 280.0
                        }
                    }
                }
            }
        }
    }
}
