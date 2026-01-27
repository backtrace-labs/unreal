# Technical notes

Features: This plugin will update default and platform-specific DataRouteURL to send crashes to Sauce Labs Error Reporting projects.

Code Modules: only one Editor module: SauceReport

Number of Blueprints: 0

Number of C++ Classes: 5

Network Replicated: No

Supported Development Platforms: Windows (Yes), Mac (Yes)

Supported Target Build Platforms: Any platform since it's an Editor module, it's not included in builds (it modifies the engine crash reporting settings that goes in builds but not the plugin itself).

Documentation Link: <https://docs.saucelabs.com/error-reporting/platform-integrations/unreal/setup/> (Production documentation will be updated when this plugin is published. Draft documentation: <https://github.com/backtrace-labs/unreal#>)

Example Projects:

- 5.0 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.0.zip>
- 5.1 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.1.zip>
- 5.2 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.2.zip>
- 5.3 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.3.zip>
- 5.4 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.4.zip>
- 5.5 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.5.zip>
- 5.6 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.6.zip>
- 5.7 <https://github.com/backtrace-labs/unreal/blob/master/verification/SauceReportProjectUE5.7.zip>

Important/Additional Notes:

Send/Close & Send/Restart simply behave normally with no additional reporting once completed.

Please verify that the checklist from the UE Editor → Tools → Error Reporting is green

Please note that this plugin does not include anything in the builds, so the crash reporter is just sending the report to a SauceLabs server rather than the Epic report server. There is no change in the functionality of the runtime.
