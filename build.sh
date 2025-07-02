export ANDROID_HOME="/home/harry/Android/Sdk/"
export ANDROID_NDK_ROOT="/home/harry/Android/Sdk/ndk/25.1.8937393/"
#build for linux release
#scons platform=linuxbsd dev_build=no target=template_release tools=no use_lto=yes module_websocket_enable=no module_upnp_enable=no module_webrtc_enable=no module_openxr_enabled=no -j12
#build for android release
scons platform=android dev_build=no target=template_release tools=no use_lto=yes module_websocket_enable=no module_upnp_enable=no module_webrtc_enable=no module_openxr_enabled=no -j12
#build for embedded dev
# scons platform=embedded shared=yes dev_build=yes target=template_debug tools=no use_lto=yes module_websocket_enable=no module_upnp_enable=no module_webrtc_enable=no module_openxr_enabled=no -j12