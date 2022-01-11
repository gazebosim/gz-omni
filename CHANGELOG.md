103.1
* Samples
    * Added omniUsdReader, a very very simple program for build config demonstration that opens a stage and traverses it, printing all of the prims
    * Added omniUsdaWatcher, a live USD watcher that outputs a constantly updating USDA file on disk
    * Updated the nv-usd library to one with symbols so the Visual Studio Debug Visualizers work properly
* Omniverse Client Library
    * Still using 1.13.19

102.1
* Samples
    * OM-31648: Add a windows build tool configuration utility if the user wants to use an installed MSVC and the Windows SDK
    * Add a dome light with texture to the stage
    * OM-35991: Modify the MDL names and paths to reduce some code redundancy based on a forum post
    * Add Nucleus checkpoints to the Python sample
    * Avoid writing Nucleus checkpoints when live mode is enabled, this isn't supported properly
    * OM-37005: Fix a bug in the Python sample batch file if the sample was installed in a path with spaces
    * Make the /Root prim the `defaultPrim`
    * Update Omniverse Client Library to 1.13.19
* Omniverse Client Library
    * 1.13.19
        * OM-36925: Fix omniClientMakeRelative("omni://host/path/", "omni://host/path/");
    * 1.13.18
        * OM-25931: Fixed some issues around changing and calling the log callback to reduce hangs.
        * OM-36755: Fixed possible use-after-delete issue with set_log_callback (Python).
    * 1.13.17
        * OM-34879: Hard-code "mdl" as "not a layer" to work around a problem that happens if the "usdMdl" plugin is loaded
    * 1.13.16
        * OM-36756: Fix crash that could happen if two threads read a layer at the exact same time.
    * 1.13.15
        * OM-35235: Fix various hangs by changing all bindings to release the GIL except in very specific cases.
    * 1.13.14
        * OM-34879: Fix hang in some circumstances by delaying USD plugin registration until later
        * OM-33732: Remove USD diagnostic delegate
    * 1.13.13
        * OM-36256: Fixed S3 provider from generating a bad AWS signature when Omni Cache is enabled
    * 1.13.12
        * OM-20572: Fixed setAcls
    * 1.13.11
        * OM-35397: Fixed a bug that caused Linux's File Watcher Thread to peg the CPU in some cases.
    * 1.13.10
        * OM-32244: Fixed a very rare crash that could occur when reading a local file that another process has locked
    * 1.13.9
        * OM-35050: Fixed problem reloading a non-live layer after it's been modified.
    * 1.13.8
        * OM-34739: Fix regression loading MDLs introduced in     * 1.13.3
        * OM-33949: makeRelativeUrl prepends "./" to relative paths
        * OM-34752: Make sure local paths are always using "" inside USD on Windows
    * 1.13.7
        * OM-34696: Fixed bug when S3 + cloudfront + omni cache are all used
    * 1.13.6
        * OM-33914: Fixed crash when accessing http provider from mulitple threads simultaneously
    * 1.13.5
        * OM-26039: Fixed "Restoring checkpoint while USD stage is opened live wipes the content"
        * OM-33753: Fixed "running massive amounts of live edits together causes massive amounts of checkpoints"
        * OM-34432: Fixed "[Create] It will lose all data or hang Create in live session"
            * These were all the same underlying issue: When a layer is overwritten in live mode it was cleared and set as 'dirty' which would cause the next "Save()" (which happens every frame in live mode) to save the cleared layer back to the Omniverse server.
    * 1.13.4
        * OM-31830: omniClientCopy() with HTTP/S3 provider as source
        * OM-33321: Use Omni Cache 2.4.1+ new reverse proxy feature for HTTPS caching
    * 1.13.3
        * OM-33483: Don't crash when trying to save a layer that came from a mount
        * OM-27233: Support loading non-USD files (abc, drc, etc)
        * OM-4613 & OM-34150: Support saving usda files as ascii
            * Note this change means live updates no longer work with usda files (though they technically never did -- it would silently convert them to usdc files).

101.1

* Add Linux package for the Omniverse Launcher
* Add a python 3 Hello World sample
* Update the Omniverse Client Library to 1.13.2
* Update to Python 3.7
* Add a Nucleus Checkpoint example
* Add the ability to create/access a USD stage on local disk in the Hello World sample

100.2

* Update the Omniverse Client Library fix an issue with overlapping file writes

100.1

* First release
* HelloWorld sample that demonstrates how to:
    * connect to an Omniverse server
    * create a USD stage
    * create a polygonal box and add it to the stage
    * upload an MDL material and its textures to an Omniverse server
    * bind an MDL and USD Preview Surface material to the box
    * add a light to the stage
    * move and rotate the box with live updates
    * print verbose Omniverse logs
    * open an existing stage and find a mesh to do live edits
* OmniCLI sample that exercises most of the Omniverse Client Library API