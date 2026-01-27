# Sauce Labs Error Reporting for Unreal Engine

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.0%20--%205.7-blue)](https://www.unrealengine.com/)
[![Platform](https://img.shields.io/badge/Platform-Win64%20%7C%20Mac%20%7C%20Linux-lightgrey)](https://saucelabs.com/)
[![License](https://img.shields.io/badge/License-Proprietary-red)](https://github.com/backtrace-labs/unreal/blob/master/LICENSE)

Integrate Sauce Labs Error Reporting into your Unreal Engine projects to automatically capture and report crashes, helping you identify and fix issues faster.

## 📋 Table of Contents

- [Features](#-features)
- [Requirements](#-requirements)
- [Installation](#-installation-from-fab)
- [Configuration](#-configuration)
- [Usage](#-usage)
- [Platform-Specific Settings](#-platform-specific-settings)
- [Troubleshooting](#-troubleshooting)
- [Documentation](#-documentation)
- [Support](#-support)

## ✨ Features

- **Automated Crash Reporting**: Automatically capture and send crash reports to Sauce Labs
- **Multi-Platform Support**: Configure different reporting endpoints for Windows, Mac, Linux, Android, iOS, and more
- **Configuration Validation**: Built-in checklist UI to verify your setup
- **Editor Integration**: Seamlessly integrates into the Unreal Editor Tools menu
- **URL Validation**: Ensures reporting endpoints are correctly formatted
- **One-Click Fixes**: Automatically enable crash reporter in packaging settings

## 📦 Requirements

### Unreal Engine Version

- Unreal Engine 5.0 or later

### Sauce Labs Error Reporting Account and DataRouterURL

- Create Sauce Labs Error Reporting account at [backtrace.io](backtrace.io)
- Locate DataRouter URL for your project in **Project Settings** → **Integration Guides** → **Unreal**

### Development Platform

- Windows 64-bit, macOS, or Linux

## 🚀 Installation from Fab

1. Open the Epic Games Launcher/Your UE project
2. Navigate to the fab window
3. Search for "Sauce Labs Error Reporting"
4. Click **Install to Engine**
5. Open/Restart your UE project
6. Go to **Edit → Plugins**
7. Search for "SauceLabs Error Reporting"
8. Check the **Enabled** checkbox
9. Restart the editor if prompted

## 🔳 Configuration

### Step 1: Obtain Your DataRouter URL

1. Log in to your [Sauce Labs Error Reporting](https://backtrace.io/) universe
2. Navigate to **Project Settings** → **Integration Guides** → **Unreal**
3. Copy your project's **DataRouter URL** for Step 2
   1. You may create unique projects per-platform, each with a unique DataRouterURL

### Step 2: Configure & Check with the Plugin Tool

1. In the Unreal Editor, open **Tools → Error Reporting**
2. Presh Refresh button
3. Enable the Crash Reporting
4. Press the SauceLabs Reporting or Navigate to **Plugins → Sauce Labs Error Reporting**
5. Paste your DataRouter URL into the **Default Data Router URL** field
6. Go back to the SauceLabs checklist window and press Refresh, check that everything is ok (green)
   - ✅ DataRouterUrl in DefaultEngine.ini
   - ✅ DefaultEngine.ini is writable
   - ✅ Include Crash Reporter (packaging setting)
   - If "Include Crash Reporter" shows red, click **Enable Crash Reporter** to fix it automatically

## ⏩ Usage

Once configured, crash reports will be automatically sent to Sauce Labs when:

- Your packaged game crashes on end-user devices
- A fatal error occurs in the game

1. Make a build of your game
2. Simulate a crash
3. Go to your [Sauce Labs Error Reporting](https://backtrace.io/) universe to see crash reports
   1. Upload symbols in **Project Settings** → **Upload and archive** to resolve symbols in callstacks

### What Gets Reported

- **Crash Callstack**: Full stack trace of the crash
- **Engine Version**: Unreal Engine version
- **Platform**: Target platform (Windows, Mac, Linux, Android, iOS, etc.)
- **Build Configuration**: Development, Shipping, etc.
- **Custom Context**: Any additional data you've logged

## 🌍 Platform-Specific Settings

You can configure unique Error Reporting projects and DataRouterURLs for each target platform:

1. In Error Reporting, create a new project and get DataRouterURL in **Project Settings** → **Integration Guides** → **Unreal**
1. In Unreal Engine, open **Edit → Project Settings → Plugins → Sauce Labs Error Reporting**
1. Expand **Platforms To Data Router URL**
1. Click **+** to add a platform
1. Select platform (Windows, Android, iOS, etc.)
1. Enter platform-specific DataRouter URL
1. **Save** is done automatically

The plugin will automatically create platform-specific configuration files:

- `Config/Windows/WindowsEngine.ini`
- `Config/Android/AndroidEngine.ini`
- `Config/IOS/IOSEngine.ini`

### Verification Checklist

- [ ] DataRouterURL is correctly formatted
- [ ] Configuration checklist shows all green
- [ ] Include Crash Reporter is enabled
- [ ] Packaged build sends test crash successfully
- [ ] Crash appears in Sauce Labs Portal

## 🔧 Troubleshooting

### Crash Reports Not Appearing

**Problem**: Crashes aren't showing up in Sauce Labs Portal

**Solutions**:

1. Verify DataRouterURL format: `https://unreal.backtrace.io/post/[universe]/[token]`
2. Check **Tools → Crash Reporter → Error Reporting** for configuration issues
3. Ensure "Include Crash Reporter" is enabled in packaging settings
4. Check that INI files are not read-only:
   - `Config/DefaultEngine.ini`
   - `Config/DefaultGame.ini`

### Configuration Not Saving

**Problem**: Settings don't persist after clicking Save

**Solutions**:

1. Check if configuration files are read-only
2. Ensure your project directory has write permissions
3. Close other applications that might lock INI files (source control, editors)
4. Click **Refresh** in the checklist to reload configuration

### Platform-Specific URL Not Working

**Problem**: Platform override isn't being used

**Solutions**:

1. Verify platform name matches exactly (case-sensitive)
1. Check that platform INI file was created in `Config/[Platform]/`
1. Ensure DataRouterURL format is valid
1. Re-package your game after making changes

### Plugin Not Appearing in Tools Menu

**Problem**: Can't find "Error Reporting" in Tools menu

**Solutions**:

1. Verify plugin is enabled in **Edit → Plugins**
1. Restart Unreal Editor
1. Check that you're using a compatible UE version (5.0+)

### URL Validation Failing

**Problem**: Valid DataRouterURL is rejected

**Solutions**:

1. Check DataRouterURL format: `https://unreal.backtrace.io/post/[universe]/[token]`
1. Remove any trailing slashes or extra parameters
1. Verify URL was copied correctly from Error Reporting

## 📚 Documentation

- [Sauce Labs Error Reporting Documentation](https://docs.saucelabs.com/)
- [Unreal Engine Crash Reporter](https://docs.unrealengine.com/en-US/TestingAndOptimization/CrashReporter/)

## 🤝 Support

- **Documentation**: [Setting Up Backtrace for Unreal Engine](https://docs.saucelabs.com/error-reporting/platform-integrations/unreal/setup/)
- **Contact Support**: [https://saucelabs.com/contact-us](https://saucelabs.com/contact-us)
- **Fab Marketplace Page**: [View on Fab](https://fab.com/listings/4038fd34-96b1-4922-a0bb-2eef4c454f27)

## 📄 License

Copyright © Sauce Labs Inc. All Rights Reserved.

This plugin is proprietary software. See [LICENSE](https://github.com/backtrace-labs/unreal/blob/master/LICENSE) file for details.

---

**Made by [Sauce Labs Inc.](https://saucelabs.com/)**