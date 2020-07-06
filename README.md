halsim-websocket
================

Provides a websocket interface to the WPILib HAL. Start your robot with this 
simulation extension enabled, and connect to http://localhost:8080 .

Environment variables:

* `HALSIMWEB_SYSROOT` - `/` directory of site, defaults to `$(pwd)/sim`
* `HALSIMWEB_USERROOT` - `/user` directory of site, defaults to `$(pwd)/sim/user`

install locally
---------------

    ./gradlew build publish


Add to your build.gradle
------------------------

```
repositories {
  mavenLocal()
}

// Enable simulation gui support. Must check the box in vscode to enable support
// upon debugging
dependencies {
    //simulation wpi.deps.sim.gui(, true)
    simulation "edu.wpi.first.halsim:halsim_websocket-cpp:0.0.1:${wpi.platforms.desktop}debug@zip"
}
```

Demo
----

https://github.com/virtuald/halsim-websocket-demo

Credit
------

Dustin Spicuzza created this, heavily derived from the WPILib halsim-lowfi 
sim extension created by Jaci Brunning.
